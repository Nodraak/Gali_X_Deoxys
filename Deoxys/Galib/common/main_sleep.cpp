
#include "mbed.h"

#include "common/Debug.h"

#include "config.h"

#include "common/main_sleep.h"


void main_print_time(Debug *debug, Timer *main_timer)
{
    debug->printf("[timer/match] %.3f\n", main_timer->read());
}


void main_sleep(Debug *debug, Timer *loop)
{
    int to_sleep = 0;  // unit: ms

    to_sleep = (int)(1000*MAIN_LOOP_DELAY) - loop->read_ms();

    if (to_sleep > 0)
        Thread::wait(to_sleep);  // ms
    else
        debug->printf("Warn: main_sleep() %d > %d\n", loop->read_ms(), (int)1000*MAIN_LOOP_DELAY);
}
