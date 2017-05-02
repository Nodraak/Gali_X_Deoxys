#ifndef ACTUATORS_ALL_H_INCLUDED
#define ACTUATORS_ALL_H_INCLUDED

// conf: 8 bits
#define ACT_CONF_NONE           ((0x1 << 0) << 0)
#define ACT_CONF_EXTENDED       ((0x1 << 1) << 0)
#define ACT_CONF_RETRACTED      ((0x1 << 2) << 0)
#define ACT_CONF_LAST           ((0x1 << 3) << 0)

// alternate API
#define ACT_CONF_OPEN           ACT_CONF_EXTENDED
#define ACT_CONF_CLOSED         ACT_CONF_RETRACTED
#define ACT_CONF_ON             ACT_CONF_EXTENDED
#define ACT_CONF_OFF            ACT_CONF_RETRACTED

// actuator: 16 bits
#define ACT_ACTUATOR_NONE       ((0x1 << 0) << 8)
#define ACT_ACTUATOR_HEIGHT     ((0x1 << 1) << 8)
#define ACT_ACTUATOR_VERT       ((0x1 << 2) << 8)
#define ACT_ACTUATOR_HORIZ      ((0x1 << 3) << 8)
#define ACT_ACTUATOR_CLAMP      ((0x1 << 4) << 8)
#define ACT_ACTUATOR_PUMP       ((0x1 << 5) << 8)
#define ACT_ACTUATOR_FLAP       ((0x1 << 6) << 8)
#define ACT_ACTUATOR_PROG       ((0x1 << 7) << 8)
#define ACT_ACTUATOR_LAST       ((0x1 << 8) << 8)

// side: 8 bits
#define ACT_SIDE_NONE           ((0x1 << 0) << 16)
#define ACT_SIDE_LEFT           ((0x1 << 1) << 16)
#define ACT_SIDE_RIGHT          ((0x1 << 2) << 16)
#define ACT_SIDE_LAST           ((0x1 << 3) << 16)

// Mask
#define ACT_CONF_MASK           0x000F
#define ACT_ACTUATOR_MASK       0x0FF0
#define ACT_SIDE_MASK           0xF000

typedef uint32_t t_act;

/*

Gui ?

serial to CAN (CQR)
send CAN (CQR)

rec CAN (CQES)
handle actuators (global struct with conf, ...) (CQES)

*/

#endif


#ifdef IAM_QENTRESORT

#ifndef ACTUATORS_H_INCLUDED
#define ACTUATORS_H_INCLUDED

#include "mbed.h"
#include "common/Debug.h"
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
    Ax12Actuator(const char *name, Ax12Driver *ax12, uint8_t id, uint16_t retracted, uint16_t extended);

    void print(Debug *debug, int depth);
    void set(t_act act, char *val);

    void extend(void);
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
