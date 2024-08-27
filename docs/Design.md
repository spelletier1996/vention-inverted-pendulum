### Assumptions 
- Status of the turbine is equated to status of the pendulum

### Requirements

- HMI (GUI) that reports status of pendulum 
- Inverted Pendulum simulation defined entirely by mass and height above rail (M (Kg) and L(m))
- Position of the pendulum (Theta) and position of the rail (x) to be measured at 100Hz
- Rail to be controlled by input velocity Vx
- Feedback controller to keep the pendulum vertical and settle to 0mm after being subject to a disturbance Force (F (units? N/m?)) at the mass (M)
- Headless simulation to show its behavior as a response to disturbance F and input Vx
- Feedback controller that will keep the turbine upright
- Simple visualization tool to show state of the turbine

### High level

Simulation of an inverted pendulum with input Vx and Disturbance F

Feedback controller to keep upright (and bring position to 0)

Simple GUI that allows visuilization of the state of the turbine and allows for changing gain params of controller

#####
#####
#####