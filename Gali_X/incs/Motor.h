#ifndef MOTOR_H_INCLUDED
#define MOTOR_H_INCLUDED

#include "mbed.h"

class Motor {

public:
    Motor(PinName pwm_pin, PinName dir_pin, bool forward_dir);

    void setSpeed(float speed);

    void setPwm(float pwm);

    void setDirection(bool dir);

    float getPwm(void);

    bool getDirection(void);

protected:
    PwmOut pwm_;
    DigitalOut dir_;
    bool forward_dir_;
};

#endif
