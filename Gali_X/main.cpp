
#include "mbed.h"
#include "rtos.h"

#include "QEI.h"
#include "PID.h"

#include "Motor.h"
#include "utils.h"
#include "Debug.h"


int main(void)
{
    // init com (serial, wifi, xbee, ...)

    Debug debug(USBTX, USBRX, PC_10, PC_11);
    debug.printf("Hello world ! (all)\n");

    // init motors

    Motor motor_left(PA_8, PA_0, MOTOR_DIR_LEFT_FORWARD);
    Motor motor_right(PB_10, PA_1, MOTOR_DIR_RIGHT_FORWARD);

    // todo motor TH

    // init qei

    QEI enc_l(PC_2, PC_3, NC, PULSES_PER_REV, QEI::X4_ENCODING);
    QEI enc_r(PB_0, PA_4, NC, PULSES_PER_REV, QEI::X4_ENCODING);

    // pid

    PID ctrl_dist(3.0, 0.0, 0.0, PID_UPDATE_INTERVAL);
    ctrl_dist.setInputLimits(-100*1000, 100*1000);  // encoders value
    ctrl_dist.setOutputLimits(-1.0, 1.0);  // motor speed (~pwm)
    ctrl_dist.setMode(AUTO_MODE);  // AUTO_MODE or MANUAL_MODE
    ctrl_dist.setBias(0); // magic *side* effect needed for the pid to work, don't comment this

    PID ctrl_angle(1.0, 0.0, 0.0, PID_UPDATE_INTERVAL);
    ctrl_angle.setInputLimits(-M_PI, M_PI);  // angle. 0 toward, -pi on right, +pi on left
    ctrl_angle.setOutputLimits(-1.0, 1.0);  // motor speed (~pwm). -1 right, +1 left, 0 nothing
    ctrl_angle.setMode(AUTO_MODE);  // AUTO_MODE or MANUAL_MODE
    ctrl_angle.setBias(0); // magic *side* effect needed for the pid to work, don't comment this

    // init sharp + other sensors
    // init servos + other actuators

    // init timers ?
    // init ia ?

    // init tirette interrupt


    // float robot_angle = 0;

    ctrl_dist.setSetPoint(MM_TO_TICKS(2000));  // goal
    ctrl_angle.setSetPoint(0);  // goal

    debug.printf("pid dist goal %d\n", MM_TO_TICKS(1000));


    while (true)
    {
/*
        pc.printf("readable %d\n", pc.readable());
        if (pc.readable())
        {
            pc.printf("Read: %s\n", pc.getc());

        }
*/

        // update qei
        // update sharp + other sensors

        // do com (serial, ...)

        // update ia

        // debug

        // sleep

        int enc_l_val = enc_l.getPulses();
        int enc_r_val = enc_r.getPulses();

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
        ctrl_dist.setProcessValue(input_dist);

        // get pid output

        output_dist_pwm = ctrl_dist.compute();

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

        ctrl_angle.setProcessValue(input_angle);
        output_angle = ctrl_angle.compute();

        /*
            == set val to motor ==
        */

        mot_l_val = output_dist_pwm - output_angle;
        mot_l_val = constrain(mot_l_val, -1, 1);  // divide by the bigest of left/right values
        motor_left.setSpeed(mot_l_val);

        mot_r_val = output_dist_pwm + output_angle;
        mot_r_val = constrain(mot_r_val, -1, 1);
        motor_right.setSpeed(mot_r_val);

        /*
            debug
        */

        debug.printf("[pid dist] in/out %d %.3f\n", input_dist, output_dist_pwm);
        debug.printf("[pid angle] in/out %.3f %.3f\n", diff_rad, output_angle);
        debug.printf("[mot val] %d %.3f | %d %.3f\n",
            motor_left.getDirection(), motor_left.getPwm(), motor_right.getDirection(), motor_right.getPwm()
        );
        debug.printf("\n");

        Thread::wait(PID_UPDATE_INTERVAL*1000);
    }

    // do some cleanup ?

    return 0;
}
