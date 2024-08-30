#include <chrono>
#include <cmath>
#include <network.hpp>
#include <signal.h>
#include <stdio.h>

#include "hmi.hpp"

bool terminate = false;

void SignalHandler([[maybe_unused]] int sig) { terminate = true; }

int main() {
  signal(SIGINT, SignalHandler);
  // Create local objects to store shared memory
  utils::SimState state;
  utils::ControllerSettings settings;

  // Open the controllers shared memory objects
  utils::Client<utils::SimState> state_server("sim_state");
  utils::Client<utils::ControllerSettings> command_server("sim_command");

  HMI hmi;

  auto window = hmi.GlfwInit();

  // Main loop
  while (!terminate) {
    glfwPollEvents();
    if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0) {
      ImGui_ImplGlfw_Sleep(10);
      continue;
    }

    state = state_server.Read();

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    {

  ImGuiWindowFlags windowFlags = ImGuiWindowFlags_None;

const ImGuiViewport* viewport = ImGui::GetMainViewport();
  ImGui::SetNextWindowPos(viewport->WorkPos);
  ImGui::SetNextWindowSize(viewport->WorkSize);
  ImGui::SetNextWindowViewport(viewport->ID);

  windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
  windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

      ImGui::Begin("simulation view", nullptr, windowFlags);

      ImGui::SetWindowSize(ImVec2(500, 500));

        printf("position = %f\n", state.position);

      hmi.draw_pendulum(state.position * 10, state.angle);

      ImGui::Value("cart position", static_cast<float>(state.position));
      ImGui::Value("pendulum angle", static_cast<float>(state.angle));
      ImGui::Value("cart velocity", static_cast<float>(state.velocity));
      ImGui::Value("pendulum angular velocity", static_cast<float>(state.angular_velocity));

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