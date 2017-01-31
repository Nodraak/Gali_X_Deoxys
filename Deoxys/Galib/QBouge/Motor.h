#ifdef IAM_QBOUGE

#ifndef MOTOR_H_INCLUDED
#define MOTOR_H_INCLUDED

#include "mbed.h"

#define PWM_MIN                 0.08                            // pwm value at which the robot start moving
#define PWM_MAX                 1.00                            // should be 1.00 during matchs
#define PWM_STEP                (4.0*ASSERV_DELAY)              // pwm goes from 0 to X over a 1 sec timespan
#define PWM_ERROR_TOLERANCE     0.01                            // value under which a pwm value is considered zero

#define MOTOR_DIR_LEFT_FORWARD 0
#define MOTOR_DIR_LEFT_BACKWARD 1

#define MOTOR_DIR_RIGHT_FORWARD 1
#define MOTOR_DIR_RIGHT_BACKWARD 0


class Motor {

public:
    Motor(
        PinName pwm_pin, PinName dir_pin, bool forward_dir, PinName current_sense, PinName thermal_flag, PinName brake
    );

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

public:
    AnalogIn current_sense_;
    DigitalIn thermal_flag_;
    DigitalOut brake_;
};

/*
    Limit the pwm to smoothes the transition between speeds.

    Arguments:
        requested: new signed pwm value.
        current: current signed pwm value.

    Returns the new signed pwm value.
*/
float motor_cap_pwm(float requested, float current);

#endif // #ifndef MOTOR_H_INCLUDED
#endif // #ifdef IAM_QBOUGE
