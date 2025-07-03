#include "../includes/ButtonWatcher.hpp"
#include "../includes/Logger.hpp"

constexpr std::chrono::milliseconds DEBOUNCE_TIME_MS(50);
constexpr std::chrono::seconds HOLD_TIME_SEC(3);

ButtonWatcher::ButtonWatcher(GpioPin& pin, std::atomic<bool>& running, LedController&  led)
    : pin_(pin), 
    zmq_(nullptr), 
    shutdownEndpoint_(""), 
    state_(States::ButtonState::Idle), 
    shutdownSent_(false), 
    running_(running),
    led_(led) 
{}

ButtonWatcher::ButtonWatcher(GpioPin& pin, ZmqService* zmqService, const std::string& shutdownEndpoint, std::atomic<bool>& running, LedController& led)
    : pin_(pin),
      zmq_(zmqService),
      shutdownEndpoint_(shutdownEndpoint),
      state_(States::ButtonState::Idle),
      shutdownSent_(false), 
      running_(running),
      led_(led) 
{}

void ButtonWatcher::run() {
    Logger::get()->info("[ButtonWatcher] Polling for button events...");

    while (running_) {
        try {
            update();
        } catch (const std::exception& e) {
            Logger::get()->error("[ButtonWatcher] Error in update(): {}", e.what());
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(50)); 
    }
}


void ButtonWatcher::update() {
    bool isPressed = !pin_.read();
    auto now = std::chrono::steady_clock::now();

    switch (state_) {
        case States::ButtonState::Idle:
            if (isPressed) {
                pressedAt_ = now;
                state_ = States::ButtonState::Pressed;
                Logger::get()->info("[ButtonWatcher] -> Pressed");
            }
            break;

        case States::ButtonState::Pressed:
            if (!isPressed) {
                state_ = States::ButtonState::Idle;
                Logger::get()->info("[ButtonWatcher] -> Idle (bounce)");
            } else if (now - pressedAt_ > DEBOUNCE_TIME_MS) {
                state_ = States::ButtonState::Holding;
                Logger::get()->info("[ButtonWatcher] -> Holding");
            }
            break;

        case States::ButtonState::Holding:
            if (!isPressed) {
                state_ = States::ButtonState::Idle;
                Logger::get()->info("[ButtonWatcher] -> Idle (released early)");
            } else if (now - pressedAt_ > HOLD_TIME_SEC && !shutdownSent_) {
                shutdownSent_ = true;  

                if (zmq_) {
                    zmq_->send(shutdownEndpoint_, "shutdown");
                    Logger::get()->warn("[ButtonWatcher] Shutdown message sent via ZMQ");
                } else {
                    Logger::get()->warn("[ButtonWatcher] No ZMQ. Rebooting locally...");
                    system("sudo /sbin/reboot");
                }

                state_ = States::ButtonState::Triggered;
                Logger::get()->info("[ButtonWatcher] -> Triggered");
            }
            break;

        case States::ButtonState::Triggered:
            if (!isPressed) {
                state_ = States::ButtonState::Idle;
                shutdownSent_ = false;
                Logger::get()->info("[ButtonWatcher] -> Idle (after trigger)");
            }
            break;
    }
}


States::ButtonState ButtonWatcher::getState() const {
    return state_;
}
