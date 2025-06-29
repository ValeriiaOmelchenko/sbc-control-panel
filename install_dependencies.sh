#!/bin/bash

set -e  

echo "Updating package list..."
sudo apt update

echo "Installing required packages..."
sudo apt install -y \
    g++ \
    cmake \
    pkg-config \
    libzmq3-dev \
    wiringpi

echo "The following packages have been installed:"
echo "  - g++ (C++ compiler)"
echo "  - cmake (build system)"
echo "  - pkg-config (library discovery tool)"
echo "  - libzmq3-dev (ZeroMQ development library)"
echo "  - wiringPi (GPIO/I2C/SPI library for Raspberry Pi)"

echo "System is ready for project build."
