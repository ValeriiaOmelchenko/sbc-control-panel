#pragma once

#include "LedController.hpp"
#include "StateTypes.hpp"

#include <chrono>

class NetworkMonitor {
public:
    NetworkMonitor(LedController& led);

    void update();

    States::NetworkStatus getState() const;

private:
    LedController& led_;
    States::NetworkStatus state_;
    std::chrono::steady_clock::time_point lastCheck_;

    virtual bool hasNetworkInterface();
    virtual bool hasInternetConnection();
};
