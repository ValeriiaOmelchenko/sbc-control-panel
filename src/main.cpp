#include "../includes/GpioPin.hpp"
#include "../includes/LedController.hpp"
#include "../includes/MicrophoneMonitor.hpp"
#include <thread>
#include <chrono>
#include <iostream>

int main() {
    using namespace std::chrono_literals;

    std::cout << "=== MicrophoneMonitor Test ===\n";

    GpioPin micLedPin(18, GpioPin::pinMode::Out);
    LedController micLed(micLedPin);

    MicrophoneMonitor micMonitor(micLed, 0, 0x48);

    while (true) {
        micMonitor.update(); 
        std::this_thread::sleep_for(50ms); 
    }

    return 0;
}
