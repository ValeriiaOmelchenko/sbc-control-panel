#pragma once

#include "GpioPin.hpp"
#include "StateTypes.hpp"

#include <chrono>

class LedController {
public:
    explicit LedController(GpioPin& pin);

    void setPattern(States::LedPattern pattern);
    States::LedPattern getPattern() const;

    void update();

private:
    GpioPin& pin_;
    States::LedPattern pattern_;
    bool ledOn_;
    std::chrono::steady_clock::time_point lastToggle_;
    std::chrono::milliseconds getBlinkPeriod() const;
};
