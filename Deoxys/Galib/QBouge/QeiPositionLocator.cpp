#ifdef IAM_QBOUGE

#include "common/Monitoring.h"
#include "common/utils.h"
#include "common/sys.h"
#include "QBouge/Qei.h"

#include "config.h"
#include "pinout.h"

#include "QBouge/QeiPositionLocator.h"


QeiPositionLocator::QeiPositionLocator(void) :
    enc_l_(ENC_L_DATA1, ENC_L_DATA2),
    enc_r_(ENC_R_DATA1, ENC_R_DATA2)
{
    this->reset();

    asserv_ticker_ = new Ticker;
    asserv_ticker_->attach(callback(this, &QeiPositionLocator::asserv), ASSERV_DELAY_PL);
}


void QeiPositionLocator::we_are_at(int16_t x, int16_t y, float angle, bool lock) {
    if (lock)
        ticker_lock();

    pos_.x = x;
    pos_.y = y;
    angle_ = angle;

    if (lock)
        ticker_unlock();
}


void QeiPositionLocator::reset(bool lock) {
    if (lock)
        ticker_lock();

    enc_l_val_ = 0;
    enc_r_val_ = 0;
    enc_l_last_ = 0;
    enc_r_last_ = 0;

    this->we_are_at(0, 0, 0, false);
    speed_ = 0;
    speed_ang_ = 0;

    if (lock)
        ticker_unlock();
}


/*
Warning:
    This is executed in an interrupt, it should be fast to execute (it must be
    shorter than ASSERV_DELAY, otherwise overlapping will append). Avoid any
    printf, malloc, while loop, etc.
*/
void QeiPositionLocator::asserv(void) {
    float diff_l = 0, diff_r = 0;  // unit: mm
    float last_angle = 0;

    if (g_mon != NULL)
        g_mon->asserv_pl.start_new();

    /*
        Save the encoders value to a working variable so that the various
        computations and the debug are based on the same values within this
        loop.
    */

    enc_l_last_ = enc_l_val_;
    enc_r_last_ = enc_r_val_;

    enc_l_val_ = enc_l_.getPulses();
    enc_r_val_ = enc_r_.getPulses();

    /*
        Update the internal state of the MotionController (position, speed, ...)
        given the value of the encoders ticks fetched by fetchEncodersValue().
    */

    diff_l = TICKS_TO_MM_L(enc_l_val_-enc_l_last_);
    diff_r = TICKS_TO_MM_R(enc_r_val_-enc_r_last_);

    last_angle = angle_;

    mc_calcNewPos(
        diff_l, diff_r,
        angle_, pos_.x, pos_.y,
        &angle_, &pos_.x, &pos_.y
    );

    speed_ = (diff_l + diff_r) / 2.0 / ASSERV_DELAY_PL;
    speed_ang_ = (angle_ - last_angle) / ASSERV_DELAY_PL;  // this is not an angle, therefore no std_rad_angle()

    if (g_mon != NULL)
        g_mon->asserv_pl.stop_and_save();
}


/*
    diff_l and diff_r: enc distance in mm, + forward, - backward
    cur_angle: current angle. in radian. facing east is 0. positive is CCW. (std maths)
    cur_x and cur_y: current position. in mm. 0 is bottom left.

    Return 0 on success.
*/
int mc_calcNewPos(
    float diff_l, float diff_r,
    float cur_angle, float cur_x, float cur_y,
    float *new_angle_, float *new_x_, float *new_y_
) {
    float distance = 0, radius = 0;
    float dangle = 0, dx = 0, dy = 0;
    float new_angle = 0;
    float new_x = 0, new_y = 0;

    if (diff_l == diff_r)
    {
        distance = diff_l;

        dangle = 0;
        new_angle = cur_angle;

        dx = distance * cos(cur_angle);
        dy = distance * sin(cur_angle);
    }
    else if (diff_l == -diff_r)
    {
        distance = 0;

        dangle = 1.0 * diff_r / ENC_POS_RADIUS;
        new_angle = std_rad_angle(cur_angle + dangle);

        dx = 0;
        dy = 0;
    }
    else
    {
        radius = 1.0 * ENC_POS_RADIUS * (diff_r + diff_l) / (diff_r - diff_l);

        // angle
        dangle = (diff_r - diff_l) / (2 * ENC_POS_RADIUS);
        new_angle = std_rad_angle(cur_angle + dangle);

        // pos
        dx = radius * (sin(new_angle)-sin(cur_angle));
        dy = - radius * (cos(new_angle)-cos(cur_angle));
    }

    new_x = cur_x + dx;
    new_y = cur_y + dy;

    // update class members
    *new_angle_ = new_angle;
    *new_x_ = new_x;
    *new_y_ = new_y;

    return 0;
}

#endif // #ifdef IAM_QBOUGE
