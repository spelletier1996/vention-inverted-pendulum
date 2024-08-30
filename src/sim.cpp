#include "inverted_pendulum.hpp"
#include "pid.hpp"
#include "tools.hpp"
#include <chrono>
#include <network.hpp>
#include <signal.h>
#include <stdio.h>
#include <thread>

bool terminate = false;

void SignalHandler([[maybe_unused]] int sig) { terminate = true; }

int main() {
  signal(SIGINT, SignalHandler);
  // Create local objects to store shared memory
  utils::SimState state;
  utils::SimCommand command;

  // Setup sim with default initial conditions
  const double p_0 = 0;
  const double theta_0 = 5;
  Eigen::VectorXd x_0(4);
  x_0 << p_0, tools::to_radians(theta_0), 0, 0;
  InvertedPendulum simulator(x_0);

  // Create a PID controller to control the velocity
  utils::PID velocity_controller(10, 1, 0);

  // Open the controllers shared memory objects
  utils::Client<utils::SimState> state_server("sim_state");
  utils::Client<utils::SimCommand> command_server("sim_command");

  // Create local variables
  double present_velocity = 0;
  double force_input = 0;

  while (!terminate) {
    // Get the latest command from the controller
    command = command_server.Read();

    // Get the latest velocity state
    present_velocity = simulator.State()(2);

    // Update the velocity controller and feed it into the sim
    velocity_controller.UpdateError(.001, command.velocity - present_velocity);
    force_input = velocity_controller.TotalError();
    simulator.Update(.001, force_input, command.disturbance);

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