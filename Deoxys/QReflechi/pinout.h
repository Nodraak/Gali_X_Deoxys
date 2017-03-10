#ifndef PINOUT_H_INCLUDED
#define PINOUT_H_INCLUDED

#ifndef SIMULATION

    #define XBEE_TX         D1
    #define XBEE_RX         D0

    #define CAN_TX          D2
    #define CAN_RX          D10

    #define BUZZER_PIN      D5

#else  // #ifndef SIMULATION

    #define USBTX           D0
    #define USBRX           D1
    #define XBEE_TX         D2
    #define XBEE_RX         D3
    #define SCREEN_TX       D4
    #define SCREEN_RX       D5

#endif  // #ifndef SIMULATION

#endif
