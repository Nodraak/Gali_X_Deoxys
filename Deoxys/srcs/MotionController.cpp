
#include "mbed.h"

#include "PID.h"
#include "QEI.h"

#include "Motor.h"
#include "pinout.h"
#include "utils.h"

#include "MotionController.h"


MotionController::MotionController(void) :
    motor_l_(MOTOR_L_PWM, MOTOR_L_DIR, MOTOR_DIR_LEFT_FORWARD),
    motor_r_(MOTOR_R_PWM, MOTOR_R_DIR, MOTOR_DIR_RIGHT_FORWARD),
    enc_l_(ENC_L_DATA1, ENC_L_DATA2, NC, PULSES_PER_REV, QEI::X4_ENCODING),
    enc_r_(ENC_R_DATA1, ENC_R_DATA2, NC, PULSES_PER_REV, QEI::X4_ENCODING),
    pid_dist_(3.0, 0.0, 0.0, PID_UPDATE_INTERVAL),
    pid_angle_(3.0, 0.0, 0.0, PID_UPDATE_INTERVAL)
{
    pid_dist_.setInputLimits(-5*1000, 5*1000);  // encoders value
    pid_dist_.setOutputLimits(-1.0, 1.0);  // motor speed (~pwm)
    pid_dist_.setMode(AUTO_MODE);  // AUTO_MODE or MANUAL_MODE
    pid_dist_.setBias(0); // magic *side* effect needed for the pid to work, don't comment this

    pid_angle_.setInputLimits(-M_PI, M_PI);  // angle. 0 toward, -pi on right, +pi on left
    pid_angle_.setOutputLimits(-1.0, 1.0);  // motor speed (~pwm). -1 right, +1 left, 0 nothing
    pid_angle_.setMode(AUTO_MODE);  // AUTO_MODE or MANUAL_MODE
    pid_angle_.setBias(0); // magic *side* effect needed for the pid to work, don't comment this
}


void MotionController::fetchEncodersValue(void) {
    enc_l_val_ = enc_l_.getPulses();
    enc_r_val_ = enc_r_.getPulses();
}



/*
    diff_l and diff_r: enc distance in mm, + forward, - backward
    cur_angle: current angle. in radian. facing east is 0. positive is CCW. (std maths)
    cur_x and cur_y: current position. in mm. 0 is bottom left.

    Return 0 on success.
*/
int calcNewPos(
    int diff_l, int diff_r,
    float cur_angle, int cur_x, int cur_y,
    float *new_angle_, int16_t *new_x_, int16_t *new_y_
) {
    float distance = 0, radius = 0;
    float dangle = 0, dx = 0, dy = 0;
    float new_angle = 0;
    int new_x = 0, new_y = 0;

    if (diff_l == diff_r)
    {
        distance = diff_l;

        dangle = 0;

        dx = distance * cos(cur_angle);
        dy = distance * sin(cur_angle);
    }
    else
    {
        distance = (diff_l + diff_r) / 2;
        radius = 1.0 * ENC_POS_RADIUS * (diff_r + diff_l) / (diff_r - diff_l);

        // angle
        dangle = distance / radius;
        new_angle = cur_angle + dangle;

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


void MotionController::updatePositionAndOrder(void) {

    if (
        calcNewPos(
            TICKS_TO_MM(enc_l_val_-enc_l_last_), TICKS_TO_MM(enc_r_val_-enc_r_last_),
            angle_, pos_.x, pos_.y,
            &angle_, &pos_.x, &pos_.y
        ) != 0
    )
    {
        printf("MotionController::updatePositionAndOrder - FUCK\n");
    }
}


void MotionController::computePid(void) {
    int input_dist = 0;
    float input_angle = 0;
    int diff_ticks = 0;
    float diff_rad = 0;

    /*
        == pid dist ==
    */

    // get input

    input_dist = (enc_l_val_ + enc_r_val_) / 2;
    pid_dist_.setProcessValue(input_dist-pid_dist_goal_);

    // get pid output

    out_pid_dist_ = pid_dist_.compute();

    /*
        == pid angle ==
    */

    // get input

    diff_ticks = enc_r_val_ - enc_l_val_;
    diff_rad = diff_ticks * 2*M_PI / TICKS_2PI;
    input_angle = diff_rad;

    // get pid output

    pid_angle_.setProcessValue(std_rad_angle(input_angle-pid_angle_goal_));
    out_pid_angle_ = pid_angle_.compute();
}


void MotionController::updateMotors(void) {
    float mot_l_val = 0, mot_r_val = 0;

    mot_l_val = out_pid_dist_ - out_pid_angle_;
    mot_l_val = constrain(mot_l_val, -1, 1);  // todo if one of the two if > 1, divide by the bigest of left/right values
    motor_l_.setSpeed(mot_l_val);

    mot_r_val = out_pid_dist_ + out_pid_angle_;
    mot_r_val = constrain(mot_r_val, -1, 1);
    motor_r_.setSpeed(mot_r_val);
}


void MotionController::debug(Debug *debug) {
    debug->printf("[MC/in] %lld %lld\n", enc_l_val_, enc_r_val_);
    debug->printf("[MC/out_pid] (dist angle) %.3f %.3f\n", out_pid_dist_, out_pid_angle_);
    debug->printf("[MC/mot_val] (dir pwm) %d %.3f | %d %.3f\n",
        motor_l_.getDirection(), motor_l_.getPwm(), motor_r_.getDirection(), motor_r_.getPwm()
    );
}

void MotionController::pidDistSetGoal(float goal) {
    pid_dist_goal_ = goal;
}

void MotionController::pidAngleSetGoal(float goal) {
    pid_angle_goal_ = goal;
}
