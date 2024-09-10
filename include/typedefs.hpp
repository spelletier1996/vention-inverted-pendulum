#ifndef TYPEDEFS_HPP
#define TYPEDEFS_HPP

namespace network {

struct SimState {
  double position;
  double angle;
  double velocity;
  double angular_velocity;
};

struct SimCommand {
  double velocity;
  double disturbance;
  bool reset;
};

struct ControllerSettings {
  double position_Kp;
  double position_Ki;
  double position_Kd;
  double angle_Kp;
  double angle_Ki;
  double angle_Kd;
};

}  // namespace network

#endif  //TYPEDEFS_HPP