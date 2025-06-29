#include "includes/GpioPin.hpp"
#include "includes/ButtonWatcher.hpp"
#include "includes/ZmqService.hpp"
#include <thread>
#include <chrono>
#include <iostream>

int main() {
    using namespace std::chrono_literals;

    GpioPin buttonPin(5, GpioPin::pinMode::In);

    ZmqService zmq;

    ButtonWatcher button(buttonPin, zmq, "inproc://shutdown");

    std::cout << "Starting button test... Press and hold the button.\n";

    while (true) {
        button.update();  
        std::this_thread::sleep_for(50ms);
    }

    return 0;
}
