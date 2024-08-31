#!/usr/bin/env bash

if [[ -d build ]]; then
    echo "Build directory already exists. Overwriting..."
    rm -rf build
fi

mkdir build
cd build
cmake ..
make