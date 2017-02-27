#ifndef PINOUT_H_INCLUDED
#define PINOUT_H_INCLUDED

#ifndef SIMULATION

#if defined(TARGET_NUCLEO_L476RG)

    #define XBEE_TX     PC_10
    #define XBEE_RX     PC_11
    #define SCREEN_TX   PC_12
    #define SCREEN_RX   PD_2
    #define CAN_TX      PA_12
    #define CAN_RX      PA_11

#endif // TARGET_NUCLEO_L476RG

#ifdef TARGET_NUCLEO_F401RE

#warning "Don't run this in production, XBEE and SCREEN pinout are colliding"
    #define XBEE_TX     PA_9
    #define XBEE_RX     PA_10
    #define SCREEN_TX   PA_9
    #define SCREEN_RX   PA_10

#endif // TARGET_NUCLEO_F401RE

#ifdef TARGET_NUCLEO_F303K8

#warning "Don't run this in production, XBEE and SCREEN pinout are colliding"
    #define XBEE_TX         D1
    #define XBEE_RX         D0
    #define SCREEN_TX       D1
    #define SCREEN_RX       D0

    #define CAN_TX          D2  // PA_12
    #define CAN_RX          D10 // PA_11

#endif

#else  // #ifndef SIMULATION

    #define USBTX           D0
    #define USBRX           D1
    #define XBEE_TX         D2
    #define XBEE_RX         D3
    #define SCREEN_TX       D4
    #define SCREEN_RX       D5

#endif  // #ifndef SIMULATION

#endif
