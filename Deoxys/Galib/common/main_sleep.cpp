
#include "mbed.h"

#include "common/Debug.h"

#include "config.h"


void main_sleep(Debug *debug, Timer *loop)
{
    int to_sleep = 0;  // unit: ms

    to_sleep = 1000*MAIN_LOOP_DELAY - loop->read_ms();

    if (to_sleep > 0)
        Thread::wait(to_sleep);  // ms
    else
        debug->printf("[timer/loop] Warn: to_sleep %d < 0 (loop %d > %d) (ms)\n\n", to_sleep, loop->read_ms(), 1000*MAIN_LOOP_DELAY);
}
