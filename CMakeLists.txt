cmake_minimum_required(VERSION 3.2)

project(vention-inverted-pendulum
        VERSION 0.1.0
        LANGUAGES CXX)

# Set compiler settings
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -Wall -Wextra -Wpedantic -g")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR})

# Find dependencies
find_package(Boost REQUIRED)
find_package(Eigen3 REQUIRED)

# Add imgui as a subdirectory
add_subdirectory(imgui)

# Set include directory
set(${PROJECT}_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
# Set source directory
set(${PROJECT}_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
# Set include directories
include_directories(${PROJECT}_INCLUDE_DIR)

# add the executable and link libraries
add_executable(controller ${${PROJECT}_SOURCE_DIR}/controller.cpp)
target_link_libraries(controller Boost::boost Eigen3::Eigen)
target_include_directories(controller PUBLIC ${${PROJECT}_INCLUDE_DIR})
install(TARGETS controller DESTINATION bin)

add_executable(simulator ${${PROJECT}_SOURCE_DIR}/sim.cpp ${${PROJECT}_SOURCE_DIR}/inverted_pendulum.cpp)
target_link_libraries(simulator Boost::boost Eigen3::Eigen)
target_include_directories(simulator PUBLIC ${${PROJECT}_INCLUDE_DIR})
install(TARGETS simulator DESTINATION bin)

add_executable(hmi_v1 ${${PROJECT}_SOURCE_DIR}/hmi_v1.cpp)
target_link_libraries(hmi_v1 Boost::boost imgui_glfw Eigen3::Eigen)
target_include_directories(hmi_v1 PUBLIC ${${PROJECT}_INCLUDE_DIR})
install(TARGETS hmi_v1 DESTINATION bin)

add_executable(hmi_v2 ${${PROJECT}_SOURCE_DIR}/hmi_v2.cpp)
target_link_libraries(hmi_v2 Boost::boost imgui_glfw Eigen3::Eigen)
target_include_directories(hmi_v2 PUBLIC ${${PROJECT}_INCLUDE_DIR})
install(TARGETS hmi_v2 DESTINATION bin)

add_executable(tcp_test ${${PROJECT}_SOURCE_DIR}/tcp.cpp)
target_link_libraries(tcp_test Boost::boost)
target_include_directories(tcp_test PUBLIC ${${PROJECT}_INCLUDE_DIR})
install(TARGETS tcp_test DESTINATION bin)




