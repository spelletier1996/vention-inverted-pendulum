cmake_minimum_required(VERSION 3.2)

project(vention-inverted-pendulum
        VERSION 0.1.0
        LANGUAGES CXX)

# Set the C++ standard to C++23
set(CMAKE_CXX_STANDARD 23)
# Set compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++23 -Wall -Wextra -Wpedantic -g")
set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR})

# TODO: Set the build type

# Find dependencies
find_package(Boost REQUIRED)

# Set include directory
set(${PROJECT}_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
# Set source directory
set(${PROJECT}_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
# Set include directories
include_directories(${PROJECT}_INCLUDE_DIR)

# add the executable and link libraries
add_executable(${PROJECT_NAME} ${${PROJECT}_SOURCE_DIR}/main.cpp)
target_link_libraries(${PROJECT_NAME} Boost::boost)
install(TARGETS ${PROJECT_NAME} DESTINATION bin)




