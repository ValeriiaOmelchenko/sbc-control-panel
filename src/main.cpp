#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>
#include <csignal>

#include "../includes/GpioPin.hpp"
#include "../includes/LedController.hpp"
#include "../includes/NetworkMonitor.hpp"
#include "../includes/MicrophoneMonitor.hpp"
#include "../includes/ButtonWatcher.hpp"
#include "../includes/StateTypes.hpp"

std::atomic<bool> running{true};

void handleSignal(int signal) {
    running = false;
}

int main() {
    signal(SIGINT, handleSignal);

    // Define GPIO pins 
    GpioPin systemLedPin(17, GpioPin::pinMode::Out);
    GpioPin networkLedPin(27, GpioPin::pinMode::Out);
    GpioPin micLedPin(22, GpioPin::pinMode::Out);
    GpioPin buttonPin(5, GpioPin::pinMode::In);
    buttonPin.setEdgeTrigger("falling");

    // Create controllers
    LedController systemLed(systemLedPin);
    LedController networkLed(networkLedPin);
    LedController micLed(micLedPin);
    NetworkMonitor networkMonitor(networkLed);
    MicrophoneMonitor micMonitor(micLed, 0);  // ADC channel 0
    ButtonWatcher button(buttonPin, running);

    std::thread systemLedThread([&]() {
        while (running) {
            bool systemReady = system("/bin/systemctl is-system-running --quiet") == 0;
            if (systemReady) {
                systemLed.setPattern(States::LedPattern::Solid);
            } else {
                systemLed.setPattern(States::LedPattern::Off);
            }
            systemLed.update();
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    });

    std::thread networkThread([&]() {
        while (running) {
            networkMonitor.update();
            networkLed.update();
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    });

    std::thread micThread([&]() {
        while (running) {
            micMonitor.update();
            micLed.update();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    });

 std::thread buttonThread([&]() {
        button.run(); 
    });

    systemLedThread.join();
    networkThread.join();
    micThread.join();
    buttonThread.join();

    return 0;
}
