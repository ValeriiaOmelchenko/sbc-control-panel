#include "../includes/GpioPin.hpp"
#include "../includes/Logger.hpp"
#include <fstream>
#include <stdexcept>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <sstream>

GpioPin::GpioPin(int pinNumber, pinMode mode)
    : pin_(pinNumber), mode_(mode) {
    Logger::get()->debug("Initializing GPIO pin {}", pin_);
    exportPin();
    setDirection();
    valuePath_ = "/sys/class/gpio/gpio" + std::to_string(pin_) + "/value";

    if (mode == pinMode::In) {
        fd_ = open(valuePath_.c_str(), O_RDONLY | O_NONBLOCK);
        if (fd_ < 0) {
            Logger::get()->error("Failed to open {} for polling", valuePath_);
            throw std::runtime_error("Failed to open " + valuePath_ + " for polling");
        }
    }
}

GpioPin::~GpioPin() {
    std::ofstream unexportFile("/sys/class/gpio/unexport");
    if (unexportFile.is_open()) {
        Logger::get()->debug("Unexporting GPIO pin {}", pin_);
        unexportFile << pin_;
    } else {
        Logger::get()->warn("Failed to open /sys/class/gpio/unexport for pin {}", pin_);
    }
}

void GpioPin::exportPin() {
    std::ofstream exportFile("/sys/class/gpio/export");
    if (!exportFile.is_open()) {
        Logger::get()->error("Failed to open /sys/class/gpio/export");
        throw std::runtime_error("Failed to open /sys/class/gpio/export");
    }
    exportFile << pin_;
    exportFile.close();
    Logger::get()->debug("Exported GPIO pin {}", pin_);

    usleep(100000); // it could have been done better (like all other delays in this code)
}

void GpioPin::setDirection() {
    std::string directionPath = "/sys/class/gpio/gpio" + std::to_string(pin_) + "/direction";
    std::ofstream directionFile(directionPath);
    if (!directionFile.is_open()) {
        Logger::get()->error("Failed to open {} for direction", directionPath);
        throw std::runtime_error("Failed to open " + directionPath);
    }
    directionFile << (mode_ == pinMode::Out ? "out" : "in");
    directionFile.close();
    Logger::get()->debug("Set GPIO {} direction to {}", pin_, (mode_ == pinMode::Out ? "out" : "in"));
}

void GpioPin::write(bool value) {
    if (mode_ != pinMode::Out) {
        Logger::get()->error("Attempt to write to GPIO {} configured as input", pin_);
        throw std::logic_error("Attempt to write to GPIO configured as input");
    }

    std::ofstream valueFile(valuePath_);
    if (!valueFile.is_open()) {
        Logger::get()->error("Failed to open {} for writing", valuePath_);
        throw std::runtime_error("Failed to open " + valuePath_ + " for writing");
    }
    valueFile << (value ? "1" : "0");
    valueFile.close();
    Logger::get()->trace("Wrote {} to GPIO {}", value ? "1" : "0", pin_);
}

bool GpioPin::read() const {
    if (mode_ != pinMode::In) {
        Logger::get()->error("Attempt to read from GPIO {} configured as output", pin_);
        throw std::logic_error("Attempt to read from GPIO configured as output");
    }

    std::ifstream valueFile(valuePath_);
    if (!valueFile.is_open()) {
        Logger::get()->error("Failed to open {} for reading", valuePath_);
        throw std::runtime_error("Failed to open " + valuePath_ + " for reading");
    }

    std::string value;
    valueFile >> value;
    valueFile.close();
    Logger::get()->trace("Read '{}' from GPIO {}", value, pin_);
    return value == "1";
}

void GpioPin::setEdgeTrigger(const std::string& edge) {
    std::string edgePath = "/sys/class/gpio/gpio" + std::to_string(pin_) + "/edge";
    std::ofstream edgeFile(edgePath);
    if (!edgeFile.is_open()) {
        Logger::get()->error("Failed to open {} for writing", edgePath);
        throw std::runtime_error("Failed to open " + edgePath + " for writing");
    }
    edgeFile << edge;
    edgeFile.close();
    Logger::get()->debug("Set GPIO {} edge to '{}'", pin_, edge);
}

bool GpioPin::poll(int timeoutMs) {
    if (fd_ < 0) {
        Logger::get()->error("GPIO {} not configured for polling", pin_);
        throw std::runtime_error("GPIO not configured for polling");
    }

    char buf;
    lseek(fd_, 0, SEEK_SET);
    ::read(fd_, &buf, 1);

    struct pollfd pfd;
    pfd.fd = fd_;
    pfd.events = POLLPRI | POLLERR;

    int ret = ::poll(&pfd, 1, timeoutMs);
    Logger::get()->trace("Polling GPIO {} returned {}", pin_, ret);
    return ret > 0;
}
