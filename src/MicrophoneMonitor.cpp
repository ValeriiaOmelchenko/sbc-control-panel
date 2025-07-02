#include "../includes/MicrophoneMonitor.hpp"
#include "../includes/Logger.hpp"
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
    adc_->setGain(GAIN_FOUR); 
    adc_->begin();
}

int MicrophoneMonitor::readAdcRaw() {
    if (!adc_) {
        Logger::get()->error("[MicrophoneMonitor] ADC not initialized!");
        return 0;
    }
    return adc_->readADC_SingleEnded(adcChannel_);
}

void MicrophoneMonitor::update() {
    auto now = steady_clock::now();
    if (now - lastRead_ >= readInterval_) {
        lastRead_ = now;

        try {
            int value = adc_->readADC_SingleEnded(adcChannel_);
            float volts = value * (1.024 / 2048); 
            Logger::get()->debug("[MicrophoneMonitor] Raw = {}, Volts = {}", value, volts);

            if (value >= 0 && value <= 4095) {
                led_.setPattern(States::LedPattern::Solid); 
            } else {
                led_.setPattern(States::LedPattern::Off); 
            }

        } catch (const std::exception& e) {
            Logger::get()->error("[MicrophoneMonitor] ADC read error: {}", e.what());
            led_.setPattern(States::LedPattern::BlinkFast);
        }
    }
}

float MicrophoneMonitor::calculateAmplitude() {
    return 0;
}
