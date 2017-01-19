#ifndef PINOUT_H_INCLUDED
#define PINOUT_H_INCLUDED

#ifndef SIMULATION

#if defined(TARGET_NUCLEO_L476RG)

    #define MOTOR_L_CUR PC_5
    #define MOTOR_L_PWM PC_6
    #define MOTOR_L_DIR PC_8
    #define ENC_L_DATA1 PA_12
    #define ENC_L_DATA2 PA_11

    #define MOTOR_R_CUR PB_1
    #define MOTOR_R_PWM PB_15
    #define MOTOR_R_DIR PB_14
    #define ENC_R_DATA1 PB_13
    #define ENC_R_DATA2 PB_2

    #define XBEE_TX     PC_10
    #define XBEE_RX     PC_11
    #define SCREEN_TX   PC_12
    #define SCREEN_RX   PD_2

#endif // TARGET_NUCLEO_L476RG

#ifdef TARGET_NUCLEO_F401RE

    #define MOTOR_L_CUR PC_5
    #define MOTOR_L_PWM PC_6
    #define MOTOR_L_DIR PC_8
    #define ENC_L_DATA1 PA_12
    #define ENC_L_DATA2 PA_11

    #define MOTOR_R_CUR PB_1
    #define MOTOR_R_PWM PB_15
    #define MOTOR_R_DIR PB_14
    #define ENC_R_DATA1 PB_13
    #define ENC_R_DATA2 PB_2

#warning "Don't run this in production, XBEE and SCREEN pinout are colliding"
    #define XBEE_TX     PA_9
    #define XBEE_RX     PA_10
    #define SCREEN_TX   PA_9
    #define SCREEN_RX   PA_10

#endif // TARGET_NUCLEO_F401RE

#ifdef TARGET_NUCLEO_F303K8

    #define MOTOR_L_PWM     D7  // PF_0
    #define MOTOR_L_DIR     D11 // PB_5
    #define MOTOR_L_CUR     D6  // PB_1
    #define MOTOR_L_TH      NC
    #define MOTOR_L_BRK     NC

    #define ENC_L_DATA1     D3  // PB_0
    #define ENC_L_DATA2     D12 // PB_4

    #define MOTOR_R_PWM     A5  // PA_6
    #define MOTOR_R_DIR     D13 // PB_3
    #define MOTOR_R_CUR     A4  // PA_5
    #define MOTOR_R_TH      NC
    #define MOTOR_R_BRK     NC

    #define ENC_R_DATA1     A1  // PA_1
    #define ENC_R_DATA2     A2  // PA_3

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
