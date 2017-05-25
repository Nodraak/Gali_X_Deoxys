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

#define COLOR_L_GREEN       PC_0  // analog in
#define COLOR_L_BLUE        PA_5  // analog in
#define COLOR_L_AX12_ID     11
#define COLOR_L_REVERSED    false

#define COLOR_R_GREEN       PA_6  // analog in
#define COLOR_R_BLUE        PA_7  // analog in
#define COLOR_R_AX12_ID     14
#define COLOR_R_REVERSED    true

#endif
