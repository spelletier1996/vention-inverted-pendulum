#include "inverted_pendulum.hpp"

void model::InvertedPendulum::Update(const double& dt, const double& cart_force,
                                     const double& pendulum_disturbance) {
  // Recover state parameters
  double theta = x_(1);                       // angle of the pendulum
  double velocity = x_(2);                    // velocity of the base
  double omega = x_(3);                       // angular rate of the pendulum
  double disturbance = pendulum_disturbance;  // disturbance
  double force = cart_force;                  // force applied to the base

  // Compute common terms
  double sin_theta = std::sin(theta);
  double cos_theta = std::cos(theta);
  double omega_squared = std::pow(omega, 2);
  double length_squared = std::pow(l_, 2);

  // Calculate derivatives
  x_dot_(0) = velocity;
  x_dot_(1) = omega;
  x_dot_(2) =
      (-m_ * l_ * sin_theta * omega_squared +
       m_ * g_ * (m_ * length_squared / J_t_) * sin_theta * cos_theta -
       c_ * velocity - (gamma_ / J_t_) * m_ * l_ * cos_theta * omega + force +
       disturbance * cos_theta) /
      (M_t_ - m_ * (m_ * length_squared / J_t_) * cos_theta * cos_theta);
  x_dot_(3) = (-m_ * length_squared * sin_theta * cos_theta * omega_squared +
               M_t_ * g_ * l_ * sin_theta - c_ * l_ * cos_theta * velocity -
               gamma_ * (M_t_ / m_) * omega + l_ * cos_theta * force +
               disturbance * sin_theta) /
              (J_t_ * (M_t_ / m_) - m_ * (l_ * cos_theta) * (l_ * cos_theta));

  // Apply Euler method to solve differential equations
  x_ += x_dot_ * dt;
}