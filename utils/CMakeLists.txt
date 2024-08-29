# Set the project name
project (utils)

# Add a library with the listed sources
add_library(${PROJECT_NAME} src/network.cpp)
target_link_libraries(${PROJECT_NAME})

#link the include directories
target_include_directories( ${PROJECT_NAME}
    PUBLIC ${PROJECT_SOURCE_DIR}/include
)