#include <chrono>
#include <cmath>
#include <network.hpp>
#include <signal.h>
#include <stdio.h>

#include "hmi.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "tools.hpp"

bool terminate = false;

void SignalHandler([[maybe_unused]] int sig) { terminate = true; }

int main() {
  signal(SIGINT, SignalHandler);
  // Create local objects to store shared memory
  utils::SimState state;
  utils::SimCommand command;
  utils::ControllerSettings settings;

  // Open the controllers shared memory objects
  utils::Client<utils::SimState> state_server("sim_state");
  utils::Client<utils::SimCommand> command_server("sim_command");
  utils::Client<utils::ControllerSettings> settings_server(
      "controller_settings");

  HMI hmi;

  GLFWwindow *window = hmi.GlfwInit();

  if (window == nullptr) {
    printf("Failed to initialize GLFW\n");
    return 0;
  }

  float disturbance = 0.0f;

  // Main loop
  while (!terminate) {
    glfwPollEvents();
    if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0) {
      ImGui_ImplGlfw_Sleep(10);
      continue;
    }

    // update the shared memory objects
    state = state_server.Read();
    command = command_server.Read();
    settings = settings_server.Read();

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Draw the primary HMI window
    {

      ImGuiWindowFlags windowFlags = ImGuiWindowFlags_None;

      const ImGuiViewport *viewport = ImGui::GetMainViewport();
      ImGui::SetNextWindowPos(viewport->WorkPos);
      ImGui::SetNextWindowSize(viewport->WorkSize);
      ImGui::SetNextWindowViewport(viewport->ID);

      windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                     ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
      windowFlags |=
          ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

      ImGui::Begin("simulation view", nullptr, windowFlags);

      auto window_size = ImGui::GetWindowSize();

      auto scaled_position =
          ((state.position + 10) / 20 * window_size.x) - window_size.x / 2;

      hmi.draw_pendulum(scaled_position, state.angle, 50);

      ImGui::Value("cart position", static_cast<float>(state.position));
      ImGui::Value("pendulum angle",
                   static_cast<float>(tools::RadToDeg(state.angle)));
      ImGui::Value("cart velocity", static_cast<float>(state.velocity));
      ImGui::Value("pendulum angular velocity",
                   static_cast<float>(state.angular_velocity));

      if (ImGui::BeginTabBar("Options")) {
        if (ImGui::BeginTabItem("Sim Controls")) {

          ImGui::Button("Send");

          if (ImGui::IsItemActive()) {
            command.disturbance = disturbance;
          } else {
            command.disturbance = 0;
          }

          ImGui::SameLine();
          ImGui::SliderFloat("Disturbance", &disturbance, -100, 100);

          ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Controller Options")) {

          ImGui::Text("Controller options coming soon...");

          ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
      }

      ImGui::End();
    }

    // {

    //   ImGui::Begin("simulation visualization", nullptr,
    //                ImGuiWindowFlags_NoCollapse);

    //   ImGui::SetWindowSize(ImVec2(500, 500));

    //   ImGui::End();
    // }

    // Rendering
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w,
                 clear_color.z * clear_color.w, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);

    command_server.Write(command);
    settings_server.Write(settings);
  }
#ifdef __EMSCRIPTEN__
  EMSCRIPTEN_MAINLOOP_END;
#endif

  // Cleanup
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(window);
  glfwTerminate();

  printf("\nExiting HMI\n");
  return 0;
}