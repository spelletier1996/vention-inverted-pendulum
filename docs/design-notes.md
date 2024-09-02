# Design Notes

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

Simple GUI that allows visualization of the state of the turbine and allows for changing gain params of controller

Program structure options:
1. Three seperate processes (emulate seperate computers)
2. Two processes (Seperate HMI and controller + Sim threaded)
3. Single processes multiple threads?

Option 1 in this case seems the most correct, it allows for emulation of a real system where the sim is taking the place of hardware, 
the controller is running on the primary computational unit (MachineMotion in the case of vention), and then the HMI is simply a swappable front-end 
running on a handheld tablet or maybe on the controller itself.

### Medium Level

With a basic structure the medium level considerations need to be thought through

##### Network Structure
We will be communication between three separate processes how will this be structured?
As the HMI and simulation are going to be swappable, it seems correct to have the controller be the "server" and a source of truth for settings
and the current state of the sim

We can have the sim/hardware push data to the controller and the controller then push control data (Vx) to the simulator/hardware

The HMI would then simply read the state and control settings directly from the controller as a source of truth and could modify the controller settings as needed

State and controller settings cant be stored in the HMI itself as it might be disabled when running independently or when being updated

This structure leads to the sim and hmi being dependent on the controller as the common element but the controller being independent of them

##### Simulation 

The sim approach can be done in a few ways
- using a physics library/engine (ex: game engine) to directly model the pendulum 
- modeling the pendulum in matlab and extracting the equations of motion
- deriving the equations of motion directly
- using existing equations

The method chosen was motivated by simplicity, taking an existing set of equations of motion and modifying them with the addition of disturbance D

The design final solution given a real world situation would likely be proper modeling of the system in matlab to allow for advanced controller design. 

The equations of motion can easily be used to model the system by taking their derivatives, and integrating them over a timestep dt. In this case I chose 1000hz to allow for a higher frequency then the controller modeling a real world situation

The equations of motion describe input as a force U but the client wants input Vx and this more closely models an actuator that would not generally be effort controlled

To solve this we can implement a simple velocity control PID loop inside the sim itself emulating the control loop of a smart actuator that itself would take in position, velocity ect...

Finally in terms of testing with a disturbance generally the method in reality would be a physical input as that's the not the case here a simple disturbance input is available for testing

##### Controller 

The controller design can be approached in multiple ways.

Ideally we use the equations of motion and then develop the controller with references to existing designs in well established papers using matlab or another controller design tool.

control methods include P, PI, PID, PID+feedforward, LQR ect... 

In this case for simplicity I implemented a dual multivariable PID controller that combines a controller for angle of the arm and position X of the cart. 

Ideally according to the papers I reviewed linked bellow, a LQR design may be ideal as it is better suited to multi variable input with single control output

### Low Level

##### IPC Selection

There are many IPC approaches that this network can take
- SharedMemory
- TCP
- UDP
- linux pipes, queues, ect...
- hardware emulation (VCAN)

I believe that in a design final product that the proper approach would be using a TCP/IP based connection between the controller and HMI and then a CAN based connection between the controller and Sim. 

This would best emulate the real world hardware. CAN would use a CANOpen protocol and the HMI could potentially use either a simple Push/Pull system or a REST api

I chose to implement a Shared memory approach that attempts to emulate a server client system. 

In terms of single computer communication shared memory is fast, efficient and reliable but would of course not easily allow inter-computer communication without some sort of middle-ware.

# Messy Notes for private use 

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
