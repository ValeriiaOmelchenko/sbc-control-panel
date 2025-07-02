#pragma once

#include <string>

class GpioPin {
public:
    enum class pinMode { In, Out };

    GpioPin(int pinNumber, pinMode mode);
    ~GpioPin();

    void write(bool value);     
    bool read() const;       
    void setEdgeTrigger(const std::string& edge); // "none", "rising", "falling", "both"
    bool poll(int timeoutMs);   

private:
    int pin_;
    pinMode mode_;
    std::string valuePath_;       
    int fd_ = -1;

    void exportPin();
    void setDirection();
};
