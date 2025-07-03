#include "../includes/LedController.hpp"
#include "../includes/Logger.hpp"
#include <iostream>
#include <wiringPi.h>

using namespace std::chrono;

constexpr milliseconds SLOW_BLINK_PERIOD(400);
constexpr milliseconds FAST_BLINK_PERIOD(100);

LedController::LedController(GpioPin& pin)
    : pin_(pin),
      pattern_(States::LedPattern::Off),
      ledOn_(false),
      lastToggle_(steady_clock::now())
{
    if (pin_.getMode() == GpioPin::pinMode::Out) {
        pin_.write(false);
}
}

void LedController::setPattern(States::LedPattern pattern) {
    if (pattern_ == pattern) return; 
    pattern_ = pattern;
    lastToggle_ = steady_clock::now();

    switch (pattern_) {
        case States::LedPattern::Off:
            pin_.write(false);
            ledOn_ = false;
            Logger::get()->debug("[Pattern] Off");
            break;
        case States::LedPattern::Solid:
            pin_.write(true);
            ledOn_ = true;
            Logger::get()->debug("[Pattern] Solid");
            break;
        case States::LedPattern::BlinkSlow:
            Logger::get()->debug("[Pattern] BlinkSlow");
            break;
        case States::LedPattern::BlinkFast:
            Logger::get()->debug("[Pattern] BlinkFast");
            break;
    }
}

States::LedPattern LedController::getPattern() const {
    return pattern_;
}

void LedController::update() {
    if (pattern_ == States::LedPattern::BlinkSlow || pattern_ == States::LedPattern::BlinkFast) {
        auto now = steady_clock::now();
        auto period = getBlinkPeriod();

        if (now - lastToggle_ >= period) {
            ledOn_ = !ledOn_;
            pin_.write(ledOn_);
            lastToggle_ = now;
        }
    }
}

milliseconds LedController::getBlinkPeriod() const {
    return (pattern_ == States::LedPattern::BlinkFast) ? FAST_BLINK_PERIOD : SLOW_BLINK_PERIOD;
}

void LedController::setPwm(int dutyCyclePercent) {
    int pwmValue = dutyCyclePercent * 1024 / 100;
    pin_.writePwm(pwmValue);
}

