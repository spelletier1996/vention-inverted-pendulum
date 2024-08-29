### Pendulum SIM
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