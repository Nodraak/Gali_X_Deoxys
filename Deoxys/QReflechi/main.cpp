
#include "mbed.h"
#include "rtos.h"

#include "common/Debug.h"
#include "common/Messenger.h"
#include "common/OrdersFIFO.h"
#include "common/com.h"
#include "common/main_sleep.h"
#include "common/mem_stats.h"
#include "common/utils.h"

#include "common/test.h"

#include "demos.h"
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

    Timer match, loop;  // todo dynamic alloc ?
    match.start();
    loop.start();
    CanMessenger *messenger = new CanMessenger;
    OrdersFIFO *orders = new OrdersFIFO(ORDERS_COUNT);

    int ret = demo_jpo(orders);
    if (ret != 0)
    {
        debug->printf("ERROR when filling OrdersFIFO (%d)\n", ret);
        while (1)
            ;
    }

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
        com_handle_can(debug, messenger, orders);

        /*
            Computations
        */


        /*
            outputs
        */

        // debug

        main_sleep(debug, &loop);
    }

    // do some cleanup ?

    return 0;
}
