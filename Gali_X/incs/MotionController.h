#ifndef MOTION_CONTROLLER_H_INLCUDED
#define MOTION_CONTROLLER_H_INLCUDED

#include "Debug.h"


#define PID_UPDATE_INTERVAL (1.0/10)  // sec

#define ENC_RADIUS          28                      // one enc radius
#define ENC_PERIMETER       (2*M_PI*ENC_RADIUS)     // one enc perimeter
#define TICKS_PER_MM        16.5
#define PULSES_PER_REV      (ENC_PERIMETER*TICKS_PER_MM)

#define TICKS_2PI           (87*2*M_PI * TICKS_PER_MM * 2)  // how many enc ticks after a 2*M_PI turn
#define MM_TO_TICKS(val)    ((val)*TICKS_PER_MM)
#define TICKS_TO_MM(val)    ((val)/TICKS_PER_MM)


// typedef struct  _s_order {
//     absolute / relative
//     x/y
//     dist
//     angle

//     start encl/encr ? auto value - to know when to stop ?

//     precision ?
//     speed ?

// }               s_order;


class MotionController {
public:
    MotionController(void);
    // todo
    // addOrder
    void updateMovement(Debug debug);

// protected:
    Motor motor_l_;
    Motor motor_r_;

    QEI enc_l_;
    QEI enc_r_;

    PID pid_dist_;
    PID pid_angle_;
};

#endif
