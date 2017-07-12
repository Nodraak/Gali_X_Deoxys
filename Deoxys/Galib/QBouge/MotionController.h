#ifdef IAM_QBOUGE

#ifndef MOTION_CONTROLLER_H_INLCUDED
#define MOTION_CONTROLLER_H_INLCUDED

/*
    The MotionController have the responsability to control the motors to move
    the robot to the specified coordinates (it execute orders: position, angle,
    delay, ...).
    The MotionController is uturly dumb, all the smart path optimizations are
    made by the MotionPlaner on CQR.
*/

#include "PID.h"

#include "common/Debug.h"
#include "common/Messenger.h"
#include "common/OrdersFIFO.h"
#include "common/utils.h"
#include "QBouge/Motor.h"
#include "QBouge/QeiPositionLocator.h"


// Execute next order conditions

#define MC_TARGET_TOLERANCE_DIST        5.0         // unit: mm     - Gali IX 12 - ESEO 10
#define MC_TARGET_TOLERANCE_SPEED       5.0         // unit: mm/sec              - ESEO  8
#define MC_TARGET_TOLERANCE_ANGLE       DEG2RAD(2)  // unit: rad    - Gali IX  7 - ESEO  2 deg
#define MC_TARGET_TOLERANCE_ANG_SPEED   DEG2RAD(2)  // unit: rad/sec             - ESEO  3 deg

// todo print this
// max error due to angle tolerance (sqrt(100**2+(130+140)**2) * sin(MC_TARGET_TOLERANCE_ANG_SPEED)) == 10

// Max speed

#define PID_DIST_MAX_OUPUT  0.8
#define PID_ANGLE_MAX_OUPUT 0.6

// PID settings

#define PID_DIST_P          6.0
#define PID_DIST_I          0
#define PID_DIST_D          (0.5/1000)

#define PID_ANGLE_P         3.0
#define PID_ANGLE_I         0
#define PID_ANGLE_D         (1.0/1000)

// Obstacle detection

#if 0
#define SENSOR_DIST_STOP 200
// #define SENSOR_DIST_SLOW 400
#define TABLE_MARGIN 50

typedef enum _e_ {
    SHARP_FRONT_LEFT,
    SHARP_FRONT_MIDDLE,
    SHARP_FRONT_RIGHT
};
#endif

class MotionController {
public:
    MotionController(void);

    void reset(bool lock=true);
    void we_are_at(int16_t x, int16_t y, float angle, bool lock=true);
    void update_current_order(s_order_exe *order);

    /*
        Print some information about the inputs, outputs and internal states,
        over UART or CAN.
    */
    void debug_serial(Debug *debug);
    void debug_can(CanMessenger *cm);

    void print(Debug *debug);
    void set(e_cqb_setting cqb_setting, float val);

// semi-private (call from interrupt/Ticker)
private:
    /*
        Control function (asservissement in french).
        This is called via an interrupt every `ASSERV_DELAY` seconds.

        Warning:
            This is executed in an interrupt, it should be fast to execute.
    */
    void asserv(void);

private:
    QeiPositionLocator posLoc_;

    /*
        Timer that generate an interrupt used to call asserv().
        Warning: This create an ISR. This uses the TIMER2 (TIM2_IRQn) interrupt.
    */
    Ticker *asserv_ticker_;

    // IO interfaces
    Motor motor_l_, motor_r_;

    // PID related variables
    PID pid_dist_, pid_angle_;
    float pid_dist_out_, pid_angle_out_;    // unit: between -1 and +1

    Timer timer_;
    float last_order_executed_timestamp_;

public:
    s_order_exe current_order_;
    bool is_current_order_executed_;
};

/*
    Move backward if angle is not in [-90; 90].
*/
void mc_calcDistThetaOrderPos(float *dist_, float *theta_);

/*
    Compute dist and theta which are the error the pid must correct.
    It returns 1 if the current order is executed.
*/
int mc_updateCurOrder(
    s_vector_float cur_pos,  float cur_angle, float cur_speed, float cur_speed_ang,
    s_order_exe *cur_order, float time_since_last_order_finished,
    float *dist_, float *theta_
);

#endif // #ifndef MOTION_CONTROLLER_H_INLCUDED
#endif // #ifdef IAM_QBOUGE
