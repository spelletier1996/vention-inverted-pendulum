#ifndef INCLUDE_HMI_HPP
#define INCLUDE_HMI_HPP

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <cmath>
#include <memory>

#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

class HMI {

public:
  HMI() = default;

  //!
  //!@brief Standard OpenGL setup for ImGui
  //! Taken from example project
  //! I dont like it but for some reason a shared ptr cant be used
  //! @return int imgui setup status
  //!
  auto GlfwInit() -> GLFWwindow * {
    // Create local variables
    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    const char *glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    // GL 3.2 + GLSL 150
    const char *glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);           // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char *glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+
    // only glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // 3.0+ only
#endif

    if (!glfwInit()) {
      return nullptr;
    }

    // Create window with graphics context
    GLFWwindow *window = glfwCreateWindow(
        500, 500, "Dear ImGui GLFW+OpenGL3 example", nullptr, nullptr);
    if (window == nullptr)
      return nullptr;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |=
        ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |=
        ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
#ifdef __EMSCRIPTEN__
    ImGui_ImplGlfw_InstallEmscriptenCallbacks(window, "#canvas");
#endif
    ImGui_ImplOpenGL3_Init(glsl_version);

    return window;
  }

  void draw_pendulum(double cart_position, double pendulum_angle) {
    static ImVec4 colf = ImVec4(1.0f, 1.0f, 0.4f, 1.0f);
    const ImU32 col = ImColor(colf);

    ImDrawList *draw_list = ImGui::GetWindowDrawList();

    auto window_size = ImGui::GetWindowSize();
    auto window_pos = ImGui::GetWindowPos();

    auto x_center = window_pos.x + window_size.x / 2;
    auto y_center = window_pos.y + window_size.y / 2;

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
        ImVec2(x_center + cart_position -
                   std::sin(pendulum_angle) * stick_length,
               y_center - std::cos(pendulum_angle) * stick_length),
        col, 1.0f);

    // pend mass
    draw_list->AddCircle(
        ImVec2(x_center + cart_position -
                   std::sin(pendulum_angle) * stick_length,
               y_center - std::cos(pendulum_angle) * stick_length),
        5.0f, col);
  }
};

#endif // INCLUDE_HMI_HPP
