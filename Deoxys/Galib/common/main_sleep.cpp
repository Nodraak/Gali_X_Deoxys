
#include "mbed.h"

#include "common/Debug.h"

#include "config.h"


void main_sleep(Debug *debug, Timer *loop)
{
    int to_sleep = 0;  // unit: ms

    to_sleep = 1000*MAIN_LOOP_DELAY - loop->read_ms();

    if (to_sleep > 0)
        debug->printf("[timer/loop] loop %d.%03d (to_sleep %d) (ms)\n\n", loop->read_ms(), loop->read_us()%1000, to_sleep);
    else
        debug->printf("[timer/loop] Warn: to_sleep %d < 0 (loop %d > %d) (ms)\n\n", to_sleep, loop->read_ms(), 1000*MAIN_LOOP_DELAY);

    if (to_sleep > 0)
        Thread::wait(to_sleep);  // ms
}
