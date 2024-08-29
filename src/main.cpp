#include <SFML/Graphics.hpp>
#include <iostream>

#include "Eigen/Dense"
#include "imgui.h"
#include "inverted_pendulum.hpp"
#include "pid.hpp"
#include <chrono>
#include <memory>
#include <network.hpp>
#include <stdio.h>
#include <thread>

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

double to_radians(double degrees) { return (degrees * (M_PI / 180.0F)); }

double to_degrees(double radians) { return (radians * (180.0F / M_PI)); }

int main() {

  // Set initial conditions
  const double p_0 = 0;
  const double theta_0 = 5;
  Eigen::VectorXd x_0(4);
  x_0 << p_0, to_radians(theta_0), 0, 0;

  // Set PID constants
  const double kp = 50;
  const double ki = 0;
  const double kd = 0;

  // Create a model with default parameters
  auto sim = std::make_shared<InvertedPendulum>(x_0);

  auto c_ptr = std::make_shared<utils::PID>(kp, ki, kd);

  auto x_pid = std::make_shared<utils::PID>(20, 0, 0);

  sim->Restart(x_0);

  double control_velocity = 1.0;

  std::thread sim_thread([sim]() mutable {
    auto velocity_controller = std::make_shared<utils::PID>(10, 1, 0);
    double time = 0.0;
    double disturbance = 0;
    utils::Client<double> control_velocity("control_velocity");
    while (true) {
      double u = 0;
      double present_velocity = sim->State()(2);
      auto control_point = control_velocity.Read() - present_velocity;
      velocity_controller->UpdateError(.001, control_point);
      u = velocity_controller->TotalError();

      // printf("test_variable = %f\n", test_client.Read());

      time = time + .001;
      if (time > 5 && time < 5.5) {
        disturbance = 10;
      } else
        disturbance = 0;

      sim->Update(.001, u, disturbance);
      // printf("u = %f\n", u);
      // printf("present_velocity = %f\n", present_velocity);
      // printf("control_velocity = %f\n", control_velocity);
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
  });

  std::thread control_thread([sim, &control_velocity]() mutable {
    auto angle_controller = std::make_shared<utils::PID>(50, 0, 10);
    // auto position_controller = std::make_shared<utils::PID>(10, 5, 1);
    auto position_controller = std::make_shared<utils::PID>(5, 0, 5);

    utils::Server<double> test("control_velocity");

    while (true) {

      // should be sent a different way (IPC)
      double position = sim->State()(0);
      double angle = sim->State()(1);

      auto position_error = 0 - position;

      position_controller->UpdateError(.01, position_error);

      angle_controller->UpdateError(.01, 0 - angle);

      // control_velocity = position_controller->TotalError();
      control_velocity =
          angle_controller->TotalError() - position_controller->TotalError();

      test.Write(control_velocity);

      // printf("angle_error = %f\n", angle_controller->TotalError());
      printf("position = %f\n", position);
      printf("position_error = %f\n", position_controller->TotalError());
      // printf("control_velocity = %f\n", control_velocity);

      // position_controller->UpdateError(.01, position -
      // angle_controller->TotalError());

      // control_velocity =  position_controller->TotalError();

      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
  });

  std::thread hmi_thread([sim, theta_0]() {
  // sf::RenderWindow window(sf::VideoMode(640, 480), "Inverted Pendulum");
  // // Load font
  // sf::Font font;
  // if (!font.loadFromFile("Roboto-Regular.ttf")) {
  //   std::cout << "Failed to load font!\n";
  // }

  // // Create text to display simulation time
  // sf::Text text;
  // text.setFont(font);
  // text.setCharacterSize(24);
  // const sf::Color grey = sf::Color(0x7E, 0x7E, 0x7E);
  // text.setFillColor(grey);
  // text.setPosition(480.0F, 360.0F);

  // // Create text to display controller type
  // sf::Text type;
  // type.setFont(font);
  // type.setCharacterSize(24);
  // const sf::Color turquoise = sf::Color(0x06, 0xC2, 0xAC);
  // type.setFillColor(turquoise);
  // type.setPosition(480.0F, 384.0F);

  // // Create a track for the cart
  // sf::RectangleShape track(sf::Vector2f(640.0F, 2.0F));
  // track.setOrigin(320.0F, 1.0F);
  // track.setPosition(320.0F, 240.0F);
  // const sf::Color light_grey = sf::Color(0xAA, 0xAA, 0xAA);
  // track.setFillColor(light_grey);

  // // Create the cart of the inverted pendulum
  // sf::RectangleShape cart(sf::Vector2f(100.0F, 100.0F));
  // cart.setOrigin(50.0F, 50.0F);
  // cart.setPosition(320.0F, 240.0F);
  // cart.setFillColor(sf::Color::Black);

  // // Create the pole of the inverted pendulum
  // sf::RectangleShape pole(sf::Vector2f(20.0F, 200.0F));
  // pole.setOrigin(10.0F, 200.0F);
  // pole.setPosition(320.0F, 240.0F);
  // pole.setRotation(-theta_0);
  // const sf::Color brown = sf::Color(0xCC, 0x99, 0x66);
  // pole.setFillColor(brown);

  // // Create a clock to run the simulation
  // sf::Clock clock;

  // while (window.isOpen()) {
  //   sf::Event event;
  //   while (window.pollEvent(event)) {
  //     switch (event.type) {
  //     case sf::Event::Closed:
  //       window.close();
  //       break;
  //     }
  //   }

  //   ImGui::ShowDemoWindow(&show_demo_window);

  //   Eigen::VectorXd x = sim->State();

  //   // Update SFML drawings
  //   cart.setPosition(320.0F + 100 * x(0), 240.0F);
  //   pole.setPosition(320.0F + 100 * x(0), 240.0F);
  //   pole.setRotation(to_degrees(-x(1)));

  //   window.clear(sf::Color::White);
  //   window.draw(track);
  //   window.draw(cart);
  //   window.draw(pole);
  //   window.draw(text);
  //   window.draw(type);
  //   window.display();
  // }

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
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char *glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+
    // only glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // 3.0+ only
#endif

  if(!glfwInit()){
    return -1;
}

    // Create window with graphics context
    GLFWwindow *window = glfwCreateWindow(
        1280, 720, "Dear ImGui GLFW+OpenGL3 example", nullptr, nullptr);
    if (window == nullptr)
      return 1;
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

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can
    // also load multiple fonts and use ImGui::PushFont()/PopFont() to select
    // them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you
    // need to select the font among multiple.
    // - If the file cannot be loaded, the function will return a nullptr.
    // Please handle those errors in your application (e.g. use an assertion, or
    // display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and
    // stored into a texture when calling
    // ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame
    // below will call.
    // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use
    // Freetype for higher quality font rendering.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string
    // literal you need to write a double backslash \\ !
    // - Our Emscripten build process allows embedding fonts to be accessible at
    // runtime from the "fonts/" folder. See Makefile.emscripten for details.
    // io.Fonts->AddFontDefault();
    // io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
    // io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    // io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    // io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    // ImFont* font =
    // io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f,
    // nullptr, io.Fonts->GetGlyphRangesJapanese()); IM_ASSERT(font != nullptr);

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Main loop
#ifdef __EMSCRIPTEN__
    // For an Emscripten build we are disabling file-system access, so let's not
    // attempt to do a fopen() of the imgui.ini file. You may manually call
    // LoadIniSettingsFromMemory() to load settings from your own storage.
    io.IniFilename = nullptr;
    EMSCRIPTEN_MAINLOOP_BEGIN
#else
    while (true)
#endif
    {
      // Poll and handle events (inputs, window resize, etc.)
      // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to
      // tell if dear imgui wants to use your inputs.
      // - When io.WantCaptureMouse is true, do not dispatch mouse input data to
      // your main application, or clear/overwrite your copy of the mouse data.
      // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input
      // data to your main application, or clear/overwrite your copy of the
      // keyboard data. Generally you may always pass all inputs to dear imgui,
      // and hide them from your application based on those two flags.
      glfwPollEvents();
      if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0) {
        ImGui_ImplGlfw_Sleep(10);
        continue;
      }

      // Start the Dear ImGui frame
      ImGui_ImplOpenGL3_NewFrame();
      ImGui_ImplGlfw_NewFrame();
      ImGui::NewFrame();

      // 1. Show the big demo window (Most of the sample code is in
      // ImGui::ShowDemoWindow()! You can browse its code to learn more about
      // Dear ImGui!).
      if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);

      // 2. Show a simple window that we create ourselves. We use a Begin/End
      // pair to create a named window.
      {
        static float f = 0.0f;
        static int counter = 0;

        ImGui::Begin("Hello, world!"); // Create a window called "Hello, world!"
                                       // and append into it.

        ImGui::Text("This is some useful text."); // Display some text (you can
                                                  // use a format strings too)
        ImGui::Checkbox("Demo Window",
                        &show_demo_window); // Edit bools storing our window
                                            // open/close state
        ImGui::Checkbox("Another Window", &show_another_window);

        ImGui::SliderFloat(
            "float", &f, 0.0f,
            1.0f); // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::ColorEdit3(
            "clear color",
            (float *)&clear_color); // Edit 3 floats representing a color

        if (ImGui::Button(
                "Button")) // Buttons return true when clicked (most widgets
                           // return true when edited/activated)
          counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                    1000.0f / io.Framerate, io.Framerate);
        ImGui::End();
      }

      // 3. Show another simple window.
      if (show_another_window) {
        ImGui::Begin(
            "Another Window",
            &show_another_window); // Pass a pointer to our bool variable (the
                                   // window will have a closing button that
                                   // will clear the bool when clicked)
        ImGui::Text("Hello from another window!");
        if (ImGui::Button("Close Me"))
          show_another_window = false;
        ImGui::End();
      }

      // Rendering
      ImGui::Render();
      int display_w, display_h;
      glfwGetFramebufferSize(window, &display_w, &display_h);
      glViewport(0, 0, display_w, display_h);
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
    
  });

  hmi_thread.join();
  sim_thread.join();
  return 0;
}
