#ifndef MOTION_CONTROLLER_H_INLCUDED
#define MOTION_CONTROLLER_H_INLCUDED

/*
    The MotionController have the responsability to control the motors to move
    the robot to the specified coordinates. The MotionController is uturly dumb,
    all the smart path optimizations are made by the MotionPlaner.
*/

#include "PID.h"
#include "QEI.h"

#include "Debug.h"
#include "Motor.h"

#include "utils.h"


#define PID_UPDATE_INTERVAL (1.0/10)  // sec

#define ENC_RADIUS          28                      // one enc radius
#define ENC_PERIMETER       (2*M_PI*ENC_RADIUS)     // one enc perimeter
#define TICKS_PER_MM        16.5
#define PULSES_PER_REV      (ENC_PERIMETER*TICKS_PER_MM)

#define ENC_POS_RADIUS      87                      // distance from one enc to the center of the robot
#define TICKS_2PI           (2*M_PI*ENC_POS_RADIUS * TICKS_PER_MM * 2)  // how many enc ticks after a 2*M_PI turn
#define MM_TO_TICKS(val)    ((val)*TICKS_PER_MM)
#define TICKS_TO_MM(val)    ((val)/TICKS_PER_MM)

#define MAX_ORDERS_COUNT    20

// default pid tunning
#define PID_DIST_KU 1.7
#define PID_DIST_TU 0.7
#define PID_ANGLE_KU 6.0
#define PID_ANGLE_TU 0.2

typedef enum    _e_order_type {
    ORDER_TYPE_POS,
    ORDER_TYPE_DIST,
    ORDER_TYPE_ANGLE,
    ORDER_TYPE_DELAY
}               e_order_type;

typedef struct  _s_order {
    bool enabled :1;  // 0: no order, 1: struct filled with an order

    e_order_type type :3;

    s_vector_float pos;
    int dist;
    float angle;  // radians
    float delay;
}               s_order;


class MotionController {
public:
    MotionController(void);

    /*
        Save the encoders value to a working variable so that the various
        computations and the debug are based on the same values within this
        loop.
    */
    void fetchEncodersValue(void);

    /*
        Update the internal state of the MotionController given the value of the
        encoders ticks fetched by fetchEncodersValue().
    */
    // update the position the robot think it is
    void updatePosition(void);
    // recompute the distance and angle correction to apply
    int updateCurOrder(void);

    /*
        Compute the PIDs output based on the internal state of the
        MotionController() computed by updateCurOrder().
    */
    void computePid(void);

    /*
        Apply the PIDs output to the motors.
    */
    void updateMotors(void);

    /*
        Print some information about the inputs, outputs and internal states.
    */
    void debug(Debug *debug);

    /*
        Set the goal that the MotionController will make the robot move to.
    */
    void pidDistSetGoal(float goal);
    void pidAngleSetGoal(float goal);

    /*
        Clear all saved orders.
    */
    void ordersReset(void);

    /*
        Add a new order to the list of orders to execute.
    */
    int ordersAppend(e_order_type type, int16_t x, int16_t y, int dist, float angle, float delay);

    /*
        Set the motors speed (range is 0-1).
    */
    void setMotor(float l, float r);

private:
    /*
        Discard the current order and execute the next one.
        Should be called only when the current order is achieved.
    */
    void updateGoalToNextOrder(void);

private:  // I/O
    Motor motor_l_, motor_r_;  // io interfaces
    QEI enc_l_, enc_r_;  // io interfaces
    PID pid_dist_, pid_angle_;

    int64_t enc_l_last_, enc_r_last_;  // last value of the encoders. Used to determine movement and speed. Unit: enc ticks
    s_order orders_[MAX_ORDERS_COUNT];  // planned movement orders

    // pid
    int64_t enc_l_val_, enc_r_val_;  // tmp variable used as a working var - use this instead of the raw value from the QEI objects. Unit: enc ticks
    float pid_dist_goal_, pid_angle_goal_;  // units: mm and rad
    float pid_dist_out_, pid_angle_out_;  // unit: between -1 and +1

public:
    // robot infos
    s_vector_float pos_;  // unit: mm
    float angle_;  // unit: radians
    float speed_;  // unit: mm/sec
};


int calcNewPos(
    float diff_l, float diff_r,
    float cur_angle, float cur_x, float cur_y,
    float *new_angle_, float *new_x_, float *new_y_
);

#endif
