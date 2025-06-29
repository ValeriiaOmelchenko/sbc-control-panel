#include "../includes/GpioPin.hpp"
#include "../includes/LedController.hpp"
#include <thread>
#include <chrono>
#include <iostream>

int main() {
    using namespace std::chrono_literals;

    std::cout << "=== Starting LED Pattern Test ===\n";

    GpioPin ledSystemPin(17, GpioPin::pinMode::Out);
    GpioPin ledNetworkPin(27, GpioPin::pinMode::Out);
    GpioPin ledMicPin(18, GpioPin::pinMode::Out);

    LedController ledSystem(ledSystemPin);
    LedController ledNetwork(ledNetworkPin);
    LedController ledMic(ledMicPin);

    // Solid ON
    std::cout << "[Pattern] Solid\n";
    ledSystem.setPattern(States::LedPattern::Solid);
    ledNetwork.setPattern(States::LedPattern::Solid);
    ledMic.setPattern(States::LedPattern::Solid);
    std::this_thread::sleep_for(3s);

    // OFF
    std::cout << "[Pattern] Off\n";
    ledSystem.setPattern(States::LedPattern::Off);
    ledNetwork.setPattern(States::LedPattern::Off);
    ledMic.setPattern(States::LedPattern::Off);
    std::this_thread::sleep_for(2s);

    // Blinking Fast
    std::cout << "[Pattern] BlinkFast\n";
    ledSystem.setPattern(States::LedPattern::BlinkFast);
    ledNetwork.setPattern(States::LedPattern::BlinkFast);
    ledMic.setPattern(States::LedPattern::BlinkFast);
    for (int i = 0; i < 160; ++i) {
        ledSystem.update();
        ledNetwork.update();
        ledMic.update();
        std::this_thread::sleep_for(25ms);
    }  

    // Blinking Slow
    std::cout << "[Pattern] BlinkSlow\n";
    ledSystem.setPattern(States::LedPattern::BlinkSlow);
    ledNetwork.setPattern(States::LedPattern::BlinkSlow);
    ledMic.setPattern(States::LedPattern::BlinkSlow);
    for (int i = 0; i < 60; ++i) {
        ledSystem.update();
        ledNetwork.update();
        ledMic.update();
        std::this_thread::sleep_for(100ms);  
    }

    std::cout << "=== Test Complete ===\n";
    ledSystem.setPattern(States::LedPattern::Off);
    ledNetwork.setPattern(States::LedPattern::Off);
    ledMic.setPattern(States::LedPattern::Off);

    return 0;
}
