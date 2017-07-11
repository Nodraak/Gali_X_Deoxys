#ifdef IAM_QBOUGE

#ifndef MOTOR_H_INCLUDED
#define MOTOR_H_INCLUDED

#include "mbed.h"

#define PWM_MIN                 0.08                        // pwm value at which the robot starts moving
#define PWM_MAX                 1.00                        // should always be 1. Maximum PWM should be set by MC

// Pwm goes from 0 to 1 over a X sec timespan.
// This is used to increment slowly the speed, to prevent the wheels to slip
// Note: be sure to have a steep increase (low PWM_STEP_1_AFTER_X_SEC value),
// otherwise it might fucked up the PID settings
#define PWM_STEP_1_AFTER_X_SEC  0.25
// This is acceleration and should not be constant, but be incremented -> https://www.rcva.fr/10-ans-dexperience/4/
#define PWM_STEP                (1.0/(ASSERV_FPS*PWM_STEP_1_AFTER_X_SEC))

#define PWM_IS_ALMOST_ZERO      (PWM_STEP/2)                // Pwm under this value is considered to be 0

/*
    Forward and backward direction (value of the digital input `direction` of
    the H bridge) for each motor.

    Wheels must turn in opposite directions to move the robot forward or backwards.
    This can be set via the code (here, from code to H bridge), or in the PCB
    routing (from H bridge to plug), or in the motor wiring (plug to motor).
*/
#define MOTOR_DIR_LEFT_FORWARD      1
#define MOTOR_DIR_LEFT_BACKWARD     0
#define MOTOR_DIR_RIGHT_FORWARD     0
#define MOTOR_DIR_RIGHT_BACKWARD    1


class Motor {

public:
    Motor(PinName pwm_pin, PinName dir_pin, bool forward_dir);

    /*
        Set direction.
            false (0): backward
            true (1): forward
    */
    void setDir(bool dir);

    /*
        Set unsigned PWM value.
        Value between 0 and 1.
    */
    void setUPwm(float uPwm);

    /*
        Set speed (direction and pwm).
        Value between -1 and +1.
    */
    void setSPwm(float sPwm);

    /*
        Get direction.
            false (0): backward
            true (1): forward
    */
    bool getDir(void);

    /*
        Set unsigned PWM value.
        Value between 0 and 1.
    */
    float getUPwm(void);

    /*
        Get theorical speed (direction and pwm).
        Value between -1 and +1.
    */
    float getSPwm(void);

    /*
        Compute and save the actual speed of the wheel. This value is computed
        from encoder ticks.
    */
    void updateSpeed(float mm_since_last_loop);

    /*
        Return the computed speed.
        Unit: mm/sec
    */
    float getSpeed(void);

protected:
    PwmOut pwm_;
    DigitalOut dir_;
    bool forward_dir_;
    float speed_;  // unit: mm/sec
    float last_sPwm_;
};

#endif // #ifndef MOTOR_H_INCLUDED
#endif // #ifdef IAM_QBOUGE
