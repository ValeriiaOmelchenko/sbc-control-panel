#pragma once

#include "GpioPin.hpp"
#include "ZmqService.hpp"
#include "StateTypes.hpp"
#include "LedController.hpp"


#include <chrono>

class ButtonWatcher {
public:
    ButtonWatcher(GpioPin& pin, std::atomic<bool>& running, LedController&  led);
    ButtonWatcher(GpioPin& pin, ZmqService* zmqService, const std::string& shutdownEndpoint,  std::atomic<bool>& running, LedController& led);


    void run(); 
    void update();
    States::ButtonState getState() const;

private:
    std::atomic<bool>& running_;
    GpioPin& pin_;
    ZmqService* zmq_;
    LedController&  led_;
    std::string shutdownEndpoint_;
    States::ButtonState state_;
    std::chrono::steady_clock::time_point pressedAt_;
    bool shutdownSent_;
};
