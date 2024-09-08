#include <cmath>
#include <csignal>
#include <cstdio>
#include <network.hpp>

#include "hmi.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "tools.hpp"

bool terminate = false;

void SignalHandler([[maybe_unused]] int sig) {
  terminate = true;
}

auto main() -> int {
  signal(SIGINT, SignalHandler);
  // Create local objects to store shared memory
  network::SimState state;
  network::SimCommand command;
  network::ControllerSettings settings;

  // Open the controllers shared memory objects
  network::Client<network::SimState> state_server("sim_state");
  network::Client<network::SimCommand> command_server("sim_command");
  network::Client<network::ControllerSettings> settings_server(
      "controller_settings");
  network::Client<bool> reset("reset_signal");

  GLFWwindow* window = hmi::GlfwInit();

  if (window == nullptr) {
    printf("Failed to initialize GLFW\n");
    return 0;
  }

  float disturbance = 0.0F;
  settings = settings_server.Read();

  // Main loop
  while ((glfwWindowShouldClose(window) == 0) && !terminate) {
    // Poll and handle events (inputs, window resize, etc.)
    glfwPollEvents();
    if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0) {
      ImGui_ImplGlfw_Sleep(10);
      continue;
    }

    // update the shared memory objects
    state = state_server.Read();
    command = command_server.Read();

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Draw the primary HMI window
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;

    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);

    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                    ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |=
        ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    ImGui::Begin("simulation view", nullptr, window_flags);

    // Draw the pendulum animation
    ImVec2 window_size = ImGui::GetWindowSize();
    float scaled_position =
        ((static_cast<float>(state.position) + 10) / 20 * window_size.x) -
        window_size.x / 2;
    hmi::DrawPendulum(scaled_position, static_cast<float>(state.angle), 100);

    // Publish the state variables
    ImGui::Value("Position", static_cast<float>(state.position));
    ImGui::Value("Velocity", static_cast<float>(state.velocity));
    ImGui::Value("Angle", static_cast<float>(tools::RadToDeg(state.angle)));
    ImGui::Value("Angular Velocity",
                 static_cast<float>(state.angular_velocity));

    // Draw the options tab bar
    if (ImGui::BeginTabBar("Options")) {
      if (ImGui::BeginTabItem("Controls")) {

        // Publish disturbance while pressed
        ImGui::Button("Send");
        if (ImGui::IsItemActive()) {
          command.disturbance = disturbance;
        } else {
          command.disturbance = 0;
        }
        ImGui::SameLine();
        ImGui::SliderFloat("Disturbance", &disturbance, -100, 100);

        // Restart the sim and controllers
        if (ImGui::Button("Restart Simulator")) {
          reset.Write(true);
        }

        ImGui::EndTabItem();
      }
      if (ImGui::BeginTabItem("PID Settings")) {

        ImGui::Text("Controller options coming soon...");

        ImGui::EndTabItem();
      }
      ImGui::EndTabBar();
    }

    ImGui::End();

    // Render the window drawn above
    ImGui::Render();
    int display_w;
    int display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    ImVec4 clear_color = ImVec4(0.45F, 0.55F, 0.60F, 1.00F);
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w,
                 clear_color.z * clear_color.w, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window);

    // Write updated values to shared memory
    double saved_disturbance = command.disturbance;
    command = command_server.Read();
    command.disturbance = saved_disturbance;
    command_server.Write(command);
  }

  // Cleanup
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(window);
  glfwTerminate();

  printf("\nExiting HMI\n");
  return 0;
}