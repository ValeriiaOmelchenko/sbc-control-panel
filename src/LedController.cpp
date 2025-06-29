#include "../includes/LedController.hpp"

using namespace std::chrono;

constexpr milliseconds SLOW_BLINK_PERIOD(200);
constexpr milliseconds FAST_BLINK_PERIOD(50);

LedController::LedController(GpioPin& pin)
    : pin_(pin),
      pattern_(States::LedPattern::Off),
      ledOn_(false),
      lastToggle_(steady_clock::now())
{
    pin_.write(false);
}

void LedController::setPattern(States::LedPattern pattern) {
    pattern_ = pattern;
    lastToggle_ = steady_clock::now();

    switch (pattern_) {
        case States::LedPattern::Off:
            pin_.write(false);
            ledOn_ = false;
            break;
        case States::LedPattern::Solid:
            pin_.write(true);
            ledOn_ = true;
            break;
        case States::LedPattern::BlinkSlow:
        case States::LedPattern::BlinkFast:
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
