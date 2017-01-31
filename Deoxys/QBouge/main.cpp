
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


MotionController *mc = NULL;
CanMessenger *messenger = NULL;

void asserv_main(void);


int main(void)
{
    Debug *debug = NULL;
    Timer *loop = NULL;
    Ticker *asserv_ticker = NULL;

    /*
        Initializing
    */

    debug = new Debug;

    debug->printf("Initializing\n");

    mem_stats_dynamic(debug);
    mem_stats_objects(debug);
    mem_stats_settings(debug);
    test_run_all(debug);

    messenger = new CanMessenger;
    loop = new Timer;
    loop->start();

    mc = new MotionController;
    asserv_ticker = new Ticker;
    asserv_ticker->attach(asserv_main, ASSERV_DELAY);

    mem_stats_dynamic(debug);

    debug->printf("Initialisation done.\n\n");

    // todo wait for tirette (can msg)

    /*
        Go!
    */

    while (true)
    {
        loop->reset();

        com_handle_serial(debug, messenger, mc);
        com_handle_can(debug, messenger, mc);

        mc->debug(debug);

        main_sleep(debug, loop);
    }

    /*
        Cleanup
    */

    debug->printf("Cleaning...\n");
    Thread::wait(100);

    asserv_ticker->detach();
    delete asserv_ticker;
    delete loop;
    delete messenger;
    delete mc;
    delete debug;

    return 0;
}


void asserv_main(void)
{
    Timer timer;

    timer.start();

    // Input

    mc->fetchEncodersValue();

    // Compute

    mc->updatePosition();
    mc->updateCurOrder();
    // if room for storing another order is available, request the next one
    if (ORDERS_COUNT - mc->orders_->size() > 0)
        messenger->send_msg_CQB_next_order_request(ORDERS_COUNT - mc->orders_->size());
    mc->computePid();

    // Iutput

    mc->updateMotors();

    // Timer stuff

    timer.stop();
    if (timer.read() > ASSERV_DELAY)
    {
        // todo: we are in the shit :/
    }
}
