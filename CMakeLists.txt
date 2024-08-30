cmake_minimum_required(VERSION 3.2)

project(vention-inverted-pendulum
        VERSION 0.1.0
        LANGUAGES CXX)

# Set the C++ standard to C++23
set(CMAKE_CXX_STANDARD 23)
# Set compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -Wall -Wextra -Wpedantic -g")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR})

# TODO: Set the build type

# Find dependencies
find_package(Boost REQUIRED)
find_package(Eigen3 REQUIRED)

add_subdirectory(imgui)

# Set include directory
set(${PROJECT}_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
# Set source directory
set(${PROJECT}_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
# Set include directories
include_directories(${PROJECT}_INCLUDE_DIR)

# add the executable and link libraries
add_executable(controller ${${PROJECT}_SOURCE_DIR}/controller.cpp ${${PROJECT}_SOURCE_DIR}/inverted_pendulum.cpp)
target_link_libraries(controller Boost::boost sfml-graphics sfml-window sfml-system imgui_glfw Eigen3::Eigen)
target_include_directories(controller PUBLIC ${${PROJECT}_INCLUDE_DIR})
install(TARGETS controller DESTINATION bin)

add_executable(simulator ${${PROJECT}_SOURCE_DIR}/sim.cpp ${${PROJECT}_SOURCE_DIR}/inverted_pendulum.cpp)
target_link_libraries(simulator Boost::boost sfml-graphics sfml-window sfml-system imgui_glfw Eigen3::Eigen)
target_include_directories(simulator PUBLIC ${${PROJECT}_INCLUDE_DIR})
install(TARGETS simulator DESTINATION bin)

add_executable(hmi_v1 ${${PROJECT}_SOURCE_DIR}/hmi_v1.cpp ${${PROJECT}_SOURCE_DIR}/inverted_pendulum.cpp)
target_link_libraries(hmi_v1 Boost::boost sfml-graphics sfml-window sfml-system imgui_glfw Eigen3::Eigen)
target_include_directories(hmi_v1 PUBLIC ${${PROJECT}_INCLUDE_DIR})
install(TARGETS hmi_v1 DESTINATION bin)




