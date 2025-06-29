#pragma once

#include "GpioPin.hpp"
#include "ZmqService.hpp"
#include "StateTypes.hpp"

#include <chrono>

class ButtonWatcher {
public:
    ButtonWatcher(GpioPin& pin, ZmqService& zmqService, const std::string& shutdownEndpoint);
    void update();
    States::ButtonState getState() const;

private:
    GpioPin& pin_;
    ZmqService& zmq_;
    std::string shutdownEndpoint_;
    States::ButtonState state_;
    std::chrono::steady_clock::time_point pressedAt_;
    bool shutdownSent_;
};
