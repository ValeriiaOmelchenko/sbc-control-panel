#include "../includes/MicrophoneMonitor.hpp"
#include "../includes/Logger.hpp"
#include <wiringPi.h>
#include <thread>

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
    adc_->setGain(GAIN_TWO); 
    adc_->begin();
}

int MicrophoneMonitor::readAdcRaw() {
    if (!adc_) {
        Logger::get()->error("[MicrophoneMonitor] ADC not initialized!");
        return 0;
    }
    int value = adc_->readADC_SingleEnded(adcChannel_);
    Logger::get()->debug("[MicrophoneMonitor] Raw ADC value on channel {}: {}", adcChannel_, value);
    return value;
}


void MicrophoneMonitor::update() {
    auto now = steady_clock::now();
    if (now - lastRead_ >= readInterval_) {
        lastRead_ = now;

        try {
            float amplitude = calculateAmplitude();              
            int pwmPercent = amplitudeToPwm(amplitude);         
            Logger::get()->debug("[MicrophoneMonitor] Amplitude = {:.3f} V → PWM = {}%", amplitude, pwmPercent);

            led_.setPwm(pwmPercent);                         

        } catch (const std::exception& e) {
            Logger::get()->error("[MicrophoneMonitor] ADC read error: {}", e.what());
            led_.setPattern(States::LedPattern::BlinkFast);    
        }
    }
}


float MicrophoneMonitor::calculateAmplitude() {
    constexpr int samples = 50;
    int minVal = 4095;
    int maxVal = 0;

    for (int i = 0; i < samples; ++i) {
        int value = adc_->readADC_SingleEnded(adcChannel_);
        if (value < minVal) minVal = value;
        if (value > maxVal) maxVal = value;
        std::this_thread::sleep_for(std::chrono::milliseconds(2));  
    }

    int peakToPeak = maxVal - minVal;
    float volts = peakToPeak * (2.048f / 2048);  
    return volts;
}

int MicrophoneMonitor::amplitudeToPwm(float amplitudeVolts) {
    constexpr float minVolts = 0.01f;
    constexpr float maxVolts = 1.0f;    

    float clamped = std::min(std::max(amplitudeVolts, minVolts), maxVolts);
    int duty = static_cast<int>((clamped / maxVolts) * 100.0f);
    return duty; 
}