
#include "mbed.h"

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
    Debug *debug = NULL;
    Timer *loop = NULL, *match = NULL;

    /*
        Initializing
    */

    PwmOut buzzer_(PC_8);

    buzzer_.period(1./4000);
    buzzer_.write(0.50);
    wait_ms(200);
    buzzer_.period(1./3000);
    buzzer_.write(0.50);
    wait_ms(200);
    buzzer_.period(1./2000);
    buzzer_.write(0.50);
    wait_ms(400);
    buzzer_.period_us(1);

    debug = new Debug;

    debug->printf("Initializing\n");

    mem_stats_dynamic(debug);
    mem_stats_objects(debug);
    mem_stats_settings(debug);
    test_run_all(debug);

    debug->printf("CanMessenger...\n");
    CanMessenger *messenger = new CanMessenger;

    debug->printf("Timer (loop)...\n");
    loop = new Timer;
    loop->start();

    debug->printf("OrdersFIFO...\n");
    OrdersFIFO *orders = new OrdersFIFO(ORDERS_COUNT);

    debug->printf("Timer (match)...\n");
    match = new Timer;
    match->start();

    debug->printf("demo_load()...\n");
    int ret = demo_load(orders, demo_table, DEMO_TABLE_SIZE);
    if (ret != 0)
    {
        debug->printf("ERROR when filling OrdersFIFO (%d)\n", ret);
        while (1)
            ;
    }

    // init sharp + other sensors
    // init servos + other actuators

    // init ia ?

    mem_stats_dynamic(debug);

    debug->printf("Initialisation done.\n\n");
    debug->set_current_level(Debug::DEBUG_DEBUG);

    /*
        Ready, wait for tirette
    */

    // todo wait for other boards -> ping msg over can -> or wait to be in while to find out | + setup isr to ping ?
    // todo wait for tirette

    /*
        Go!
    */

    match->reset();
    while (true)  // todo match.read() < 90
    {
        loop->reset();
        debug->printf("[timer/match] %.3f\n", match->read());

        // todo ping/pong each board -> if no response since XX, then do something

        // update sharp + other sensors

        com_handle_serial(debug, messenger);
        com_handle_can(debug, messenger, orders);

        main_sleep(debug, loop);
    }

    // todo stop motors over can
    // todo funny action

    /*
        Cleanup
    */

    debug->set_current_level(Debug::DEBUG_INITIALISATION);
    debug->printf("Cleaning...\n");

    delete match;
    delete orders;
    delete loop;
    delete messenger;
    delete debug;

    return 0;
}
