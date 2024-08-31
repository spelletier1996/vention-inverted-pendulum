#ifndef INCLUDE_HMI_HPP
#define INCLUDE_HMI_HPP

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <cmath>
#include <cstdio>

#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h>

namespace hmi {

//!
//!@brief Standard OpenGL setup for ImGui
//! Taken from example project
//! Id rather use a smart pointer but the ImGui functions require raw pointers
//! @return int imgui setup status
//!
inline auto GlfwInit() -> GLFWwindow * {
  // GL 3.0 + GLSL 130
  const char *glsl_version = "#version 130";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

  // Check if glfw was initialized
  if (!glfwInit()) {
    return nullptr;
  }

  // Create window with graphics context
  GLFWwindow *window =
      glfwCreateWindow(400, 400, "Vention Inverted Pendulum", nullptr, nullptr);
  if (window == nullptr)
    return nullptr;
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1); // Enable vsync

  // Set callback for the x button to close window
  glfwSetWindowCloseCallback(window, [](GLFWwindow *window) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  });

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();

  // Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);

  return window;
}

//!
//!@brief Draws a pendulum to the center of the current window
//!@param cart_position
//!@param pendulum_angle current pendulum angle in radians
//!@param y_offset offset from the y center in pixels
//!
inline void draw_pendulum(double cart_position, double pendulum_angle,
                          float y_offset = 0) {
  const ImU32 col = ImColor(255, 50, 0, 255);

  ImDrawList *draw_list = ImGui::GetWindowDrawList();

  auto window_size = ImGui::GetWindowSize();
  auto window_pos = ImGui::GetWindowPos();

  auto x_center = window_pos.x + window_size.x / 2;
  auto y_center = window_pos.y + window_size.y / 2 + y_offset;

  double line_thinkness = 2.0;
  double rect_size = 15.0;
  // cart
  draw_list->AddRect(
      ImVec2(x_center + rect_size + cart_position, y_center + rect_size),
      ImVec2(x_center - rect_size + cart_position, y_center - rect_size), col,
      0.0f, ImDrawFlags_None, line_thinkness);

  // track
  draw_list->AddLine({0, y_center}, {2 * x_center, y_center}, col,
                     line_thinkness);

  double stick_length = 50.0;
  // stick
  draw_list->AddLine(
      ImVec2(x_center + cart_position, y_center),
      ImVec2(x_center + cart_position - std::sin(pendulum_angle) * stick_length,
             y_center - std::cos(pendulum_angle) * stick_length),
      col, 1.0f);

  // pend mass
  draw_list->AddCircle(
      ImVec2(x_center + cart_position - std::sin(pendulum_angle) * stick_length,
             y_center - std::cos(pendulum_angle) * stick_length),
      5.0f, col);
}

} // namespace hmi

#endif // INCLUDE_HMI_HPP
