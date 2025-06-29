#include "../includes/GpioPin.hpp"
#include "../includes/LedController.hpp"
#include "../includes/NetworkMonitor.hpp"
#include <thread>
#include <chrono>
#include <iostream>

class TestNetworkMonitor : public NetworkMonitor {
public:
    TestNetworkMonitor(LedController& led)
        : NetworkMonitor(led), simulateNoInternet(false), simulateNoNetwork(false) {}

    void setSimulateNoInternet(bool v) { simulateNoInternet = v; }
    void setSimulateNoNetwork(bool v) { simulateNoNetwork = v; }

protected:
    bool hasNetworkInterface() override {
        return !simulateNoNetwork;
    }

    bool hasInternetConnection() override {
        return !simulateNoInternet;
    }

private:
    bool simulateNoInternet;
    bool simulateNoNetwork;
};

int main() {
    using namespace std::chrono_literals;

    std::cout << "=== Simulated NetworkMonitor Test ===\n";

    GpioPin netLedPin(27, GpioPin::pinMode::Out);
    LedController netLed(netLedPin);

    TestNetworkMonitor network(netLed);

    int counter = 0;
    while (true) {
        if (counter == 10) {
            std::cout << "[Simulate] NoInternet\n";
            network.setSimulateNoInternet(true);
        } else if (counter == 20) {
            std::cout << "[Simulate] NoNetwork\n";
            network.setSimulateNoNetwork(true);
        } else if (counter == 30) {
            std::cout << "[Simulate] Internet connection\n";
            network.setSimulateNoInternet(false);
            network.setSimulateNoNetwork(false);
            counter = 0;  
        }

        network.update();              
        std::this_thread::sleep_for(1s); 
        counter++;
    }

    return 0;
}
