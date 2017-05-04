#ifdef IAM_QENTRESORT

#ifndef ACTUATORS_H_INCLUDED
#define ACTUATORS_H_INCLUDED

#include "mbed.h"
#include "common/Debug.h"
#include "common/OrdersFIFO.h"
#include "QEntreQSort/Ax12Driver.h"

/*
    What:
        Actuators wraps all the low level actuators drivers with settings such
        as min/max value.

    Examples of usage:
        a->left.arm_.move_up()
        a->left.arm_.vert.extend()
        a->left.flap_.extend()
        a->left.flap_.set_extended(xx)
        //a->dist_pid.set_kp(xx)
*/

/*
    # Low level drivers
*/

/*
    ## Servo
*/

class ServoActuator {
public:
    ServoActuator(const char *name, PwmOut servo, float retracted, float extended);

    void print(Debug *debug, int depth);
    void set(t_act act, char *val);
    void activate(t_act act);

    void extend(void);
    void retract(void);

    // alternate API
    void open(void);
    void close(void);

private:
    const char *name_;
    PwmOut servo_;

    float retracted_;
    float extended_;
};

/*
    ## Ax12
*/

class Ax12Actuator {
public:
    Ax12Actuator(const char *name, Ax12Driver *ax12, uint8_t id, uint16_t retracted, uint16_t neutral, uint16_t extended);

    void print(Debug *debug, int depth);
    void set(t_act act, char *val);
    void activate(t_act act);

    void extend(void);
    void neutral(void);
    void retract(void);

    void ping(void);

    uint16_t read_pos(void);
    void write_pos(uint16_t pos);

    uint16_t read_speed(void);
    void write_speed(uint16_t speed);

private:
    const char *name_;
    Ax12Driver *ax12_;

    uint8_t id_;

    uint16_t retracted_;
    uint16_t neutral_;
    uint16_t extended_;

    // uint16_t speed_;
};

/*
    ## Boolean
*/

class BooleanActuator {
public:
    BooleanActuator(const char *name, DigitalOut out, bool retracted, bool extended);

    void print(Debug *debug, int depth);
    void set(t_act act, char *val);
    void activate(t_act act);

    void extend(void);
    void retract(void);

    // alternate API
    void on(void);
    void off(void);

private:
    const char *name_;
    DigitalOut out_;

    bool retracted_;
    bool extended_;
};

/*
    # Medium level drivers
*/

/*
    ## Arm
*/

class ArmActuator {
public:
    ArmActuator(
        Ax12Actuator height,
        Ax12Actuator vert,
        Ax12Actuator horiz,
        ServoActuator clamp,
        BooleanActuator pump
    );

    void print(Debug *debug, int depth);
    void set(t_act act, char *val);
    void activate(t_act act);

    void ping_all(void);

    void read_pos_all(Debug *debug);
    void write_pos_all(uint16_t pos1, uint16_t pos2, uint16_t pos3);

    void read_speed_all(Debug *debug);
    void write_speed_all(uint16_t speed);

    void init(void);
    void grab(void);
    void move_up(void);
    void release(void);
    void move_down(void);

public:
    Ax12Actuator height_;
    Ax12Actuator vert_;
    Ax12Actuator horiz_;
    ServoActuator clamp_;
    BooleanActuator pump_;
};

/*
    ## OneSide
*/

class OneSideCylindersActuators {
public:
    OneSideCylindersActuators(
        const char *name,
        ArmActuator arm,
        ServoActuator flap
    );

    void print(Debug *debug, int depth);
    void set(t_act act, char *val);
    void activate(t_act act);

public:
    const char *name_;
    ArmActuator arm_;
    ServoActuator flap_;
    // rotating device (dc motor + color sensor)
};

/*
    # High level
*/

/*
    ## Actuators
*/

class Actuators {
public:
    Actuators(OneSideCylindersActuators left, OneSideCylindersActuators right, ServoActuator prograde_dispenser);

    void print(Debug *debug, int depth);
    void set(t_act act, char *val);
    void activate(t_act act);

    OneSideCylindersActuators *side(t_act side);

public:
    OneSideCylindersActuators left_;
    OneSideCylindersActuators right_;
    ServoActuator prograde_dispenser_;
    // ((dc motor + micro switch) | step motor) radial_dispenser
    // colorArmActuator side;
};

#endif // #ifndef ACTUATORS_H_INCLUDED
#endif // #ifdef IAM_QENTRESORT

/*

Gui ?

serial to CAN (CQR)
send CAN (CQR)

rec CAN (CQES)
handle actuators (global struct with conf, ...) (CQES)

*/
