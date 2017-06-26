#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED

/*
    Read Doc/code_assumptions.md to not fuck everything up by changing things here.
*/

/*
    Debug prints. Comment to disable, uncomment to enable.
*/

// #define PRINT_TIME
#define PRINT_COM_CAN_REC
#define PRINT_COM_CAN_SEND
// #define PRINT_MONITORING_RESET
#define PRINT_SYS_CAN_STATS

/*
    Common config
*/

#define MAIN_LOOP_FPS       200
#define MAIN_LOOP_DELAY     (1.0/MAIN_LOOP_FPS)


#define ORDERS_COUNT        100

/*
    Board-specific config
*/

/*
    datasheet MG90S
    50 Hz
    1 to 2 ms (over 20 ms pulse width)
        => 5% to 10% 5V pwm
        => 7% to 15% 3.3V pwm
*/
#define SERVO_PWM_PERIOD    (1.0/50)

/*
    Actuators config
*/

#define ACT_L_ARM_HEIGHT_R      450
#define ACT_L_ARM_HEIGHT_N      415
#define ACT_L_ARM_HEIGHT_E      380
#define ACT_L_ARM_VERT_R        740
#define ACT_L_ARM_VERT_N        300
#define ACT_L_ARM_VERT_E        160
#define ACT_L_ARM_HORIZ_R       940
#define ACT_L_ARM_HORIZ_N       790
#define ACT_L_ARM_HORIZ_E       640

#define ACT_L_ARM_CLAMP_R       0.05
#define ACT_L_ARM_CLAMP_E       0.12
#define ACT_L_ARM_PUMP_R        true
#define ACT_L_ARM_PUMP_E        false

#define ACT_L_FLAP_R            0.05
#define ACT_L_FLAP_E            0.10
#define ACT_L_PROG_R            0.06
#define ACT_L_PROG_E            0.11


#define ACT_R_ARM_HEIGHT_R      400
#define ACT_R_ARM_HEIGHT_N      400
#define ACT_R_ARM_HEIGHT_E      420
#define ACT_R_ARM_VERT_R        800
#define ACT_R_ARM_VERT_N        300
#define ACT_R_ARM_VERT_E        170
#define ACT_R_ARM_HORIZ_R        20
#define ACT_R_ARM_HORIZ_N       125
#define ACT_R_ARM_HORIZ_E       330

#define ACT_R_ARM_CLAMP_R       0.08
#define ACT_R_ARM_CLAMP_E       0.05
#define ACT_R_ARM_PUMP_R        true
#define ACT_R_ARM_PUMP_E        false

#define ACT_R_FLAP_R            0.10
#define ACT_R_FLAP_E            0.04
#define ACT_R_PROG_R            0.06
#define ACT_R_PROG_E            0.11

/*
    Sensor config
*/

#define SENSOR_ULTRASOUND_ENABLED   // comment to disable, uncomment to enable


#endif // CONFIG_H_INCLUDED
