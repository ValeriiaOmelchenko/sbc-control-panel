#pragma once

#include "LedController.hpp"
#include <chrono>
#include <memory>
#include "../libs/Adafruit_ADS1X15_RPi/Adafruit_ADS1015.h"


class Adafruit_ADS1015;

class MicrophoneMonitor {
public:
    MicrophoneMonitor(LedController& led, int adcChannel = 0, int i2cAddress = 0x48);

    void update();  
    int readAdcRaw();

private:
    LedController& led_;
    int adcChannel_;
    int i2cAddress_;

    std::unique_ptr<Adafruit_ADS1015> adc_;
    std::chrono::steady_clock::time_point lastRead_;
    const std::chrono::milliseconds readInterval_{500};

    float calculateAmplitude(); // I hope I'll have time to do this
};
