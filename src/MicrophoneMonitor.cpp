#include "../includes/MicrophoneMonitor.hpp"
#include <Adafruit_ADS1015.h>
#include <iostream>
#include <wiringPi.h>

using namespace std::chrono;

MicrophoneMonitor::MicrophoneMonitor(LedController& led, int adcChannel, int i2cAddress)
    : led_(led),
      adcChannel_(adcChannel),
      i2cAddress_(i2cAddress),
      lastRead_(steady_clock::now())
{
    wiringPiSetup(); 
    adc_ = std::make_unique<Adafruit_ADS1015>();
    adc_->setI2cAddress(i2cAddress_);
    adc_->begin();
}

int MicrophoneMonitor::readAdcRaw() {
    if (!adc_) {
        std::cerr << "[MicrophoneMonitor] ADC not initialized!\n";
        return 0;
    }
    return adc_->readADC_SingleEnded(adcChannel_);
}

void MicrophoneMonitor::update() {
    auto now = steady_clock::now();
    if (duration_cast<milliseconds>(now - lastRead_) < readInterval_) return;

    lastRead_ = now;
    int raw = readAdcRaw();

    if (raw > 100) {
        led_.setPattern(States::LedPattern::Solid);
    } else {
        led_.setPattern(States::LedPattern::Off);
    }
}

float MicrophoneMonitor::calculateAmplitude() {

    return 0;
}
