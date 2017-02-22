
#include "mbed.h"

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
bool request_next_order = false;
void asserv_main(void);


void interrupt_priorities_init(void)
{
    NVIC_SetPriorityGrouping(0);

    NVIC_SetPriority(EXTI0_IRQn, 1);
    NVIC_SetPriority(EXTI1_IRQn, 1);
    NVIC_SetPriority(EXTI2_TSC_IRQn, 2);
    NVIC_SetPriority(EXTI3_IRQn, 2);

    NVIC_SetPriority(TIM2_IRQn, 5); // asserv

    NVIC_SetPriority(CAN_TX_IRQn, 10);
    NVIC_SetPriority(CAN_RX0_IRQn, 10);
    NVIC_SetPriority(CAN_RX1_IRQn, 10);
    NVIC_SetPriority(CAN_SCE_IRQn, 10);

    NVIC_SetPriority(USART1_IRQn, 11);
    NVIC_SetPriority(USART2_IRQn, 11);
    NVIC_SetPriority(USART3_IRQn, 11);
}


int main(void)
{
    Debug *debug = NULL;
    CanMessenger *messenger = NULL;
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

    interrupt_priorities_init();

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

        if (request_next_order)
        {
            messenger->send_msg_CQB_next_order_request(ORDERS_COUNT - mc->orders_->size());
            request_next_order = false;
        }

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
        request_next_order = true;
    mc->computePid();

    // Output

    mc->updateMotors();

    // Timer stuff

    timer.stop();
    if (timer.read() > ASSERV_DELAY)
    {
        // todo: we are in the shit :/
    }
}
