#pragma once

#include <string>

class GpioPin {
public:
    enum class pinMode { In, Out };

    GpioPin(int pinNumber, pinMode mode);
    ~GpioPin();

    void write(bool value);     
    bool read() const;         

private:
    int pin_;
    pinMode mode_;
    std::string valuePath_;    

    void exportPin();
    void setDirection();
};
