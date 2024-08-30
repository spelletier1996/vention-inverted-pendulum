#include "pid.hpp"
#include <chrono>
#include <memory>
#include <network.hpp>
#include <signal.h>
#include <stdio.h>
#include <thread>

bool terminate = false;

void SignalHandler([[maybe_unused]] int sig) { terminate = true; }

int main() {
  signal(SIGINT, SignalHandler);
  // Set initial conditions
  utils::SimState state;
  utils::SimCommand command;
  utils::ControllerSettings settings;
  // Create the controllers with initial conditions
  auto angle_controller = std::make_shared<utils::PID>(50, 0, 10);
  auto position_controller = std::make_shared<utils::PID>(5, 0, 5);

  // Create the shared memory objects for communication with the HMI and
  // Simulation
  utils::Server<utils::SimState> state_server("sim_state");
  utils::Server<utils::SimCommand> command_server("sim_command");
  utils::Server<utils::ControllerSettings> settings_server(
      "controller_settings");

  while (!terminate) {
    // Read all the shared memory objects
    state = state_server.Read();
    settings = settings_server.Read();

    auto position_error = 0 - state.position;

    position_controller->UpdateError(.01, position_error);

    angle_controller->UpdateError(.01, 0 - state.angle);

    // control_velocity = position_controller->TotalError();
    command.velocity =
        angle_controller->TotalError() - position_controller->TotalError();

    command_server.Write(command);

    printf("position = %f\n", state.position);
    printf("position_error = %f\n", position_controller->TotalError());

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
  printf("\nExiting controller\n");
  return 0;
}