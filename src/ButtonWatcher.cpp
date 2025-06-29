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
    auto now = std::chrono::steady_clock::now();

    switch (state_) {
        case States::ButtonState::Idle:
            if (isPressed) {
                pressedAt_ = now;
                state_ = States::ButtonState::Pressed;
                std::cout << "[ButtonWatcher] -> Pressed\n";
            }
            break;

        case States::ButtonState::Pressed:
            if (!isPressed) {
                state_ = States::ButtonState::Idle;
                std::cout << "[ButtonWatcher] -> Idle (bounce)\n";
            // protection against contact rattle
            } else if (now - pressedAt_ > DEBOUNCE_TIME_MS) {
                state_ = States::ButtonState::Holding;
                std::cout << "[ButtonWatcher] -> Holding\n";
            }
            break;

        case States::ButtonState::Holding:
            if (!isPressed) {
                state_ = States::ButtonState::Idle;
                std::cout << "[ButtonWatcher] -> Idle (released early)\n";
            } else if (now - pressedAt_ > HOLD_TIME_SEC) {
                if (!shutdownSent_) {
                    zmq_.send(shutdownEndpoint_, "shutdown");
                    shutdownSent_ = true;
                    std::cout << "[ButtonWatcher] Shutdown triggered\n";
                }
                state_ = States::ButtonState::Triggered;
                std::cout << "[ButtonWatcher] -> Triggered\n";
            }
            break;

        case States::ButtonState::Triggered:
            if (!isPressed) {
                state_ = States::ButtonState::Idle;
                shutdownSent_ = false;
                std::cout << "[ButtonWatcher] -> Idle (after trigger)\n";
            }
            break;
    }
}


States::ButtonState ButtonWatcher::getState() const {
    return state_;
}
