#ifndef INCLUDE_PID_HPP
#define INCLUDE_PID_HPP

namespace controller {

class PID {
 public:
  PID() = default;
  PID(double proportional_gain, double intergral_gain, double derivative_gain)
      : Kp_(proportional_gain), Ki_(intergral_gain), Kd_(derivative_gain){};

  //!
  //!@brief Updates the PID errors based on the control point
  //!@param dt time step (seconds)
  //!@param control_point desired control point (any units)
  //!
  void UpdateError(double time_step, double control_point) {
    double prev_cte = p_error_;

    p_error_ = control_point;
    i_error_ = time_step * i_error_ + control_point;
    d_error_ = (control_point - prev_cte) / time_step;
  }

  //!
  //!@brief Returns the total calculated error based on the last update
  //!@return const double
  //!
  [[nodiscard]] auto TotalError() const -> double {
    double total = Kp_ * p_error_ + Ki_ * i_error_ + Kd_ * d_error_;
    return total;
  }

  //!
  //!@brief Access methods for the PID gains
  //!@note Not thread safe
  //!@return const double& to the gain value
  //!
  [[nodiscard]] auto ProportionalGain() const -> const double& { return Kp_; }
  //!
  //!@brief Set the proportional gain
  //!@note Not thread safe
  //!@param gain The new gain value
  //!
  auto ProportionalGain(double gain) { Kp_ = gain; }
  [[nodiscard]] auto IntegralGain() const -> const double& { return Ki_; }
  auto IntegralGain(double gain) { Ki_ = gain; }
  [[nodiscard]] auto DerivativeGain() const -> const double& { return Kd_; }
  auto DerivativeGain(double gain) { Kd_ = gain; }

  auto Reset() {
    p_error_ = 0.0;
    i_error_ = 0.0;
    d_error_ = 0.0;
  }

  auto Reset(double proportional_gain, double intergral_gain,
             double derivative_gain) {
    Kp_ = proportional_gain;
    Ki_ = intergral_gain;
    Kd_ = derivative_gain;
    Reset();
  }

 private:
  double p_error_{0.0};  // Proportional error
  double i_error_{0.0};  // Integral error
  double d_error_{0.0};  // Derivative error

  double Kp_{0.0};  // Proportional gain
  double Ki_{0.0};  // Integral gain
  double Kd_{0.0};  // Derivative gain
};

}  // namespace controller

#endif  // INCLUDE_PID_HPP
