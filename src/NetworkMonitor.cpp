#include "../includes/NetworkMonitor.hpp"
#include "../includes/Logger.hpp"
#include <cstdlib>
#include <fstream>
#include <string>
#include <chrono>

using namespace std::chrono;

constexpr seconds CHECK_INTERVAL(3s);

NetworkMonitor::NetworkMonitor(LedController& led)
    : led_(led),
      state_(States::NetworkStatus::NoNetwork),
      lastCheck_(steady_clock::now() - CHECK_INTERVAL)
{}

void NetworkMonitor::update() {
    auto now = std::chrono::steady_clock::now();

    if (now - lastCheck_ < std::chrono::seconds(2))
        return;

    lastCheck_ = now;

    if (!hasNetworkInterface()) {
        Logger::get()->warn("[NetworkMonitor] No network interfaces detected");
        led_.setPattern(States::LedPattern::BlinkFast);
        state_ = States::NetworkStatus::NoNetwork;
    } else if (!hasInternetConnection()) {
        Logger::get()->info("[NetworkMonitor] Local network available, no Internet");
        led_.setPattern(States::LedPattern::BlinkSlow);
        state_ = States::NetworkStatus::LocalOnly;
    } else {
        Logger::get()->info("[NetworkMonitor] Internet connection available");
        led_.setPattern(States::LedPattern::Solid);
        state_ = States::NetworkStatus::Connected;
    }
}

States::NetworkStatus NetworkMonitor::getState() const {
    return state_;
}

bool NetworkMonitor::hasInternetConnection() {
    int result = std::system("ping -c 1 -W 1 8.8.8.8 > /dev/null 2>&1");
    Logger::get()->debug("[NetworkMonitor] Ping result: {}", result);
    return result == 0;
}

bool NetworkMonitor::hasNetworkInterface() {
    std::ifstream interfaces("/proc/net/dev");
    std::string line;
    while (std::getline(interfaces, line)) {
        if (line.find("lo:") != std::string::npos) continue;
        if (line.find(":") != std::string::npos) {
            Logger::get()->debug("[NetworkMonitor] Found interface: {}", line);
            return true;
        }
    }
    Logger::get()->debug("[NetworkMonitor] No external network interfaces found");
    return false;
}
