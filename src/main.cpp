#include <SFML/Graphics.hpp>
#include <iostream>

#include "Eigen/Dense"
#include "inverted_pendulum.hpp"
#include "pid.hpp"
#include <chrono>
#include <memory>
#include <thread>

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

  // Design LQR controller
  // LQR optimal;
  // ptr->Linearize();
  // optimal.A_ = ptr->A_;
  // optimal.B_ = ptr->B_;
  // optimal.Q_ = Eigen::MatrixXd::Identity(4, 4);
  // optimal.Q_(0, 0) = 10;
  // optimal.R_ = Eigen::MatrixXd::Identity(1, 1);
  // optimal.Compute();

  bool pid = true;

  sim->Restart(x_0);

  double control_velocity = 1.0;

  std::thread sim_thread([sim, &control_velocity]() mutable {
    auto velocity_controller = std::make_shared<utils::PID>(50, 20, 0);
    while (true) {
      double u = 0;
      double disturbance = 0;
      double present_velocity = sim->State()(2);
      auto control_point = control_velocity - present_velocity;
      velocity_controller->UpdateError(.001, control_point);
      u = velocity_controller->TotalError();

      sim->Update(.001, u, disturbance);
      printf("u = %f\n", u);
      printf("present_velocity = %f\n", present_velocity);
      printf("control_velocity = %f\n", control_velocity);
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
  });

  std::thread control_thread([sim, &control_velocity]() mutable {
    auto angle_controller = std::make_shared<utils::PID>(100, 50, 50);
    auto position_controller = std::make_shared<utils::PID>(20, 5, 1);
    while (true) {

      // should be sent a different way (IPC)
      double position = sim->State()(0);
      double angle = sim->State()(1);

      angle_controller->UpdateError(.01, 0.0 - angle);
      auto angle_error = angle_controller->TotalError();

      auto position_error = 0 - position;
      position_controller->UpdateError(.01, position_error);

      control_velocity =
          angle_controller->TotalError() - position_controller->TotalError();

      // position_controller->UpdateError(.01, position -
      // angle_controller->TotalError());

      // control_velocity =  position_controller->TotalError();

      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  });

  std::thread hmi_thread([sim, theta_0]() {
    sf::RenderWindow window(sf::VideoMode(640, 480), "Inverted Pendulum");
    // Load font
    sf::Font font;
    if (!font.loadFromFile("Roboto-Regular.ttf")) {
      std::cout << "Failed to load font!\n";
    }

    // Create text to display simulation time
    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(24);
    const sf::Color grey = sf::Color(0x7E, 0x7E, 0x7E);
    text.setFillColor(grey);
    text.setPosition(480.0F, 360.0F);

    // Create text to display controller type
    sf::Text type;
    type.setFont(font);
    type.setCharacterSize(24);
    const sf::Color turquoise = sf::Color(0x06, 0xC2, 0xAC);
    type.setFillColor(turquoise);
    type.setPosition(480.0F, 384.0F);

    // Create a track for the cart
    sf::RectangleShape track(sf::Vector2f(640.0F, 2.0F));
    track.setOrigin(320.0F, 1.0F);
    track.setPosition(320.0F, 240.0F);
    const sf::Color light_grey = sf::Color(0xAA, 0xAA, 0xAA);
    track.setFillColor(light_grey);

    // Create the cart of the inverted pendulum
    sf::RectangleShape cart(sf::Vector2f(100.0F, 100.0F));
    cart.setOrigin(50.0F, 50.0F);
    cart.setPosition(320.0F, 240.0F);
    cart.setFillColor(sf::Color::Black);

    // Create the pole of the inverted pendulum
    sf::RectangleShape pole(sf::Vector2f(20.0F, 200.0F));
    pole.setOrigin(10.0F, 200.0F);
    pole.setPosition(320.0F, 240.0F);
    pole.setRotation(-theta_0);
    const sf::Color brown = sf::Color(0xCC, 0x99, 0x66);
    pole.setFillColor(brown);

    // Create a clock to run the simulation
    sf::Clock clock;

    while (window.isOpen()) {
      sf::Event event;
      while (window.pollEvent(event)) {
        switch (event.type) {
        case sf::Event::Closed:
          window.close();
          break;
        }
      }
      Eigen::VectorXd x = sim->State();

      // Update SFML drawings
      cart.setPosition(320.0F + 100 * x(0), 240.0F);
      pole.setPosition(320.0F + 100 * x(0), 240.0F);
      pole.setRotation(to_degrees(-x(1)));

      window.clear(sf::Color::White);
      window.draw(track);
      window.draw(cart);
      window.draw(pole);
      window.draw(text);
      window.draw(type);
      window.display();
    }
  });

  hmi_thread.join();

  // while (window.isOpen()) {
  //   sf::Event event;
  //   while (window.pollEvent(event)) {
  //     switch (event.type) {
  //     case sf::Event::Closed:
  //       window.close();
  //       break;
  //     }
  //   }

    // Update the simulation
    // sf::Time elapsed = clock.getElapsedTime();
    // const float time = elapsed.asSeconds();
    // const std::string msg = std::to_string(time);
    // text.setString("Time   " + msg.substr(0, msg.find('.') + 2));
    // const std::string action = pid ? "Action PID" : "Action LQR";
    // type.setString(action);
    // if (time < 20) {
    //   double u = 0;

    //   double position = ptr->State()(0);
    //   double angle = ptr->State()(1);
    //   double velocity = ptr->State()(2);
    //   double position_error = 0.0 - angle;
    //   x_pid->UpdateError(.001, position_error);
    //   double error = x_pid->TotalError() - velocity;
    //   c_ptr->UpdateError(.001, error);
    //   // u = c_ptr->TotalError() - x_pid->TotalError();
    //   u = c_ptr->TotalError();

    //   ptr->Update(.001, u, 0);
    //   printf("u = %f\n", u);
    //   printf("velocity = %f\n", velocity);
    // } else {
    //   // delete ptr;
    //   // ptr = new InvertedPendulum(x_0);
    //   ptr->Restart();
    //   clock.restart();
    //   pid = (pid) ? false : true;
    // }

    // Eigen::VectorXd x = sim->State();

    // // Update SFML drawings
    // cart.setPosition(320.0F + 100 * x(0), 240.0F);
    // pole.setPosition(320.0F + 100 * x(0), 240.0F);
    // pole.setRotation(to_degrees(-x(1)));

    // window.clear(sf::Color::White);
    // window.draw(track);
    // window.draw(cart);
    // window.draw(pole);
    // window.draw(text);
    // window.draw(type);
    // window.display();

    // auto currentTime = std::chrono::high_resolution_clock::now();
    // std::chrono::duration<double> elapsedTime = currentTime - prevTime;
    // double frequency = 1.0 / elapsedTime.count();
    // prevTime = currentTime;

    // std::cout << ": Frequency = " << frequency << " Hz" << std::endl;

    // std::this_thread::sleep_for(std::chrono::milliseconds(1));
    return 0;
  }
