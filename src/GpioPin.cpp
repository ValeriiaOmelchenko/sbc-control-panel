#include "../includes/GpioPin.hpp"
#include <fstream>
#include <stdexcept>
#include <unistd.h>     
#include <sstream>

GpioPin::GpioPin(int pinNumber, pinMode mode)
    : pin_(pinNumber), mode_(mode) {
    exportPin();
    setDirection();
    valuePath_ = "/sys/class/gpio/gpio" + std::to_string(pin_) + "/value";
}

GpioPin::~GpioPin() {
    std::ofstream unexportFile("/sys/class/gpio/unexport");
    if (unexportFile.is_open()) {
        unexportFile << pin_;
    }
}

void GpioPin::exportPin() {
    std::ofstream exportFile("/sys/class/gpio/export");
    if (!exportFile.is_open()) {
        throw std::runtime_error("Failed to open /sys/class/gpio/export");
    }
    exportFile << pin_;
    exportFile.close();

    usleep(100000); // it could have been done better (like all other delays in this code)
}

void GpioPin::setDirection() {
    std::string directionPath = "/sys/class/gpio/gpio" + std::to_string(pin_) + "/direction";
    std::ofstream directionFile(directionPath);
    if (!directionFile.is_open()) {
        throw std::runtime_error("Failed to open " + directionPath);
    }
    directionFile << (mode_ == pinMode::Out ? "out" : "in");
    directionFile.close();
}

void GpioPin::write(bool value) {
    if (mode_ != pinMode::Out) {
    throw std::logic_error("Attempt to write to GPIO configured as input");
    }

    std::ofstream valueFile(valuePath_);
    if (!valueFile.is_open()) {
        throw std::runtime_error("Failed to open " + valuePath_ + " for writing");
    }
    valueFile << (value ? "1" : "0");
    valueFile.close();
}

bool GpioPin::read() const {
    if (mode_ != pinMode::In) {
    throw std::logic_error("Attempt to read from GPIO configured as output");
    }

    std::ifstream valueFile(valuePath_);
    if (!valueFile.is_open()) {
        throw std::runtime_error("Failed to open " + valuePath_ + " for reading");
    }

    std::string value;
    valueFile >> value;
    valueFile.close();
    return value == "1";
}
