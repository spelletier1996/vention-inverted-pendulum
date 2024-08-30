# vention-inverted-pendulum
### Introduction
This is my attempt at solving the vention inverted pendulum take home exam

The solution is split into three seperate executables to somewhat emulate a real world scenerio. The simulation is taking the place of real world hardware, the controller is running on a seperate device and the HMI is independent from the controller allowing it to be updated while maintaining stability.

Further requirements and design decisions can be reviewed in ```docs/design```

### Dependencies
The following dependencies are required to run the full sim + controller + HMI

- [Boost](https://github.com/boostorg/boost) specifically Boost::interprocess for IPC communication
- [ImGui](https://github.com/ocornut/imgui) HMI (already in the repo no action required)
- [Eigen3](https://eigen.tuxfamily.org/index.php?title=Main_Page) Vector math library 
- [glfw3](https://www.glfw.org/) 
- [opengl](https://www.opengl.org/) 
- [cmake](https://cmake.org/) 
- [clang](https://clang.llvm.org/) 

### Building
##### Native
Their are two build options either ensure all dependencies are installed and then build using:

```
mkdir build 
cd ./build
cmake ..
make
```
##### Docker
Alternatively the included Dockerfile can be used first build the image from the project root with

```docker build -t pendulum_sim .```

#### Running

TODO: how to actually run needs to be finished

#### Originality
As this is meant as a take home exam here I will list which code I wrote vs borrowed from external sources, excluding dependencies already listed. If not mentioned chances are I wrote them myself.

The imgui setup code (hmi.hpp GlfwInit) was taked from examples.

The inverted_pendulum.cpp was based on an existing project found [here](https://github.com/jasleon/Inverted-Pendulum) with the equations being modified and the overall code being updated to meet more modern C++ standards. 

The controller code was also originally based on the above project but again updated to modern standards.