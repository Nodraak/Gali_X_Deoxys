#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED

#define MAIN_LOOP_FPS       10
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

// demo_test_com
// #define MC_START_X          0
// #define MC_START_Y          0
// #define MC_START_ANGLE      DEG2RAD(0)

#endif // CONFIG_H_INCLUDED
