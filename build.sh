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

if systemctl is-active --quiet "$SERVICE_NAME"; then
    echo "Restarting $SERVICE_NAME..."
    sudo systemctl restart "$SERVICE_NAME"
    echo "$SERVICE_NAME restarted."
else
    echo "$SERVICE_NAME is not running."
fi

echo "Build completed successfully"
