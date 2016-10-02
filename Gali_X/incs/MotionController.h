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

    void fetchEncodersValue(void);
    void computePid(void);
    void updateMotors(void);
    void debug(Debug *debug);

private:  // I/O
    Motor motor_l_, motor_r_;
    QEI enc_l_, enc_r_;

protected:  // internal
    int enc_l_val_, enc_r_val_;
    float out_pid_dist_;
    float out_pid_angle_;

public:  // interface
    PID pid_dist_;
    PID pid_angle_;
};

#endif
