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

### Questions

- What IPC will we use
- What GUI library
- ...

##### Notes

The simulation can be approached in a few different ways from directly diriving the equations of motion ourselves to using an existing simulator online
I feel that the ideal solution here would be to model the system in matlab and then extract the plant from their but I dont currenly have access nor the time to do so. The alternative is to find an existing model that we can borrow

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
 Example of a simulation to borrow from https://gitlab.com/carlossvg/inverted_pendulum_sim

 In terms of custom derivation of equations and going through the math the time and chance of error are very high, using an existing simulation seems like the safer approach as long as we are transparent about that fact

 IPC methods to consider
- Sockets
- shared memory (boost interprocess)
- DDS
- Message Queues

Leaning towards Sockets as that would simulate the real world connection between the physical machine and our controller
would also be more similar to a can socket connection
Shared memory and msg queues are faster and better suited to large data but that doesnt really apply to our situation

The simulator will be broken into three components the simulation loop running at X frequency
A listener to a TCP port for updating the input variables F and D
A thread that is sending the state information at 100hz to the controller

Alternative approach would be to use boost interprocess and message queues or shared memory, would make the code and approach a lot more managable
The approach would be very similar to the above

The correct approach in this case is a communication method such as sockets that allow for not just inter process but inter system communication
I took a shared_memory approach as its something I am more familiar with but in a real world situation i would use a network implementation

Need to research a simple pendulum model to implement 
Seems to be a decent paper on the subject https://papers.ssrn.com/sol3/papers.cfm?abstract_id=4444887
Seems that simply balancing the the pol is much simpler than the full swing up problem maybe thats the way to go https://underactuated.csail.mit.edu/acrobot.html
Small angle approximation
https://github.com/jasleon/Inverted-Pendulum/blob/main/src/inverted_pendulum.cpp - good reference for checking approach

need to look into IPC methods
sockets using boostasio simple example of client server 
https://www.codingwiththomas.com/blog/boost-asio-server-client-example

Server client example: https://www.codingwiththomas.com/blog/boost-asio-server-client-example


dx/dt = x_dot
dθ/dt = θ_dot
dx_dot/dt = (u + m * l * θ_ddot * cos(θ) - m * l * θ_dot^2 * sin(θ)) / (M + m)
dθ_dot/dt = (-m * g * sin(θ) - m * x_ddot * cos(θ) + D) / (m * l)


  x_dot_(0) = vx;
  x_dot_(1) = omega;
  x_dot_(2) = (-m_ * l_ * s_t * o_2 + m_ * g_ * (m_ * l_2 / J_t_) * s_t * c_t -
               c_ * vx - (gamma_ / J_t_) * m_ * l_ * c_t * omega + u) /
              (M_t_ - m_ * (m_ * l_2 / J_t_) * c_t * c_t);
  x_dot_(3) =
      (-m_ * l_2 * s_t * c_t * o_2 + M_t_ * g_ * l_ * s_t - c_ * l_ * c_t * vx -
       gamma_ * (M_t_ / m_) * omega + l_ * c_t * u) /
      (J_t_ * (M_t_ / m_) - m_ * (l_ * c_t) * (l_ * c_t));


//With disturbance D

  x_dot_(0) = vx;
  x_dot_(1) = omega;
  x_dot_(2) = (-m_ * l_ * s_t * o_2 + m_ * g_ * (m_ * l_2 / J_t_) * s_t * c_t -
               c_ * vx - (gamma_ / J_t_) * m_ * l_ * c_t * omega + u + D * c_t) 
               /
              (M_t_ - m_ * (m_ * l_2 / J_t_) * c_t * c_t);
  x_dot_(3) = (-m_ * l_2 * s_t * c_t * o_2 + M_t_ * g_ * l_ * s_t - c_ * l_ * c_t * vx -
               gamma_ * (M_t_ / m_) * omega + l_ * c_t * u + D * s_t) 
               /
              (J_t_ * (M_t_ / m_) - m_ * (l_ * c_t) * (l_ * c_t));

//Without 
  x_dot_(0) = vx;
  x_dot_(1) = omega;
  x_dot_(2) = (-m_ * l_ * s_t * o_2 + m_ * g_ * (m_ * l_2 / J_t_) * s_t * c_t -
               c_ * vx - (gamma_ / J_t_) * m_ * l_ * c_t * omega + u) 
               /
              (M_t_ - m_ * (m_ * l_2 / J_t_) * c_t * c_t);
  x_dot_(3) =
      (-m_ * l_2 * s_t * c_t * o_2 + M_t_ * g_ * l_ * s_t - c_ * l_ * c_t * vx -
       gamma_ * (M_t_ / m_) * omega + l_ * c_t * u) /
      (J_t_ * (M_t_ / m_) - m_ * (l_ * c_t) * (l_ * c_t));

todo:
fix backend script