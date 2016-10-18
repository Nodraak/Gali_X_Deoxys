#ifndef MOTION_CONTROLLER_H_INLCUDED
#define MOTION_CONTROLLER_H_INLCUDED

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

// order type can be OR'ed to combine different types
typedef enum    _e_order_type {
    ORDER_TYPE_POS      = (1 << 0),
    ORDER_TYPE_DIST     = (1 << 1),
    ORDER_TYPE_ANGLE    = (1 << 2),
    ORDER_TYPE_DELAY    = (1 << 3)
}               e_order_type;

typedef struct  _s_order {
    bool enabled;  // 0: no order, 1: struct filled with an order
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
    void updatePositionAndOrder(void);

    /*
        Compute the PIDs output based on the internal state of the
        MotionController() fetched by fetchEncodersValue().
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

    void ordersReset(void);

    int ordersAppend(e_order_type type, int16_t x, int16_t y, int dist, float angle, float delay);

private:  // I/O
    Motor motor_l_, motor_r_;
    QEI enc_l_, enc_r_;

    int64_t enc_l_last_, enc_r_last_;
    s_order orders_[MAX_ORDERS_COUNT];  // planned movement orders

protected:  // internal
    // pid
    int64_t enc_l_val_, enc_r_val_;
    PID pid_dist_;
    PID pid_angle_;
    float pid_dist_goal_, pid_angle_goal_;
    float out_pid_dist_;
    float out_pid_angle_;

    // robot infos
    s_vector_float pos_;
    float angle_;  // radians
    float speed_;  // mm/sec
};


int calcNewPos(
    int diff_l, int diff_r,
    float cur_angle, float cur_x, float cur_y,
    float *new_angle_, float *new_x_, float *new_y_
);

#endif
