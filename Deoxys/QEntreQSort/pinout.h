#ifndef PINOUT_H_INCLUDED
#define PINOUT_H_INCLUDED

#ifndef SIMULATION

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

#else  // #ifndef SIMULATION

    #define USBTX           D0
    #define USBRX           D1
    #define XBEE_TX         D2
    #define XBEE_RX         D3
    #define SCREEN_TX       D4
    #define SCREEN_RX       D5

#endif  // #ifndef SIMULATION

#endif
