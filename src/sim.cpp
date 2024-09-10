#include <chrono>
#include <csignal>
#include <cstdio>
#include <network.hpp>
#include <thread>
#include "inverted_pendulum.hpp"
#include "pid.hpp"
#include "tools.hpp"
#include "typedefs.hpp"

bool terminate = false;

void SignalHandler([[maybe_unused]] int sig) {
  terminate = true;
}

auto main() -> int {
  signal(SIGINT, SignalHandler);
  // Create local objects to store shared memory
  network::SimState state;
  network::SimCommand command;

  // Setup sim with default initial conditions
  const double p_0 = 0;
  const double theta_0 = 5;
  Eigen::VectorXd x_0(4);
  x_0 << p_0, tools::DegToRad(theta_0), 0, 0;
  model::InvertedPendulum simulator(x_0);

  // Create a PID controller to control the velocity
  controller::PID velocity_controller(10, 1, 0);

  // Open the controllers shared memory objects
  network::Client<network::SimState> state_server("sim_state");
  network::Client<network::SimCommand> command_server("sim_command");
  network::Client<bool> reset("reset_signal");

  // Create local variables
  double present_velocity = 0;
  double force_input = 0;

  while (!terminate) {
    // Reset the sim and all related variables
    if (reset.Read()) {
      printf("\nResetting Simulator\n");
      simulator.Restart();
      state.position = 0;
      state.angle = 0;
      state.velocity = 0;
      state.angular_velocity = 0;
      present_velocity = 0;
      force_input = 0;
      velocity_controller.Reset();
      state_server.Write(state);
      // small delay to sync with controller
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    // Get the latest command from the controller
    command = command_server.Read();

    // Get the latest velocity state
    present_velocity = simulator.State()(2);

    // Update the velocity controller and feed it into the sim
    velocity_controller.UpdateError(.001, command.velocity - present_velocity);
    force_input = velocity_controller.TotalError();
    simulator.Update(.001, force_input, command.disturbance);

    // Limit the position for simplicity
    if (simulator.State()(0) > 10) {
      simulator.Position(10);
    } else if (simulator.State()(0) < -10) {
      simulator.Position(-10);
    }

    // update and write the new state
    state.position = simulator.State()(0);
    state.angle = simulator.State()(1);
    state.velocity = simulator.State()(2);
    state.angular_velocity = simulator.State()(3);
    state_server.Write(state);

    // Run the sim at 1000hz
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  printf("\nExiting sim\n");
  return 0;
}