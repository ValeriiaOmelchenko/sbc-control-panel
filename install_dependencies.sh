#!/bin/bash

set -e  

echo "Updating package list..."
sudo apt update

echo "Installing required packages..."
sudo apt install -y \
    g++ \
    cmake \
    pkg-config \
    libzmq3-dev\
    libspdlog-dev

echo "The following packages have been installed:"
echo "  - g++ (C++ compiler)"
echo "  - cmake (build system)"
echo "  - pkg-config (library discovery tool)"
echo "  - libzmq3-dev (ZeroMQ development library)"
echo "  - libspdlog-dev (modern C++ logging library)"

echo "System is ready for project build."
