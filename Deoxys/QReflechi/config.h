#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED

// #define PRINT_TIME
#define PRINT_COM_CAN_REC
// #define PRINT_COM_CAN_SEND
#define PRINT_MONITORING_RESET
#define PRINT_SYS_CAN_STATS


#define MAIN_LOOP_FPS       50
#define MAIN_LOOP_DELAY     (1.0/MAIN_LOOP_FPS)


#define ORDERS_COUNT        200

// demo_table
// #define MC_START_X          (130+75+10)     // 215
// #define MC_START_Y          (1080-160)      // 920
// #define MC_START_ANGLE      DEG2RAD(0)

// demo_table_arm
#define MC_START_X          130
#define MC_START_Y          920
#define MC_START_ANGLE      DEG2RAD(180)

#endif // CONFIG_H_INCLUDED
