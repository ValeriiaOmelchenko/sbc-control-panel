#include "ButtonWatcher.hpp"
#include <iostream>

constexpr std::chrono::milliseconds DEBOUNCE_TIME_MS(50);
constexpr std::chrono::seconds HOLD_TIME_SEC(3);

ButtonWatcher::ButtonWatcher(GpioPin& pin, ZmqService& zmqService, const std::string& shutdownEndpoint)
    : pin_(pin),
      zmq_(zmqService),
      shutdownEndpoint_(shutdownEndpoint),
      state_(States::ButtonState::Idle),
      shutdownSent_(false)
{}

void ButtonWatcher::update() {
    bool isPressed = !pin_.read();

    switch (state_) {
        case States::ButtonState::Idle:
            if (isPressed) {
                pressedAt_ = std::chrono::steady_clock::now();
                state_ = States::ButtonState::Pressed;
            }
            break;

        case States::ButtonState::Pressed:
            if (!isPressed) {
                // protection against contact rattle
                state_ = States::ButtonState::Idle;
            } else if (std::chrono::steady_clock::now() - pressedAt_ > DEBOUNCE_TIME_MS) {
                state_ = States::ButtonState::Holding;
            }
            break;

        case States::ButtonState::Holding:
            if (!isPressed) {
                state_ = States::ButtonState::Idle;
            } else if (std::chrono::steady_clock::now() - pressedAt_ > HOLD_TIME_SEC) {
                if (!shutdownSent_) {
                    zmq_.send(shutdownEndpoint_, "shutdown");
                    shutdownSent_ = true;
                    std::cout << "[ButtonWatcher] Shutdown triggered\n";
                }
                state_ = States::ButtonState::Triggered;
            }
            break;

        case States::ButtonState::Triggered:
            if (!isPressed) {
                state_ = States::ButtonState::Idle;
                shutdownSent_ = false;
            }
            break;
    }
}

States::ButtonState ButtonWatcher::getState() const {
    return state_;
}
