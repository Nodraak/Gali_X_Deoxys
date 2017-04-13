
#include "mbed.h"

#include "common/Debug.h"
#include "common/Messenger.h"
#include "common/OrdersFIFO.h"
#include "common/com.h"
#include "common/main_sleep.h"
#include "common/mem_stats.h"
#include "common/utils.h"
#include "common/sys.h"
#include "QBouge/MotionController.h"

#include "common/test.h"
#include "QBouge/test_mc.h"

#include "config.h"
#include "pinout.h"


MotionController *mc = NULL;
bool request_next_order = false;
void asserv_main(void);


int main(void)
{
    Debug *debug = NULL;
    CanMessenger *messenger = NULL;
    OrdersFIFO *orders = NULL;
    Timer *loop = NULL;
    Ticker *asserv_ticker = NULL;
    Timer match;
    match.start();
    bool cqes_finished = false;

    PwmOut *ml = new PwmOut(MOTOR_L_PWM);
    ml->period(0.001 * 0.05);
    ml->write(0);
    PwmOut *mr = new PwmOut(MOTOR_R_PWM);
    mr->period(0.001 * 0.05);
    mr->write(0);

    /*
        Initializing
    */

    debug = new Debug;
    debug_pre_init(debug);

    debug->printf("Initializing\n");

    debug->printf("CanMessenger...\n");
    messenger = new CanMessenger;

    debug->printf("t=%f\n", match.read());

    mem_stats_objects(debug);
    mem_stats_settings(debug);
    test_run_all(debug);
    debug->printf("CAN_FRAME_BUS_OCCUPATION %.3f ms\n", CAN_FRAME_BUS_OCCUPATION*1000);
    debug->printf("CAN_MAX_MSG_PER_SEC %.1f\n", CAN_MAX_MSG_PER_SEC);
    debug->printf("CAN_MAX_MSG_PER_200Hz_FRAME %.1f\n", CAN_MAX_MSG_PER_200Hz_FRAME);

    debug->printf("OrdersFIFO...\n");
    orders = new OrdersFIFO(ORDERS_COUNT);

    debug->printf("Timer...\n");
    loop = new Timer;
    loop->start();

    delete ml;
    delete mr;

    debug->printf("MotionController...\n");
    mc = new MotionController;

    debug->printf("Ticker...\n");
    asserv_ticker = new Ticker;  // ISR. This uses the TIMER2 (TIM2_IRQn)
    asserv_ticker->attach(callback(mc, &MotionController::asserv), ASSERV_DELAY);

    debug->printf("interrupt_priorities...\n");
    sys_interrupt_priorities_init();

    mem_stats_dynamic(debug);

    debug->printf("Initialisation done. (%f)\n\n", match.read());
    debug->set_current_level(Debug::DEBUG_DEBUG);

    // todo wait for tirette (can msg)

    /*
        Go!
    */

    match.reset();

    while (true)
    {
        loop->reset();
        debug->printf("[timer/match] %.3f\n", match.read());

        com_handle_can(debug, messenger, orders, &cqes_finished, mc);

        if (mc->current_order_.type == ORDER_EXE_TYPE_WAIT_CQB_FINISHED)
        {
            messenger->send_msg_CQB_finished();
            mc->is_current_order_executed_ = true;
        }

        if (cqes_finished && (mc->current_order_.type == ORDER_EXE_TYPE_WAIT_CQES_FINISHED))
        {
            mc->is_current_order_executed_ = true;
            cqes_finished = false;
        }

        if (mc->is_current_order_executed_)
        {
            // get the next order
            while (orders->next_order_execute())
                ;

            // copy it to MC - WARNING: disable asserv interrupt for as little as possible
            NVIC_DisableIRQ(TIM2_IRQn);
            memcpy(&mc->current_order_, &orders->current_order_, sizeof(s_order_exe));
            NVIC_EnableIRQ(TIM2_IRQn);

            mc->is_current_order_executed_ = false;
        }

        if (orders->next_order_should_request())
            messenger->send_msg_CQB_next_order_request(1);

        mc->debug(debug);
        mc->debug(messenger);

        main_sleep(debug, loop);
    }

    /*
        Cleanup
    */

    debug->set_current_level(Debug::DEBUG_INITIALISATION);
    debug->printf("Cleaning...\n");

    asserv_ticker->detach();
    delete asserv_ticker;
    delete loop;
    delete messenger;
    delete mc;
    delete debug;

    return 0;
}
