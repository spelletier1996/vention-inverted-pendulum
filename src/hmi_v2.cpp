#include <cmath>
#include <network.hpp>
#include <signal.h>
#include <stdio.h>
#include <zmq.hpp>
#include "hmi.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "tools.hpp"

bool terminate = false;

void SignalHandler([[maybe_unused]] int sig) { terminate = true; }

int main() {
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

  GLFWwindow *window = hmi::GlfwInit();

  if (window == nullptr) {
    printf("Failed to initialize GLFW\n");
    return 0;
  }

  float disturbance = 0.0f;
  settings = settings_server.Read();
  float track_width = 5.0f;

  // Create a ZMQ context and socket to communicate with the hmi
  zmq::context_t ctx;
  zmq::socket_t sock(ctx, zmq::socket_type::sub);
  sock.connect("tcp://127.0.0.1:5000");

  // Main loop
  while (!glfwWindowShouldClose(window) && !terminate) {
    zmq::message_t message(12);
    auto res = sock.recv(message, zmq::recv_flags::dontwait);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    if (res.has_value()) {
      printf("Received message: %s\n", message.to_string().c_str());
    }
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

    // Draw the pendulum animation
    ImVec2 window_size = ImGui::GetWindowSize();
    double scaled_position =
        ((state.position + 10) / 20 * window_size.x) - window_size.x / 2;
    hmi::draw_pendulum(scaled_position, state.angle, 100);

    // Publish the state variables
    ImGui::Value("Position", static_cast<float>(state.position));
    ImGui::Value("Velocity", static_cast<float>(state.velocity));
    ImGui::Value("Angle", static_cast<float>(tools::RadToDeg(state.angle)));
    ImGui::Value("Angular Velocity",
                 static_cast<float>(state.angular_velocity));

    /// V2
    ImGui::PushItemWidth(80.0f);
    ImGui::InputFloat("Track Width", &track_width);
    ImGui::PopItemWidth();
    ImGui::SameLine();

    ImGui::PushItemWidth(80.0f);
    if (std::abs(state.position) > ((track_width / 2) - 0.1)) {
      ImU32 colour = ImColor(255, 50, 50, 255);
      ImGui::PushStyleColor(ImGuiCol_Button, colour);
      ImGui::Button("Position Warning");

    } else {
      ImU32 colour = ImColor(140, 200, 80, 200);
      ImGui::PushStyleColor(ImGuiCol_Button, colour);
      ImGui::Button("Position Good");
    }
    ImGui::PopStyleColor(1);
    ImGui::PopItemWidth();
    ///

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

        /// V2
        ImGui::PushItemWidth(80.0f);
        ImGui::Text("Position Controller");
        ImGui::InputDouble("P:Kp", &settings.position_Kp);
        ImGui::SameLine();
        ImGui::InputDouble("P:Ki", &settings.position_Ki);
        ImGui::SameLine();
        ImGui::InputDouble("P:Kd", &settings.position_Kd);
        ImGui::Text("Angle Controller");
        ImGui::InputDouble("A:Kp", &settings.angle_Kp);
        ImGui::SameLine();
        ImGui::InputDouble("A:Ki", &settings.angle_Ki);
        ImGui::SameLine();
        ImGui::InputDouble("A:Kd", &settings.angle_Kd);
        ImGui::PopItemWidth();

        if (ImGui::Button("Set Gains")) {
          settings_server.Write(settings);
        }
        ///

        ImGui::EndTabItem();
      }
      ImGui::EndTabBar();
    }

    ImGui::End();

    // Render the window drawn above
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

    // Write updated values to shared memory
    double saved_disturbance = command.disturbance;
    command = command_server.Read();
    command.disturbance = saved_disturbance;
    command_server.Write(command);
  }

  sock.disconnect("tcp://127.0.0.1:5000");
  zmq_ctx_destroy(&ctx);

  // Cleanup
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(window);
  glfwTerminate();

  printf("\nExiting HMI\n");
  return 0;
}