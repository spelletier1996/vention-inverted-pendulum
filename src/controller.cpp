#include "pid.hpp"
#include <chrono>
#include <iostream>
#include <memory>
#include <network.hpp>
#include <signal.h>
#include <stdio.h>
#include <thread>
#include <unistd.h>

bool terminate = false;

// default gains for a stable system
constexpr double default_position_Kp = 5;
constexpr double default_position_Ki = 0;
constexpr double default_position_Kd = 5;
constexpr double default_angle_Kp = 50;
constexpr double default_angle_Ki = 10;
constexpr double default_angle_Kd = 20;

void SignalHandler([[maybe_unused]] int sig) { terminate = true; }

int main() {
  signal(SIGINT, SignalHandler);
  // Set initial conditions
  network::SimState state{0, 0, 0, 0};
  network::SimCommand command{0, 0, false};
  network::ControllerSettings settings{default_position_Kp, default_position_Ki,
                                       default_position_Kd, default_angle_Kp,
                                       default_angle_Ki,    default_angle_Kd};
  // Create the controllers with initial conditions
  auto angle_controller = std::make_shared<controller::PID>(
      default_angle_Kp, default_angle_Ki, default_angle_Kd);
  auto position_controller = std::make_shared<controller::PID>(
      default_position_Kp, default_position_Ki, default_position_Kd);

  // Create the shared memory objects for communication with the HMI and
  // Simulation
  network::Server<network::SimState> state_server("sim_state");
  network::Server<network::SimCommand> command_server("sim_command");
  network::Server<network::ControllerSettings> settings_server(
      "controller_settings");
  network::Server<bool> reset("reset_signal");

  // Write config to shared memory
  settings_server.Write(settings);

  printf("\nStarting controller\n");

  // Main loop
  while (!terminate) {
    // Check for reset signal and reset the controller
    if (reset.Read()) {
      printf("\nResetting controller\n");
      position_controller->Reset(default_position_Kp, default_position_Ki,
                                 default_position_Kd);
      angle_controller->Reset(default_angle_Kp, default_angle_Ki,
                              default_angle_Kd);
      command.velocity = 0;
      command.disturbance = 0;
      command.reset = false;
      settings.angle_Kp = default_angle_Kp;
      settings.angle_Ki = default_angle_Ki;
      settings.angle_Kd = default_angle_Kd;
      settings.position_Kp = default_position_Kp;
      settings.position_Ki = default_position_Ki;
      settings.position_Kd = default_position_Kd;
      state.position = 0;
      state.angle = 0;
      state.velocity = 0;
      state.angular_velocity = 0;
      settings_server.Write(settings);
      command_server.Write(command);
      state_server.Write(state);
      // Small delay to sync with sim
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
      reset.Write(false);
    }

    // Read all the shared memory objects
    state = state_server.Read();
    settings = settings_server.Read();

    // Update PID Settings
    position_controller->ProportionalGain(settings.position_Kp);
    position_controller->IntegralGain(settings.position_Ki);
    position_controller->DerivativeGain(settings.position_Kd);
    angle_controller->ProportionalGain(settings.angle_Kp);
    angle_controller->IntegralGain(settings.angle_Ki);
    angle_controller->DerivativeGain(settings.angle_Kd);

    // Update controller errors
    auto position_error = 0 - state.position;

    position_controller->UpdateError(.01, position_error);

    angle_controller->UpdateError(.01, 0.0 - state.angle);

    // Calculate the control command
    command.velocity =
        angle_controller->TotalError() - position_controller->TotalError();

    command_server.Write(command);

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
  printf("\nExiting controller\n");
  return 0;
}