#ifndef PINOUT_H_INCLUDED
#define PINOUT_H_INCLUDED

#ifndef SIMULATION

    #define CAN_TX              D2
    #define CAN_RX              D10

    #define AX12_PIN_SWITCH     D13
    #define AX12_PIN_TX         D1
    #define AX12_PIN_RX         D0

    #define AX12_L_PIN_SERVO    D5
    #define AX12_L_PIN_VALVE    D12
    #define AX12_R_PIN_SERVO    D6
    #define AX12_R_PIN_VALVE    D11

#else  // #ifndef SIMULATION

    #define USBTX           D0
    #define USBRX           D1
    #define XBEE_TX         D2
    #define XBEE_RX         D3
    #define SCREEN_TX       D4
    #define SCREEN_RX       D5

#endif  // #ifndef SIMULATION

#endif
