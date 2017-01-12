
#include "mbed.h"
#include "rtos.h"
#include "mbed_stats.h"

#include "common/Debug.h"
#include "common/Messenger.h"
#include "common/com.h"
#include "common/utils.h"

#include "common/test.h"

#include "pinout.h"



void mem_stats(Debug *debug)
{
    debug->printf("----- sizeof\n");
    debug->printf("Debug            %d\n", sizeof(Debug));
    debug->printf("\tBufferedSerial %d*%d=%d\n", sizeof(BufferedSerial), Debug::DEBUG_LAST, sizeof(BufferedSerial)*Debug::DEBUG_LAST);
    debug->printf("Timer            %d\n", sizeof(Timer));
    debug->printf("CanMessenger     %d\n", sizeof(CanMessenger));

    mbed_stats_heap_t heap_stats;
    osEvent info;
    osThreadId main_id = osThreadGetId();

    debug->printf("----- heap\n");

    mbed_stats_heap_get(&heap_stats);
    debug->printf("Current heap: %d\n", heap_stats.current_size);
    debug->printf("Max heap size: %d\n", heap_stats.max_size);

    debug->printf("----- stack\n");

    info = _osThreadGetInfo(main_id, osThreadInfoStackSize);
    if (info.status != osOK)
        error("Could not get stack size");
    uint32_t stack_size = (uint32_t)info.value.v;

    info = _osThreadGetInfo(main_id, osThreadInfoStackMax);
    if (info.status != osOK)
        error("Could not get max stack");
    uint32_t max_stack = (uint32_t)info.value.v;

    debug->printf("Stack used %d of %d bytes\n", max_stack, stack_size);

    debug->printf("-----\n");
}


int main(void)
{
    int to_sleep = 0;

    Debug *debug = new Debug;

    mem_stats(debug);
    test_run_all(debug);
    Thread::wait(500);

    /*
        Initializing
    */

    debug->printf("Initializing\n");

    Timer match, loop;  // todo dynamic alloc ?
    match.start();
    loop.start();
    CanMessenger *messenger = new CanMessenger;

    // init sharp + other sensors
    // init servos + other actuators


    // init ia ?

    // init tirette interrupt -> polling

    debug->printf("Initialisation done.\n\n");

    mem_stats(debug);

    /*
        Ready, wait for tirette
    */

    // todo wait for tirette

    /*
        Go!
    */

    match.reset();
    while (true)
    {
        loop.reset();
        debug->printf("[timer/match] %.3f\n", match.read());

        /*
            inputs
        */

        // update sharp + other sensors

        com_handle_serial(debug, messenger);
        com_handle_can(debug, messenger);

        Message rec_msg;
        while (messenger->read_msg(&rec_msg))
        {
            debug->printf("Message %d %d -", rec_msg.id, rec_msg.len);
            debug->printf(" %x", rec_msg.payload.raw_data[0]);
            debug->printf(" %x", rec_msg.payload.raw_data[1]);
            debug->printf(" %x", rec_msg.payload.raw_data[2]);
            debug->printf(" %x", rec_msg.payload.raw_data[3]);

            debug->printf(" ");

            debug->printf(" %x", rec_msg.payload.raw_data[4]);
            debug->printf(" %x", rec_msg.payload.raw_data[5]);
            debug->printf(" %x", rec_msg.payload.raw_data[6]);
            debug->printf(" %x", rec_msg.payload.raw_data[7]);

            debug->printf("\n");
        }

        /*
            Computations
        */


        /*
            outputs
        */

        // debug

        // sleep
        to_sleep = 1000/MAIN_LOOP_FPS - loop.read_ms();
        if (to_sleep > 0)
        {
            debug->printf("[timer/loop] %d\n\n", loop.read_ms());
            Thread::wait(to_sleep);
        }
        else
        {
            debug->printf("[timer/loop] Warn: to_sleep == %d < 0\n\n", to_sleep);
        }
    }

    // do some cleanup ?

    return 0;
}
