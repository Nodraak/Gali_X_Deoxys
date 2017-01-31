
#include "mbed.h"
#include "rtos.h"

#include "common/Debug.h"
#include "common/Messenger.h"
#include "common/OrdersFIFO.h"
#include "common/com.h"
#include "common/main_sleep.h"
#include "common/mem_stats.h"
#include "common/utils.h"
#include "QBouge/MotionController.h"

#include "common/test.h"
#include "QBouge/test_mc.h"

#include "config.h"
#include "pinout.h"


int main(void)
{
    Debug *debug = new Debug;

    mem_stats_dynamic(debug);
    mem_stats_objects(debug);
    mem_stats_settings(debug);
    test_run_all(debug);

    /*
        Initializing
    */

    debug->printf("Initializing\n");

    MotionController *mc = new MotionController;
    Timer match, loop;  // todo dynamic alloc ?
    match.start();
    loop.start();
    CanMessenger *messenger = new CanMessenger;

    // init sharp + other sensors
    // init servos + other actuators


    // init ia ?

    // init tirette interrupt -> polling

    debug->printf("Initialisation done.\n\n");

    mem_stats_dynamic(debug);

    /*
        Ready, wait for tirette
    */

    // todo wait for tirette

    /*
        Go!
    */

    uint8_t debug_frame_counter = 0;

    match.reset();
    while (true)
    {
        loop.reset();
        // todo debug level - enable/disable
        if (debug_frame_counter == 0)
            debug->printf("[timer/match] %.3f\n", match.read());

        /*
            inputs
        */

        // update qei
        mc->fetchEncodersValue();

        // update sharp + other sensors

        com_handle_serial(debug, messenger, mc);
        com_handle_can(debug, messenger, mc);

        /*
            Computations
        */

        mc->updatePosition();
        mc->updateCurOrder(match.read(), messenger);
        mc->computePid();

        /*
            outputs
        */

        // move
        mc->updateMotors();

        // debug
        if (debug_frame_counter == 0)
        {
            mc->debug(debug);
            mc->debug(messenger);
        }

        main_sleep(debug, &loop);
    }

    // do some cleanup ?

    return 0;
}
