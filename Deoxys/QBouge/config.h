#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED

#define MAIN_LOOP_FPS       10
#define MAIN_LOOP_DELAY     (1.0/MAIN_LOOP_FPS)

/*
    The asservissement function is call through an interrupt (Ticker) (a priori
    mbed links that to TIM2).
    For reference: gali IX 500 Hz (2 ms) - eseo 200 Hz (5 ms) - two randoms from
    GitHub 100 Hz (10 ms) and 67 Hz (15 ms).
*/
#define ASSERV_FPS          200
#define ASSERV_DELAY        (1.0/ASSERV_FPS)

#define ORDERS_COUNT        5

#define MC_START_X          130
#define MC_START_Y          (1080-150)
#define MC_START_ANGLE      0

#endif // CONFIG_H_INCLUDED
