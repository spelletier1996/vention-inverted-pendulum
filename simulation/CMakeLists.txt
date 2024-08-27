# Set the project name
project (simulation)

# Add a library with the listed sources
add_library(${PROJECT_NAME} src/inverted_pendulum.cpp)

#link the include directories
target_include_directories( ${PROJECT_NAME}
    PUBLIC ${PROJECT_SOURCE_DIR}/include
)