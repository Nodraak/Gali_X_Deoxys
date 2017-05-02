#ifndef PINOUT_H_INCLUDED
#define PINOUT_H_INCLUDED

#ifndef SIMULATION

    #define CAN_TX              D2
    #define CAN_RX              D10

    #define AX12_PIN_SWITCH     D13
    #define AX12_PIN_TX         D1
    #define AX12_PIN_RX         D0

    #define ACT_L_CLAMP         D6
    #define ACT_L_PUMP          D12
    #define ACT_L_FLAP          A1

    #define ACT_R_CLAMP         D5
    #define ACT_R_PUMP          D11
    #define ACT_R_FLAP          A2

    #define ACT_PROGRADE_DISPENSER  D3

#else  // #ifndef SIMULATION

    #define USBTX           D0
    #define USBRX           D1
    #define XBEE_TX         D2
    #define XBEE_RX         D3
    #define SCREEN_TX       D4
    #define SCREEN_RX       D5

#endif  // #ifndef SIMULATION

#endif
