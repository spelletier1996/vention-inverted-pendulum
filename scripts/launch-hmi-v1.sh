#!/bin/bash

# Function to execute an executable
execute_exe() {
    if [[ -x "$1" ]]; then
        "$1" &
    else
        echo "Executable not found or not executable: $1"
    fi
}

# Launch the simulator and controller in parallel

execute_exe "./build/hmi_v1"
