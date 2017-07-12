#ifdef IAM_QBOUGE

#ifndef QEI_POSITION_LOCATOR_H_INCLUDED
#define QEI_POSITION_LOCATOR_H_INCLUDED

#include "common/utils.h"
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


class QeiPositionLocator {
public:

    QeiPositionLocator(void);

    void reset(bool lock=true);
    void we_are_at(int16_t x, int16_t y, float angle, bool lock=true);

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
    /*
        Timer that generate an interrupt used to call asserv().
        Warning: This create an ISR. This uses the TIMER2 (TIM2_IRQn) interrupt.
    */
    Ticker *asserv_ticker_;

    Qei enc_l_, enc_r_;

public:
    // Current and last value of the encoders. Unit: encoder ticks
    int32_t enc_l_val_, enc_r_val_;
    int32_t enc_l_last_, enc_r_last_;

    // Robot properties
    s_vector_float pos_;    // unit: mm
    float angle_;           // unit: radians
    float speed_;           // unit: mm/sec
    float speed_ang_;       // unit: rad/sec
};

int mc_calcNewPos(
    float diff_l, float diff_r,
    float cur_angle, float cur_x, float cur_y,
    float *new_angle_, float *new_x_, float *new_y_
);

#endif // #ifndef QEI_POSITION_LOCATOR_H_INCLUDED
#endif // #ifdef IAM_QBOUGE
