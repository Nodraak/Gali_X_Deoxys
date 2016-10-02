
#include "mbed.h"

#include "QEI.h"
#include "PID.h"

#include "Motor.h"
#include "utils.h"
#include "MotionController.h"


MotionController::MotionController(void) :
    motor_l_(PA_8, PA_0, MOTOR_DIR_LEFT_FORWARD),
    motor_r_(PB_10, PA_1, MOTOR_DIR_RIGHT_FORWARD),
    enc_l_(PC_2, PC_3, NC, PULSES_PER_REV, QEI::X4_ENCODING),
    enc_r_(PB_0, PA_4, NC, PULSES_PER_REV, QEI::X4_ENCODING),
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


void MotionController::updateMovement(Debug debug) {
    int enc_l_val = enc_l_.getPulses();
    int enc_r_val = enc_r_.getPulses();

    int input_dist = 0;
    float input_angle = 0;
    float output_dist_pwm = 0;
    float output_angle = 0;
    float mot_l_val = 0;
    float mot_r_val = 0;

    /*
        == pid dist ==
    */

    // get input

    input_dist = (enc_l_val + enc_r_val) / 2;
    pid_dist_.setProcessValue(input_dist - MM_TO_TICKS(1000));

    // get pid output

    output_dist_pwm = pid_dist_.compute();

    /*
        == pid angle ==
    */

    // get input

    // todo apply proper modulo to (enc_l_val - enc_r_val)
    float diff_ticks = enc_r_val - enc_l_val;
    float diff_rad = diff_ticks * 2*M_PI / TICKS_2PI;

    while (diff_rad < -M_PI)
        diff_rad += 2*M_PI;
    while (diff_rad > M_PI)
        diff_rad -= 2*M_PI;

    input_angle = diff_rad;

    // get pid output

    pid_angle_.setProcessValue(input_angle);
    output_angle = pid_angle_.compute();

    /*
        == set val to motor ==
    */

    mot_l_val = output_dist_pwm - output_angle;
    mot_l_val = constrain(mot_l_val, -1, 1);  // todo if one of the two if > 1, divide by the bigest of left/right values
    motor_l_.setSpeed(mot_l_val);

    mot_r_val = output_dist_pwm + output_angle;
    mot_r_val = constrain(mot_r_val, -1, 1);
    motor_r_.setSpeed(mot_r_val);


    debug.printf("[pid dist] in/out %d %.3f\n", input_dist, output_dist_pwm);
    debug.printf("[pid angle] in/out %.3f %.3f\n", diff_rad, output_angle);
    debug.printf("[mot val] %d %.3f | %d %.3f\n",
        motor_l_.getDirection(), motor_l_.getPwm(), motor_r_.getDirection(), motor_r_.getPwm()
    );
    debug.printf("\n");
}
