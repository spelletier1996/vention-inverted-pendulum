# Set the project name
project (simulation)

# Find dependencies
find_package(Eigen3 REQUIRED)

# Add a library with the listed sources
add_library(${PROJECT_NAME} src/inverted_pendulum.cpp)

#link dependencies
target_link_libraries(${PROJECT_NAME} Eigen3::Eigen utils)

#link the include directories
target_include_directories( ${PROJECT_NAME}
    PUBLIC ${PROJECT_SOURCE_DIR}/include
)