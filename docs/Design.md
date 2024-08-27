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

Simulation of an inverted pendulum with with IPC @100hz interface for Vx, Angle, Position and Disturbance

Feedback controller to keep upright (and bring position to 0)

Simple GUI that allows visuilization of the state of the turbine and allows for changing gain params of controller

##### Sim
What will be our model
What IPC will we use to comm with controller
How will we model the disturbance

Modeling approach: 
Will use a small angle approaximation of the system without solving the swing up problem for simplicity
Will create a basic SIM in c++ 
 - Ideally we would create a more indepth model using matlab or potentially a physics library in C++
 - This simple implementation is mostly a proof of concept

 Equations of motion will be based on: https://papers.ssrn.com/sol3/papers.cfm?abstract_id=4444887
 They use the FBD approach which I am more familiar with then the energy approach shown here https://underactuated.csail.mit.edu/acrobot.html
 Another example with force of wind accounted for https://www.digitalxplore.org/up_proc/pdf/19-138138560665-69.pdf

 






##### Controller
Controller method? PI, PID, Something else?
IPC for providing an API to the GUI
##### Gui
Select a simple GUI front end (imgui??)
Decide on display method graph vs animation?
