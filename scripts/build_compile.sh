#!/usr/bin/env bash

if [[ -d build ]]; then
    echo "Build directory already exists. Overwriting..."
    rm -rf build
fi

mkdir local_build
cd local_build
cmake ..
make