#ifdef IAM_QBOUGE

#ifndef MOTION_CONTROLLER_H_INLCUDED
#define MOTION_CONTROLLER_H_INLCUDED

/*
We should extract the stuff related to QEI Positionning out of MotionController

QeiPosition
    qei l, r
    pos
    angle
    speed, speed_ang

MotionController
    Motor l, r
    Pid dist, angle
    current order
*/

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
#include "QBouge/Qei.h"


#define ENC_RADIUS          28.2                    // one encoder radius
#define ENC_PERIMETER       (2*M_PI*ENC_RADIUS)     // one encoder perimeter
#define ENC_POS_RADIUS      87                      // distance from one encoder to the center of the robot

#define TICKS_PER_MM_L      16.5
#define TICKS_PER_MM_R      131.8
#define MM_TO_TICKS_L(val)  ((val)*TICKS_PER_MM_L)
#define TICKS_TO_MM_L(val)  ((val)/TICKS_PER_MM_L)
#define MM_TO_TICKS_R(val)  ((val)*TICKS_PER_MM_R)
#define TICKS_TO_MM_R(val)  ((val)/TICKS_PER_MM_R)

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

    void reset(void);
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

// really private
private:
    /*
        Disable and enable the Interrupt TIM2 used by the Ticker to call asserv().
        Theses functions must be used by every function that is called from the
        main thread (non interrupt) and that modify private attributes. Otherwise
        data corruption might happen.
    */
    void lock(void);
    void unlock(void);

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
    */
    void updateCurOrder(void);

    /*
        Compute the PIDs output based on the internal state of the
        MotionController() computed by updateCurOrder() (distance and angle
        errors to correct).
    */
    void computePid(void);

    /*
        Apply the computed PIDs output to the motors.
    */
    void updateMotors(void);

    /*
        Set the goal that the MotionController will make the robot move to.
    */
    void pidDistSetGoal(float goal);
    void pidAngleSetGoal(float goal);

private:
    /*
        Timer that generate an interrupt used to call asserv().
        Warning: This create an ISR. This uses the TIMER2 (TIM2_IRQn) interrupt.
    */
    Ticker *asserv_ticker_;

    // IO interfaces
    Qei enc_l_, enc_r_;
    Motor motor_l_, motor_r_;

    // Last value of the encoders. Used to determine speed. Unit: encoder ticks
    int32_t enc_l_last_, enc_r_last_;

    // PID related variables
    PID pid_dist_, pid_angle_;
    int32_t enc_l_val_, enc_r_val_;         // tmp variable used as a working var
                                            //   use this instead of the raw value from the QEI objects.
                                            //   unit: encoder ticks
    float pid_dist_goal_, pid_angle_goal_;  // units: mm and rad
    float pid_dist_out_, pid_angle_out_;    // unit: between -1 and +1

    Timer timer_;
    float last_order_executed_timestamp_;

public:
    // Robot properties
    s_vector_float pos_;    // unit: mm
    float angle_;           // unit: radians
    float speed_;           // unit: mm/sec
    float speed_ang_;       // unit: rad/sec

    s_order_exe current_order_;
    bool is_current_order_executed_;
};


int mc_calcNewPos(
    float diff_l, float diff_r,
    float cur_angle, float cur_x, float cur_y,
    float *new_angle_, float *new_x_, float *new_y_
);

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
