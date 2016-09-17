
#include "mbed.h"
#include "rtos.h"

#include "QEI.h"
#include "PID.h"

#include "motor.h"

#define PID_UPDATE_INTERVAL (1.0/10)  // sec

#define ENC_RADIUS          28                      // one enc radius
#define ENC_PERIMETER       (2*M_PI*ENC_RADIUS)     // one enc perimeter
#define TICKS_PER_MM        16.5
#define PULSES_PER_REV      (ENC_PERIMETER*TICKS_PER_MM)
#define ENC_POS_RADIUS      87                      // distance from one enc to the center of the robot
#define TICKS_2PI           (87*2*M_PI * TICKS_PER_MM * 2)  // how many enc ticks after a 2*M_PI turn

#define MM_TO_TICKS(val)    ((val)*TICKS_PER_MM)
#define TICKS_TO_MM(val)    ((val)/TICKS_PER_MM)

#define PWM_MIN 0.07  // pwm value at which the robot start moving

#define MOTOR_DIR_LEFT_FORWARD 0
#define MOTOR_DIR_LEFT_BACKWARD 1

#define MOTOR_DIR_RIGHT_FORWARD 1
#define MOTOR_DIR_RIGHT_BACKWARD 0


float abs(float i)
{
    return (i < 0) ? (-i) : i;
}

/*
    Re-maps a number from one range to another.
    Notes:
        Does not constrain values to within the range
        The "lower bounds" of either range may be larger or smaller than the "upper bounds"

    Cf. https://www.arduino.cc/en/Reference/Map
*/
double map(double x, double in_min, double in_max, double out_min, double out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

double constrain(double val, double min, double max)
{
    if (val < min)
        return min;
    else if (val > max)
        return max;
    else
        return val;
}

int main(void)
{
    // init com (serial, wifi, xbee, ...)

    Serial pc(USBTX, USBRX);
    pc.baud(115200);
    // pc.format(8, Serial::None, 1);  // usefull?
    pc.printf("Hello, world! pc\n");

    Serial xbee(PC_10, PC_11);
    xbee.baud(115200);
    // xbee.format(8, Serial::None, 1);  // usefull?
    xbee.printf("Hello, world! xbee\n");

    // init motors

    PwmOut mot_l(PA_8);  // D7 == PA_8
    mot_l.period(0.001 * 0.1);      // 0.0001 == 10K Hz
    PwmOut mot_r(PB_10);  // D6 == PB_10
    mot_r.period(0.001 * 0.1);      // 0.0001 == 10K Hz

    DigitalOut mot_l_dir(PA_1);
    mot_l_dir = MOTOR_DIR_LEFT_FORWARD;
    DigitalOut mot_r_dir(PA_0);
    mot_r_dir = MOTOR_DIR_RIGHT_FORWARD;

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

    ctrl_dist.setSetPoint(MM_TO_TICKS(1000));  // goal
    ctrl_angle.setSetPoint(0);  // goal

    xbee.printf("pid dist goal %d\n", MM_TO_TICKS(1000));


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
        float mot_l_pwm = 0;
        float mot_r_pwm = 0;

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

        // signed val
        mot_l_val = output_dist_pwm - output_angle;
        mot_l_val = constrain(mot_l_val, -1, 1);
        mot_l_pwm = abs(mot_l_val);

        mot_r_val = output_dist_pwm + output_angle;
        mot_r_val = constrain(mot_r_val, -1, 1);
        mot_r_pwm = abs(mot_r_val);

        // set dir  -- WTF ??? todo
        mot_r_dir = (mot_l_val < 0) ? MOTOR_DIR_LEFT_BACKWARD : MOTOR_DIR_LEFT_FORWARD;
        mot_l_dir = (mot_r_val < 0) ? MOTOR_DIR_RIGHT_BACKWARD : MOTOR_DIR_RIGHT_FORWARD;

        // set motor

        if (mot_l_pwm < 0.05)   mot_l = 0;
        else                    mot_l = map(mot_l_pwm, 0, 1, PWM_MIN, 1);

        if (mot_r_pwm < 0.05)   mot_r = 0;
        else                    mot_r = map(mot_r_pwm, 0, 1, PWM_MIN, 1);

        // debug

        xbee.printf("[pid dist] in/out %d %.3f\n", input_dist, output_dist_pwm);
        xbee.printf("[pid angle] in/out %.3f %.3f\n", diff_rad, output_angle);
        xbee.printf("[mot val] %.3f (%.3f) %.3f (%.3f)\n", mot_l_val, mot_l.read(), mot_r_val, mot_r.read());
        xbee.printf("\n");

        pc.printf("[pid dist] in/out %d %.3f\n", input_dist, output_dist_pwm);
        pc.printf("[pid angle] in/out %.3f %.3f\n", diff_rad, output_angle);
        pc.printf("[mot val] %.3f (%.3f) %.3f (%.3f)\n", mot_l_val, mot_l.read(), mot_r_val, mot_r.read());
        pc.printf("\n");

        Thread::wait(PID_UPDATE_INTERVAL*1000);
    }

    // do some cleanup ?

    return 0;
}
