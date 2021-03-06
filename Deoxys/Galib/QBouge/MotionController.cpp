#ifdef IAM_QBOUGE

#include "mbed.h"
#include <cstring>  // memcpy

#include "PID.h"

#include "common/Messenger.h"
#include "common/Monitoring.h"
#include "common/OrdersFIFO.h"
#include "common/utils.h"
#include "QBouge/Motor.h"
#include "QBouge/Qei.h"

#include "config.h"
#include "pinout.h"

#include "MotionController.h"


MotionController::MotionController(void) :
    enc_l_(ENC_L_DATA1, ENC_L_DATA2),
    enc_r_(ENC_R_DATA1, ENC_R_DATA2),
    motor_l_(MOTOR_L_PWM, MOTOR_L_DIR, MOTOR_DIR_LEFT_FORWARD),
    motor_r_(MOTOR_R_PWM, MOTOR_R_DIR, MOTOR_DIR_RIGHT_FORWARD),
    pid_dist_(PID_DIST_P, PID_DIST_I, PID_DIST_D, ASSERV_DELAY),
    pid_angle_(PID_ANGLE_P, PID_ANGLE_I, PID_ANGLE_D, ASSERV_DELAY)
{
    pid_dist_.setInputLimits(-3*1000, 3*1000);  // dist (mm)
    pid_dist_.setOutputLimits(-PID_DIST_MAX_OUPUT, PID_DIST_MAX_OUPUT);  // motor speed (~pwm)
    pid_dist_.setMode(AUTO_MODE);  // AUTO_MODE or MANUAL_MODE
    pid_dist_.setBias(0); // magic *side* effect needed for the pid to work, don't comment this
    pid_dist_.setSetPoint(0);
    this->pidDistSetGoal(0);  // pid's error

    pid_angle_.setInputLimits(-M_PI, M_PI);  // angle (rad). 0 toward, -pi on right, +pi on left
    pid_angle_.setOutputLimits(-PID_ANGLE_MAX_OUPUT, PID_ANGLE_MAX_OUPUT);  // motor speed (~pwm). -1 right, +1 left, 0 nothing
    pid_angle_.setMode(AUTO_MODE);  // AUTO_MODE or MANUAL_MODE
    pid_angle_.setBias(0); // magic *side* effect needed for the pid to work, don't comment this
    pid_angle_.setSetPoint(0);
    this->pidAngleSetGoal(0);  // pid's error

    timer_.start();
    last_order_executed_timestamp_ = 0;

    current_order_.type = ORDER_EXE_TYPE_NONE;
    current_order_.delay = 0;
    current_order_.pos.x = 0;
    current_order_.pos.y = 0;
    current_order_.angle = 0;
    is_current_order_executed_ = false;

    this->reset();

    asserv_ticker_ = new Ticker;
    asserv_ticker_->attach(callback(this, &MotionController::asserv), ASSERV_DELAY);
}


void MotionController::we_are_at(int16_t x, int16_t y, float angle, bool lock) {
    if (lock)
        this->lock();

    pos_.x = x;
    pos_.y = y;
    angle_ = angle;

    current_order_.pos.x = x;
    current_order_.pos.y = y;
    current_order_.angle = angle;

    if (lock)
        this->unlock();
}


void MotionController::reset(void) {
    this->lock();

    enc_l_last_ = 0;
    enc_r_last_ = 0;

    enc_l_val_ = 0;
    enc_r_val_ = 0;
    pid_dist_goal_ = 0;
    pid_angle_goal_ = 0;
    pid_dist_out_ = 0;
    pid_angle_out_ = 0;

    this->we_are_at(0, 0, 0, false);
    speed_ = 0;
    speed_ang_ = 0;

    this->unlock();
}


void MotionController::update_current_order(s_order_exe *order) {
    this->lock();
    memcpy(&this->current_order_, order, sizeof(s_order_exe));
    this->unlock();
}


/*
Warning:
    This is executed in an interrupt, it should be fast to execute (it must be
    shorter than ASSERV_DELAY, otherwise overlapping will append). Avoid any
    printf, malloc, while loop, etc.
*/
void MotionController::asserv(void) {
    if (g_mon != NULL)
        g_mon->asserv.start_new();

    // Input

    this->fetchEncodersValue();

    // Compute

    this->updatePosition();
    this->updateCurOrder();
    this->computePid();

    // Output

    this->updateMotors();

    if (g_mon != NULL)
        g_mon->asserv.stop_and_save();
}


void MotionController::lock(void) {
    NVIC_DisableIRQ(TIM2_IRQn);
}


void MotionController::unlock(void) {
    NVIC_EnableIRQ(TIM2_IRQn);
}


void MotionController::fetchEncodersValue(void) {
    enc_l_last_ = enc_l_val_;
    enc_r_last_ = enc_r_val_;

    enc_l_val_ = enc_l_.getPulses();
    enc_r_val_ = enc_r_.getPulses();
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


void MotionController::updatePosition(void) {
    float diff_l = 0, diff_r = 0;  // unit: mm
    float last_angle = 0;

    diff_l = TICKS_TO_MM_L(enc_l_val_-enc_l_last_);
    diff_r = TICKS_TO_MM_R(enc_r_val_-enc_r_last_);

    motor_l_.updateSpeed(diff_l);
    motor_r_.updateSpeed(diff_r);

    speed_ = (motor_l_.getSpeed() + motor_r_.getSpeed()) / 2.0;

    last_angle = angle_;

    mc_calcNewPos(
        diff_l, diff_r,
        angle_, pos_.x, pos_.y,
        &angle_, &pos_.x, &pos_.y
    );

    speed_ang_ = (angle_ - last_angle) / ASSERV_DELAY;  // this is not an angle, therefore no std_rad_angle()
}


void mc_calcDistThetaOrderPos(float *dist_, float *theta_) {
    float dist = 0, theta = 0;

    dist = *dist_;
    theta = *theta_;

    if ((theta < -M_PI/2) || (M_PI/2 < theta))
    {
        dist = -dist;
        theta = std_rad_angle(theta+M_PI);
    }

    *dist_ = dist;
    *theta_ = theta;
}


int mc_updateCurOrder(
    s_vector_float cur_pos,  float cur_angle, float cur_speed, float cur_speed_ang,
    s_order_exe *cur_order, float time_since_last_order_finished,
    float *dist_, float *theta_
) {
    float dx = 0, dy = 0;
    float dist = 0, theta = 0;  // units: mm, rad
    int ret = 0;

    // update the goals in function of the given order

    switch (cur_order->type)
    {
        case ORDER_EXE_TYPE_NONE:
            // nothing to do
            ret = 1;
            break;

        case ORDER_EXE_TYPE_WAIT_CQR_FINISHED:
            // do nothing. Just wait without holding pos nor angle.
            break;

        case ORDER_EXE_TYPE_DELAY:
        case ORDER_EXE_TYPE_WAIT_CQB_FINISHED:
        case ORDER_EXE_TYPE_WAIT_CQES_FINISHED:
            // hold position and angle
            dx = cur_order->pos.x - cur_pos.x;
            dy = cur_order->pos.y - cur_pos.y;
            theta = std_rad_angle(cur_order->angle - cur_angle);
            dist = DIST(dx, dy) * cos(atan2(dy, dx) - cur_angle);

            if ((cur_order->type == ORDER_EXE_TYPE_DELAY) && (cur_order->delay != 0))  // todo real delay vs holdPos vs None orders)
            {
                if (time_since_last_order_finished > cur_order->delay)
                    ret = 1;
            }
            break;

        case ORDER_EXE_TYPE_MOV_POS:
            // position order

            dx = cur_order->pos.x - cur_pos.x;
            dy = cur_order->pos.y - cur_pos.y;

            dist = DIST(dx, dy);
            theta = std_rad_angle(atan2(dy, dx) - cur_angle);
            mc_calcDistThetaOrderPos(&dist, &theta);

            if ((ABS(dist) < MC_TARGET_TOLERANCE_DIST) && (ABS(cur_speed) < MC_TARGET_TOLERANCE_SPEED))
            {
                cur_order->angle = cur_angle;  // save current angle if we need to hold it
                ret = 1;
            }

            break;

        case ORDER_EXE_TYPE_MOV_ANGLE:
            // angle order while holding position

            dx = cur_order->pos.x - cur_pos.x;
            dy = cur_order->pos.y - cur_pos.y;

            theta = std_rad_angle(cur_order->angle - cur_angle);
            dist = DIST(dx, dy) * cos(atan2(dy, dx) - cur_angle);

            if ((ABS(theta) < MC_TARGET_TOLERANCE_ANGLE) && (ABS(cur_speed_ang) < MC_TARGET_TOLERANCE_ANG_SPEED))
                ret = 1;

            break;

        case ORDER_EXE_TYPE_ACTUATOR:
        case ORDER_EXE_TYPE_COLOR_WAIT:
            // ignore on CQB
            ret = 1;
            break;

        case ORDER_EXE_TYPE_LAST:
            // nothing to do
            ret = 1;
            break;
    }

    *dist_ = dist;
    *theta_ = theta;

    return ret;
}


void MotionController::updateCurOrder(void) {
    float dist = 0, theta = 0;  // units: mm, rad

    is_current_order_executed_ = mc_updateCurOrder(
        pos_, angle_, speed_, speed_ang_,
        &current_order_, timer_.read()-last_order_executed_timestamp_,
        &dist, &theta
    );

    this->pidDistSetGoal(dist);
    this->pidAngleSetGoal(theta);
}


void MotionController::computePid(void) {
    if (is_current_order_executed_)
    {
        pid_dist_out_ = 0;
        pid_angle_out_ = 0;
    }
    else
    {
        pid_dist_.setProcessValue(-pid_dist_goal_);
        pid_dist_out_ = pid_dist_.compute();

        pid_angle_.setProcessValue(-pid_angle_goal_);
        pid_angle_out_ = pid_angle_.compute();
    }
}


void MotionController::updateMotors(void) {
    float mot_l_val = 0, mot_r_val = 0, m = 0;

    if (current_order_.type != ORDER_EXE_TYPE_NONE)
    {
        mot_l_val = pid_dist_out_ - pid_angle_out_;
        mot_r_val = pid_dist_out_ + pid_angle_out_;

        // if the magnitude of one of the two is > 1, divide by the bigest of these
        // two two magnitudes (in order to keep the scale)
        if ((ABS(mot_l_val) > 1) || (ABS(mot_r_val) > 1))
        {
            m = MAX(ABS(mot_l_val), ABS(mot_r_val));
            mot_l_val /= m;
            mot_r_val /= m;
        }
    }

    motor_l_.setSPwm(mot_l_val);
    motor_r_.setSPwm(mot_r_val);
}


void MotionController::debug_serial(Debug *debug) {
    // debug->printf("[MC/i] (ticks l r) %d %d\n", enc_l_val_, enc_r_val_);
    debug->printf("[MC/i] (mm l r) %.2f %.2f\n", TICKS_TO_MM_L(enc_l_val_), TICKS_TO_MM_R(enc_r_val_));
    debug->printf("[MC/t_pid] (dist angle) %.3f %.3f\n", pid_dist_out_, pid_angle_out_);
    // debug->printf("[MC/o_mot] (pwm) %.3f %.3f\n", motor_l_.getSPwm(), motor_r_.getSPwm());
    debug->printf(
        "[MC/o_robot] (pos angle speed ang_speed) %.0f %.0f %d %.0f %d\n",
        pos_.x, pos_.y, (int)RAD2DEG(angle_), speed_, (int)RAD2DEG(speed_ang_)
    );

    debug->printf(
        "[MC/order] %s %d %d %d %.3f\n", e2s_order_exe_type[current_order_.type],
        current_order_.pos.x, current_order_.pos.y, (int)RAD2DEG(current_order_.angle), current_order_.delay
    );
}

void MotionController::debug_can(CanMessenger *cm) {
// todo: can fifo ? yes, but still take advantage of built in hw can fifo (of size 3 IIRC)

    cm->send_msg_CQB_MC_encs(enc_l_val_, enc_r_val_);
    // cm->send_msg_CQB_MC_pids(pid_dist_out_, pid_angle_out_);
    // cm->send_msg_CQB_MC_motors(motor_l_.getSPwm(), motor_r_.getSPwm());
    cm->send_msg_CQB_MC_pos_angle(pos_.x, pos_.y, angle_);
    // cm->send_msg_CQB_MC_speeds(speed_, speed_ang_);
}

void MotionController::print(Debug *debug) {
    debug->printf("MotionController:\n");
    debug->printf("\tdist\n");
    debug->printf("\t\tp %.3f\n", pid_dist_.getPParam());
    debug->printf("\t\ti %.3f\n", pid_dist_.getIParam());
    debug->printf("\t\td %.3f\n", pid_dist_.getDParam());
    debug->printf("\tangle\n");
    debug->printf("\t\tp %.3f\n", pid_angle_.getPParam());
    debug->printf("\t\ti %.3f\n", pid_angle_.getIParam());
    debug->printf("\t\td %.3f\n", pid_angle_.getDParam());
}

void MotionController::set(e_cqb_setting cqb_setting, float val) {
    switch (cqb_setting) {
        case CQB_SETTING_PID_DIST_P:
            pid_dist_.setTunings(val, pid_dist_.getIParam(), pid_dist_.getDParam());
            pid_dist_.reset();
            break;

        case CQB_SETTING_PID_DIST_I:
            pid_dist_.setTunings(pid_dist_.getPParam(), val, pid_dist_.getDParam());
            pid_dist_.reset();
            break;

        case CQB_SETTING_PID_DIST_D:
            pid_dist_.setTunings(pid_dist_.getPParam(), pid_dist_.getIParam(), val);
            pid_dist_.reset();
            break;

        case CQB_SETTING_PID_ANGLE_P:
            pid_angle_.setTunings(val, pid_angle_.getIParam(), pid_angle_.getDParam());
            pid_angle_.reset();
            break;

        case CQB_SETTING_PID_ANGLE_I:
            pid_angle_.setTunings(pid_angle_.getPParam(), val, pid_angle_.getDParam());
            pid_angle_.reset();
            break;

        case CQB_SETTING_PID_ANGLE_D:
            pid_angle_.setTunings(pid_angle_.getPParam(), pid_angle_.getIParam(), val);
            pid_angle_.reset();
            break;
    }
}

void MotionController::pidDistSetGoal(float goal) {
    pid_dist_goal_ = goal;
}

void MotionController::pidAngleSetGoal(float goal) {
    pid_angle_goal_ = goal;
}

#if 0
void MotionController::sensor_handle(enum what, int16_t dist) {
    int sensor_dx = 0, sensor_dy = 0;
    int sensor_x = 0, sensor_y = 0;

    if (dist == -1)
        return;

    switch (what)
    {
        // todo define ?
        case front_left:
            sensor_dx = -200;
            sensor_dy = 130;
        case front_midlle:
            sensor_dx = 0;
            sensor_dy = 130;
        case front_right:
            sensor_dx = 200;
            sensor_dy = 130;
        case back_left:
            sensor_dx = -200;
            sensor_dy = -130;
        case back_right:
            sensor_dx = 200;
            sensor_dy = -130;
    }

    sensor_x = cur_x_ + (sin(cur_angle_)*sensor_dx + cos(cur_angle_)*sensor_dy) + cos(cur_angle_)*dist;
    sensor_y = cur_y_ + (-cos(cur_angle_)*sensor_dx + sin(cur_angle_)*sensor_dy) + sin(cur_angle_)*dist;

    if (!(
        (sensor_x >= 0+TABLE_MARGIN)
        && (sensor_x < 2000-TABLE_MARGIN)
        && (sensor_y >= 0+TABLE_MARGIN)
        && (sensor_y < 3000-TABLE_MARGIN)
    ))
        return;

    if (dist < SENSOR_DIST_STOP)
        sensor_stop = timer_.read();
    // else if (dist < SENSOR_DIST_SLOW)
    //     sensor_slow = timer_.read();
}
#endif

#endif // #ifdef IAM_QBOUGE
