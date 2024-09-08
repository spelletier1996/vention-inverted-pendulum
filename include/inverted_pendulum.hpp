#ifndef SRC_INVERTED_PENDULUM_HPP
#define SRC_INVERTED_PENDULUM_HPP

#include <Eigen/Dense>
#include <utility>

namespace model {
class InvertedPendulum {
 public:
  //!
  //!@brief Construct a new Inverted Pendulum object
  //!
  //!@param M base mass (kg)
  //!@param m pendulum mass (kg)
  //!@param J pendulum moment (kg m^2)
  //!@param l pendulum length (m)
  //!@param c base friction coefficient (N s/m)
  //!@param gamma pendulum friction coefficient (N m s/rad)
  //!@param x_0 Initial conditions (m, rad, m/s, rad/s)
  //!
  InvertedPendulum(double cart_mass, double pend_mass, double pend_moment,
                   double pend_length, double base_friction,
                   double pend_friction, Eigen::VectorXd x_0)
      : M_(cart_mass),
        m_(pend_mass),
        J_(pend_moment),
        l_(pend_length),
        c_(base_friction),
        gamma_(pend_friction),
        g_(9.81),
        M_t_(cart_mass + pend_mass),
        J_t_(pend_moment + pend_mass * std::pow(pend_length, 2)),
        x_(std::move(x_0)),
        x_dot_(Eigen::VectorXd(4).setZero()) {}

  //!
  //!@brief Constructs with default values and given initial conditions
  //!@param x_0
  //!
  explicit InvertedPendulum(Eigen::VectorXd x_0)
      : InvertedPendulum(1.0, 1.0, 1.0, 1.0, 1.0, 1.0, std::move(x_0)) {}

  InvertedPendulum() : InvertedPendulum(Eigen::VectorXd(4)) {
    x_ << 0, 0, 0, 0;
  }

  //!
  //!@brief Updates the inverted pendulum sim
  //!@param dt time step (seconds)
  //!@param cart_force cart force (N)
  //!@param pendulum_disturbance pendulum disturbance (N)
  //!
  void Update(const double& time_step, const double& cart_force,
              const double& pendulum_disturbance);

  //!
  //!@brief Returns current state vector
  //!@return Eigen::VectorXd (position m, angle rad, velocity m/s, angular
  //! velocity rad/s)
  //!
  [[nodiscard]] auto State() const -> Eigen::VectorXd { return x_; }

  //!
  //!@brief Simple position setter
  //! Just for simplicity and testing purposes
  //!@param position (m)
  //!@return auto
  //!
  auto Position(double position) { x_(0) = position; }

  //!
  //!@brief Restart the sim setting state to zero
  //!
  void Restart() {
    x_.setZero();
    Restart(x_);
  }

  void Restart(Eigen::VectorXd initial_conditions) {
    x_ = std::move(initial_conditions);
    x_dot_.setZero();
  }

 private:
  [[maybe_unused]] const double M_;  // mass of the base (Kg)
  const double m_;                   // mass of the pendulum (Kg)
  [[maybe_unused]] const double
      J_;               // moment of inertia of the pendulum (Kg m^2)
  const double l_;      // distance from the base to the pendulum (m)
  const double c_;      // coefficient of viscous friction (base)
  const double gamma_;  // coefficient of viscous friction (pendulum)
  const double g_;      // acceleration due to gravity (m/s^2)
  const double M_t_;    // total mass (Kg)
  const double J_t_;    // total inertia (Kg m^2)

  Eigen::VectorXd
      x_;  // state vector (position, angle, velocity, angular velocity)
  Eigen::VectorXd x_dot_;  // state vector derivative
};

}  // namespace model

#endif  // SRC_INVERTED_PENDULUM_HPP