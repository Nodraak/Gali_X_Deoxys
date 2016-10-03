
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
    pid_dist_.setInputLimits(-100*1000, 100*1000);  // encoders value
    pid_dist_.setOutputLimits(-1.0, 1.0);  // motor speed (~pwm)
    pid_dist_.setMode(AUTO_MODE);  // AUTO_MODE or MANUAL_MODE
    pid_dist_.setBias(0); // magic *side* effect needed for the pid to work, don't comment this

    // PID pid_angle_(6.0, 0, 0, PID_UPDATE_INTERVAL);
    // PID pid_angle_(0.3*angle_Ku, angle_Tu/2, angle_Tu/8, PID_UPDATE_INTERVAL);
    pid_angle_.setInputLimits(-M_PI, M_PI);  // angle. 0 toward, -pi on right, +pi on left
    pid_angle_.setOutputLimits(-1.0, 1.0);  // motor speed (~pwm). -1 right, +1 left, 0 nothing
    pid_angle_.setMode(AUTO_MODE);  // AUTO_MODE or MANUAL_MODE
    pid_angle_.setBias(0); // magic *side* effect needed for the pid to work, don't comment this
}


void MotionController::fetchEncodersValue(void) {
    enc_l_val_ = enc_l_.getPulses();
    enc_r_val_ = enc_r_.getPulses();
}


void MotionController::computePid(void) {
    int input_dist = 0;
    float input_angle = 0;
    float diff_ticks = 0, diff_rad = 0;

    /*
        == pid dist ==
    */

    // get input

    input_dist = (enc_l_val_ + enc_r_val_) / 2;
    pid_dist_.setProcessValue(input_dist - MM_TO_TICKS(1000));

    // get pid output

    out_pid_dist_ = pid_dist_.compute();

    /*
        == pid angle ==
    */

    // get input

    // todo apply proper modulo to (enc_l_val - enc_r_val)
    diff_ticks = enc_r_val_ - enc_l_val_;
    diff_rad = diff_ticks * 2*M_PI / TICKS_2PI;

    while (diff_rad < -M_PI)
        diff_rad += 2*M_PI;
    while (diff_rad > M_PI)
        diff_rad -= 2*M_PI;

    input_angle = diff_rad;

    // get pid output

    pid_angle_.setProcessValue(input_angle);
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
    debug->printf("[MC/in] %d %d\n", enc_l_val_, enc_r_val_);
    debug->printf("[MC/out_pid] %.3f %.3f\n", out_pid_dist_, out_pid_angle_);
    debug->printf("[MC/mot_val] %d %.3f | %d %.3f\n",
        motor_l_.getDirection(), motor_l_.getPwm(), motor_r_.getDirection(), motor_r_.getPwm()
    );
    debug->printf("\n");
}
