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

  // Open the controllers shared memory objects
  utils::Client<utils::SimState> state_server("sim_state");
  utils::Client<utils::ControllerSettings> command_server("sim_command");

  // Create local variables

  while (!terminate) {

  }

  printf("\nExiting HMI\n");
  return 0;
}