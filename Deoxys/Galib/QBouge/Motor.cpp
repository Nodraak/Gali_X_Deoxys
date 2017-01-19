#ifdef IAM_QBOUGE

#include "mbed.h"

#include "common/utils.h"
#include "QBouge/MotionController.h"
#include "config.h"

#include "Motor.h"


Motor::Motor(
    PinName pwm_pin, PinName dir_pin, bool forward_dir, PinName current_sense, PinName thermal_flag, PinName brake
) :
    pwm_(pwm_pin), dir_(dir_pin), current_sense_(current_sense), thermal_flag_(thermal_flag), brake_(brake)
{
    forward_dir_ = forward_dir;

    pwm_.period(0.001 * 0.05);      // 0.00005 == 20K Hz (value from Gali IX)

    this->setSPwm(0);
}

void Motor::setDir(bool dir) {
    dir_ = dir ? forward_dir_ : !forward_dir_;
}

void Motor::setUPwm(float uPwm) {
    if (uPwm < PWM_ERROR_TOLERANCE)
        pwm_ = 0;
    else
    {
        float val = map(uPwm, 0, 1, PWM_MIN, 1);

        // cap based on absolute max (define). That way, changing PWM_MAX does not change the PIDs settings.
        if (val > PWM_MAX)
            val = PWM_MAX;

        // cap based on current speed of the wheel
        // todo: improve this (by reading speed_ for ex)
        float max_pwm = this->getUPwm() + 0.5;
        if (val > max_pwm)
            val = max_pwm;

        pwm_ = val;
    }
}

void Motor::setSPwm(float sPwm) {
    this->setUPwm(ABS(sPwm));
    this->setDir(sPwm >= 0);
}

bool Motor::getDir(void) {
    return dir_ == forward_dir_;
}

float Motor::getUPwm(void) {
    return pwm_.read();
}

float Motor::getSPwm(void) {
    return this->getDir() ? this->getUPwm() : -this->getUPwm();
}

void Motor::updateSpeed(float mm_since_last_loop) {
    speed_ = mm_since_last_loop / PID_UPDATE_INTERVAL;
}

float Motor::getSpeed(void) {
    return speed_;
}

#endif // #ifdef IAM_QBOUGE
