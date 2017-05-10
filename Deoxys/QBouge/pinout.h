#ifndef PINOUT_H_INCLUDED
#define PINOUT_H_INCLUDED

#ifndef SIMULATION

    #define MOTOR_L_PWM     D9
    #define MOTOR_L_DIR     D11

    #define ENC_L_DATA1     D3
    #define ENC_L_DATA2     D12

    #define MOTOR_R_PWM     D6
    #define MOTOR_R_DIR     D13

    #define ENC_R_DATA1     A1
    #define ENC_R_DATA2     A2

    #define CAN_TX          D2
    #define CAN_RX          D10

#else  // #ifndef SIMULATION

    #define USBTX           D0
    #define USBRX           D1
    #define XBEE_TX         D2
    #define XBEE_RX         D3

#endif  // #ifndef SIMULATION

#endif
