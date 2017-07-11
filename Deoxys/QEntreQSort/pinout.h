#ifndef PINOUT_H_INCLUDED
#define PINOUT_H_INCLUDED

#define CAN_TX              PA_12
#define CAN_RX              PA_11

#define LED_RUNNING         NC
#define LED_PONG_CQB        NC
#define LED_PONG_CQR        NC
#define LED_PONG_CQES       NC
#define LED_BUS_OFF         NC


#define AX12_PIN_SWITCH     PA_10   // D2
#define AX12_PIN_TX         PC_10
#define AX12_PIN_RX         PC_11

#define ACT_L_PUMP          PC_6    // digi
#define ACT_L_CLAMP         PB_6    // D10 pwm
#define ACT_L_FLAP          PC_7    // D9 pwm
#define ACT_L_PROG          PA_9    // D8 pwm

#define ACT_R_PUMP          PC_8    // digi
#define ACT_R_CLAMP         PC_9    // pwm
#define ACT_R_FLAP          PB_8    // D15 pwm
#define ACT_R_PROG          PB_9    // D14 pwm

// #define SWITCH_1            PC_5    // digi
// #define SWITCH_2            PB_12   // digi
// #define SWITCH_3            PB_11   // digi
// #define SWITCH_4            PB_2    // digi

// #define SRF1_T              PA_13
// #define SRF1_E              PH_0
// #define SRF2_T              PA_15
// #define SRF2_E              PH_1
// #define SRF3_T              PC_13
// #define SRF3_E              PC_2
// #define SRF4_T              PC_12
// #define SRF4_E              PC_3

// #define SHARP1              PA_0  // analog in
// #define SHARP2              PA_1  // analog in
// #define SHARP3              PA_4  // analog in
// #define SHARP4              PB_0  // analog in
// #define SHARP5              PC_1  // analog in

#define COLOR_L_GREEN       PC_0  // analog in
#define COLOR_L_BLUE        PA_5  // analog in
#define COLOR_L_AX12_ID     11
#define COLOR_L_REVERSED    false

#define COLOR_R_GREEN       PA_6  // analog in
#define COLOR_R_BLUE        PA_7  // analog in
#define COLOR_R_AX12_ID     14
#define COLOR_R_REVERSED    true

// #define PWM_7               PA_8
// #define PWM_8               PB_10
// #define PWM_C1              PB_4
// #define PWM_C2              PB_5
// #define PWM_a               PB_1
// #define PWM_b               PB_15
// #define PWM_c               PC_4

// #define LASER1              PB_14
// #define LASER2              PB_13

// todo IR

#endif
