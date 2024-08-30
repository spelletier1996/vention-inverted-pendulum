#!/bin/bash

# List of dependencies to install
dependencies=(
    "libglfw3"
    "libglfw3-dev"
    "xorg-dev"
    "libxkbcommon-dev"
    "clang"
    "cmake" 
    "libssl-dev"
    "cmake" 
    "libboost-dev" 
    "libsfml-dev"
)

# Function to install a dependency using apt-get
function install_dependency() {
    dependency=$1
    if ! command -v "$dependency" &> /dev/null; then
        sudo apt-get install -y "$dependency"
        if ! command -v "$dependency" &> /dev/null; then
            echo "$dependency"
        fi
    fi
}

# Install each dependency
for dependency in "${dependencies[@]}"; do
    install_dependency "$dependency"
done

echo "All dependencies installed successfully!"