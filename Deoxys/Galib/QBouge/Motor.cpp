#ifdef IAM_QBOUGE

#include "mbed.h"

#include "common/utils.h"
#include "config.h"

#include "Motor.h"


Motor::Motor(PinName pwm_pin, PinName dir_pin, bool forward_dir) : pwm_(pwm_pin), dir_(dir_pin) {
    forward_dir_ = forward_dir;
    last_sPwm_ = 0;

    pwm_.period(0.001 * 0.1);      // 0.0001 == 10K Hz (Gali IX : 20KHz) todo define

    this->setSPwm(0);
}

void Motor::setDir(bool dir) {
    dir_ = dir ? forward_dir_ : !forward_dir_;
}

void Motor::setUPwm(float uPwm) {
    pwm_ = uPwm;
}

void Motor::setSPwm(float sPwm) {
    if (ABS(sPwm) < PWM_IS_ALMOST_ZERO)
    {
        sPwm = 0;
        last_sPwm_ = 0;
    }
    else
    {
        float current = last_sPwm_;

        sPwm = constrain(sPwm, -1, 1);

        // step the raw value
        if (ABS(sPwm - current) > PWM_STEP)
        {
            if (sPwm > current)
                sPwm = current + PWM_STEP;
            else
                sPwm = current - PWM_STEP;
        }

        last_sPwm_ = sPwm;

        // map for applying the pwm
        sPwm = SIGN(sPwm) * map(ABS(sPwm), 0, 1, PWM_MIN, 1);

        // cap for boundary checking
        sPwm = constrain(sPwm, -PWM_MAX, PWM_MAX);
    }

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
    speed_ = mm_since_last_loop / ASSERV_DELAY;
}

float Motor::getSpeed(void) {
    return speed_;
}

#endif // #ifdef IAM_QBOUGE
