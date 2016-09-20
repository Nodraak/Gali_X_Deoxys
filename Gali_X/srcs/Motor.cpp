
#include "mbed.h"

#include "Motor.h"

// todo: clean this
double map(double x, double in_min, double in_max, double out_min, double out_max);
float abs(float f);


Motor::Motor(PinName pwm_pin, PinName dir_pin, bool forward_dir) : pwm_(pwm_pin), dir_(dir_pin) {
    pwm_.period(0.001 * 0.1);      // 0.0001 == 10K Hz
    setSpeed(0);

    forward_dir_ = forward_dir;
}

void Motor::setSpeed(float speed) {
    setPwm(abs(speed));
    setDirection(speed >= 0);
}

void Motor::setPwm(float pwm) {
    if (pwm < PWM_ERROR_TOLERANCE)
        pwm_ = 0;
    else
        pwm_ = map(pwm, 0, 1, PWM_MIN, 1);
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
