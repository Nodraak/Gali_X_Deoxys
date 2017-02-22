#ifdef IAM_QBOUGE

#ifndef MOTION_CONTROLLER_H_INLCUDED
#define MOTION_CONTROLLER_H_INLCUDED

/*
    The MotionController have the responsability to control the motors to move
    the robot to the specified coordinates. The MotionController is uturly dumb,
    all the smart path optimizations are made by the MotionPlaner.
*/

#include "PID.h"

#include "common/Debug.h"
#include "common/Messenger.h"
#include "common/OrdersFIFO.h"
#include "common/utils.h"
#include "QBouge/Motor.h"
#include "QBouge/Qei.h"


#define ENC_RADIUS          28.2                    // one enc radius
#define ENC_PERIMETER       (2*M_PI*ENC_RADIUS)     // one enc perimeter
#define TICKS_PER_MM        16.5

#define ENC_POS_RADIUS      87                      // distance from one enc to the center of the robot

#define MM_TO_TICKS(val)    ((val)*TICKS_PER_MM)
#define TICKS_TO_MM(val)    ((val)/TICKS_PER_MM)

#define MC_TARGET_TOLERANCE_DIST        10.0        // galiIX 12 mm
#define MC_TARGET_TOLERANCE_SPEED       5.0
#define MC_TARGET_TOLERANCE_ANGLE       DEG2RAD(10) // galiIX 7.2 deg - voir 5 ou meme 3.5 deg
#define MC_TARGET_TOLERANCE_ANG_SPEED   DEG2RAD(5)  // unit: rad/sec

// default pid tunning

#define PID_DIST_KU         1.7
#define PID_DIST_TU         0.7

#define PID_ANGLE_KU        3.0
#define PID_ANGLE_TU        0.1

#define PID_DIST_I          0
#define PID_DIST_D          0
#define PID_DIST_P          5.0

#define PID_ANGLE_I         0
#define PID_ANGLE_D         0
#define PID_ANGLE_P         2.0


class MotionController {
public:
    MotionController(void);
    ~MotionController(void);

    /*
        Save the encoders value to a working variable so that the various
        computations and the debug are based on the same values within this
        loop.
    */
    void fetchEncodersValue(void);

    /*
        Update the internal state of the MotionController (position, speed, ...)
        given the value of the encoders ticks fetched by fetchEncodersValue().
    */
    void updatePosition(void);

    /*
        Recompute the distance and angle correction to apply.
        If the current order has been reached, it loads the next one by calling
        updateGoalToNextOrder().
    */
    void updateCurOrder(void);

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
    void debug(CanMessenger *cm);

    /*
        Set the goal that the MotionController will make the robot move to.
    */
    void pidDistSetGoal(float goal);
    void pidAngleSetGoal(float goal);

    /*
        Set the motors speed (range is 0-1).

        ** WARNING **
        This function is dangerous and has side effects. You should not called
        it unless you know what you are doing.
    */
    void setMotor(float l, float r);

    /*
        Reset the OrdersFIFO orders_ and the s_order_exe current_order_.
    */
    void ordersReset(void);

private:  // I/O
    Motor motor_l_, motor_r_;  // io interfaces
    Qei enc_l_, enc_r_;  // io interfaces
public:  // todo fix this security issue
    PID pid_dist_, pid_angle_;
private:

    Timer timer_;
    int32_t enc_l_last_, enc_r_last_;  // last value of the encoders. Used to determine movement and speed. Unit: enc ticks
    float last_order_timestamp_;  // s from match start

    // pid
    int32_t enc_l_val_, enc_r_val_;  // tmp variable used as a working var - use this instead of the raw value from the QEI objects. Unit: enc ticks
    float pid_dist_goal_, pid_angle_goal_;  // units: mm and rad
    float pid_dist_out_, pid_angle_out_;  // unit: between -1 and +1

public:
    // robot infos
    s_vector_float pos_;  // unit: mm
    float angle_;  // unit: radians
    float speed_;  // unit: mm/sec
    float speed_ang_;  // unit: rad/sec

    // planned orders
    OrdersFIFO *orders_;
    s_order_exe current_order_;
};


int mc_calcNewPos(
    float diff_l, float diff_r,
    float cur_angle, float cur_x, float cur_y,
    float *new_angle_, float *new_x_, float *new_y_
);

void mc_calcDistThetaOrderPos(float *dist_, float *theta_);

int mc_updateCurOrder(
    s_vector_float cur_pos,  float cur_angle, float cur_speed, float cur_speed_ang,
    s_order_exe *cur_order, float time_since_last_order_finished,
    float *dist_, float *theta_
);

#endif // #ifndef MOTION_CONTROLLER_H_INLCUDED
#endif // #ifdef IAM_QBOUGE
