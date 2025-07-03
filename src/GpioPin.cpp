#include "../includes/GpioPin.hpp"
#include "../includes/Logger.hpp"
#include <fstream>
#include <stdexcept>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <sstream>
#include <wiringPi.h>

GpioPin::GpioPin(int pinNumber, pinMode mode)
    : pin_(pinNumber), mode_(mode), fd_(-1) {
    Logger::get()->debug("Initializing GPIO pin {}", pin_);

    if (mode_ == pinMode::PwmOut) {
        wiringPiSetup();
        ::pinMode(pin_, PWM_OUTPUT);
        Logger::get()->debug("Set pin {} as PWM_OUTPUT (wiringPi mode)", pin_);
        return;
    }

    exportPin();
    setDirection();
    valuePath_ = "/sys/class/gpio/gpio" + std::to_string(pin_) + "/value";

    if (mode_ == pinMode::In) {
        fd_ = open(valuePath_.c_str(), O_RDONLY | O_NONBLOCK);
        if (fd_ < 0) {
            Logger::get()->error("Failed to open {} for polling", valuePath_);
            throw std::runtime_error("Failed to open " + valuePath_ + " for polling");
        }
    }
}

GpioPin::~GpioPin() {
    if (mode_ == pinMode::PwmOut) return;

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

    usleep(100000);
}

void GpioPin::setDirection() {
    if (mode_ == pinMode::PwmOut) {
    wiringPiSetup();
    ::pinMode(pin_, PWM_OUTPUT);
    pwmSetMode(PWM_MODE_MS);       
    pwmSetClock(384);              
    pwmSetRange(1024);             
    Logger::get()->debug("Set GPIO {} mode to PWM_OUTPUT", pin_);
    return;
}

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
        Logger::get()->error("write() not allowed on GPIO {} (mode != Out)", pin_);
        throw std::logic_error("Cannot call write() on this GPIO mode");
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
        Logger::get()->error("read() not allowed on GPIO {} (mode != In)", pin_);
        throw std::logic_error("Cannot call read() on this GPIO mode");
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

void GpioPin::writePwm(int duty10bit) {
    if (mode_ != pinMode::PwmOut) {
        Logger::get()->error("Attempt to use PWM on GPIO {} not set to PwmOut", pin_);
        throw std::logic_error("writePwm() called on non-PWM pin");
    }

    pwmWrite(pin_, duty10bit);
}
