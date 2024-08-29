#ifndef INCLUDE_PID_HPP
#define INCLUDE_PID_HPP

namespace utils {

class PID {
public:
  PID() = default;
  PID(double Kp, double Ki, double Kd) : Kp_(Kp), Ki_(Ki), Kd_(Kd){};

  //!
  //!@brief Updates the PID errors based on the control point
  //!@param dt time step (seconds)
  //!@param control_point desired control point (any units)
  //!
  void UpdateError(double dt, double control_point) {
    double prev_cte = p_error_;

    p_error_ = control_point;
    i_error_ = dt * i_error_ + control_point;
    d_error_ = (control_point - prev_cte) / dt;
  }

  //!
  //!@brief Returns the total calculated error based on the last update
  //!@return const double
  //!
  [[nodiscard]] auto TotalError() -> const double {
    double total = Kp_ * p_error_ + Ki_ * i_error_ + Kd_ * d_error_;
    return total; // Add your total error calc here!
  }

  //!
  //!@brief Access methods for the PID gains
  //!@note Not thread safe
  //!@return const double& to the gain value
  //!
  [[nodiscard]] auto ProportionalGain() const -> const double & { return Kp_; }
  //!
  //!@brief Set the proportional gain
  //!@note Not thread safe
  //!@param gain The new gain value
  //!
  auto ProportionalGain(double gain) { Kp_ = gain; }
  [[nodiscard]] auto IntegralGain() const -> const double & { return Ki_; }
  auto IntegralGain(double gain) { Ki_ = gain; }
  [[nodiscard]] auto DerivativeGain() const -> const double & { return Kd_; }
  auto DerivativeGain(double gain) { Kd_ = gain; }

private:
  double p_error_{0.0}; // Proportional error
  double i_error_{0.0}; // Integral error
  double d_error_{0.0}; // Derivative error

  double Kp_{0.0}; // Proportional gain
  double Ki_{0.0}; // Integral gain
  double Kd_{0.0}; // Derivative gain
};

} // namespace utils

#endif // INCLUDE_PID_HPP
