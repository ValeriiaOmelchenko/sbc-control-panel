#!/bin/bash

set -e  

echo "Cleaning and creating build directory..."
rm -rf build
mkdir build
cd build

echo "Running CMake configuration..."
cmake ..

echo "Building the project..."
make

echo "Build completed successfully"
