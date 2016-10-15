#ifndef MOTOR_H_INCLUDED
#define MOTOR_H_INCLUDED

#include "mbed.h"


#define PWM_MIN 0.08  // pwm value at which the robot start moving
#define PWM_MAX 1.00  // should be 1.00 during matchs
#define PWM_ERROR_TOLERANCE 0.05

#define MOTOR_DIR_LEFT_FORWARD 0
#define MOTOR_DIR_LEFT_BACKWARD 1

#define MOTOR_DIR_RIGHT_FORWARD 1
#define MOTOR_DIR_RIGHT_BACKWARD 0


class Motor {

public:
    Motor(PinName pwm_pin, PinName dir_pin, bool forward_dir);

    /*
        Set speed (direction and pwm).
        Value between -1 and +1.
    */
    void setSpeed(float speed);

    /*
        Set unsigned PWM value.
    */
    void setPwm(float pwm);

    /*
        Set direction.
            false (0): backward
            true (1): forward
    */
    void setDirection(bool dir);

    /*
        Get unsigned PWM value.
    */
    float getPwm(void);

    /*
        Get direction.
        Cf. setDirection().
    */
    bool getDirection(void);

protected:
    PwmOut pwm_;
    DigitalOut dir_;
    bool forward_dir_;
};

#endif
