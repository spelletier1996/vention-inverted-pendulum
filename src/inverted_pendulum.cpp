#include "inverted_pendulum.hpp"

void model::InvertedPendulum::Update(const double &dt, const double &cart_force,
                                     const double &pendulum_disturbance) {
  // Recover state parameters
  double theta = x_(1);            // angle of the pendulum
  double vx = x_(2);               // velocity of the base
  double omega = x_(3);            // angular rate of the pendulum
  double d = pendulum_disturbance; // disturbance
  double u = cart_force;           // force applied to the base

  // Compute common terms
  double s_t = std::sin(theta);
  double c_t = std::cos(theta);
  double o_2 = std::pow(omega, 2);
  double l_2 = std::pow(l_, 2);

  // Calculate derivatives
  x_dot_(0) = vx;
  x_dot_(1) = omega;
  x_dot_(2) =
      (-m_ * l_ * s_t * o_2 + m_ * g_ * (m_ * l_2 / J_t_) * s_t * c_t -
       c_ * vx - (gamma_ / J_t_) * m_ * l_ * c_t * omega + u + d * c_t) /
      (M_t_ - m_ * (m_ * l_2 / J_t_) * c_t * c_t);
  x_dot_(3) =
      (-m_ * l_2 * s_t * c_t * o_2 + M_t_ * g_ * l_ * s_t - c_ * l_ * c_t * vx -
       gamma_ * (M_t_ / m_) * omega + l_ * c_t * u + d * s_t) /
      (J_t_ * (M_t_ / m_) - m_ * (l_ * c_t) * (l_ * c_t));

  // Apply Euler method to solve differential equations
  x_ += x_dot_ * dt;
}