
#include "mbed.h"

#include "utils.h"

#include "Motor.h"


Motor::Motor(PinName pwm_pin, PinName dir_pin, bool forward_dir, PinName current_sense) :
    pwm_(pwm_pin), dir_(dir_pin), current_sense_(current_sense)
{
    pwm_.period(0.001 * 0.05);      // 0.00005 == 20K Hz (value from Gali IX)
    setSpeed(0);

    forward_dir_ = forward_dir;
}

void Motor::setSpeed(float speed) {
    setPwm(ABS(speed));
    setDirection(speed >= 0);
}

void Motor::setPwm(float pwm) {
    if (pwm < PWM_ERROR_TOLERANCE)
        pwm_ = 0;
    else
    {
        float val = map(pwm, 0, 1, PWM_MIN, 1);
        if (val > PWM_MAX)
            val = PWM_MAX;
        pwm_ = val;
    }
}

void Motor::setDirection(bool dir) {
    dir_ = dir ? forward_dir_ : !forward_dir_;
}

float Motor::getPwm(void) {
    return pwm_.read();
}

bool Motor::getDirection(void) {
    return dir_ == forward_dir_;
}
