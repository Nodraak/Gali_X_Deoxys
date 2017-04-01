
#include "mbed.h"

#include "common/Debug.h"
#include "common/Messenger.h"
#include "common/OrdersFIFO.h"
#include "common/StatusLeds.h"
#include "common/com.h"
#include "common/main_sleep.h"
#include "common/mem_stats.h"
#include "common/utils.h"
#include "QEntreQSort/RoboticArm.h"

#include "common/test.h"

#include "config.h"
#include "pinout.h"

Debug *g_debug = NULL;


int main(void)
{
    Debug *debug = NULL;
    CanMessenger *messenger = NULL;
    Timer *loop = NULL;
    Timer match;
    match.start();

    bool is_current_order_executed_ = false;
    float last_order_executed_timestamp = -1;

    StatusLeds sl(A5, NC, A4, NC);

    /*
        Initializing
    */

    debug = new Debug;
g_debug = debug;
    debug_pre_init(debug);

    debug->printf("Initializing\n");

    debug->printf("CanMessenger...\n");
    messenger = new CanMessenger;

    sl.init_half();

    mem_stats_objects(debug);
    mem_stats_settings(debug);
    test_run_all(debug);

    debug->printf("Timer...\n");
    loop = new Timer;
    loop->start();

    OrdersFIFO *orders = new OrdersFIFO(ORDERS_COUNT);

    debug->printf("EventQueue...\n");

    EventQueue queue;

    int led_id = queue.call_every(500, callback(&sl, &StatusLeds::running));  // 500 ms

    messenger->on_receive_add(Message::MT_CQB_pong, callback(&sl, &StatusLeds::on_CQB_pong));
    messenger->on_receive_add(Message::MT_CQES_pong, callback(&sl, &StatusLeds::on_CQES_pong));

    debug->printf("AX12_arm...\n");
    AX12_arm ax12_arm(1, 8, 9, AX12_PIN_SERVO, AX12_PIN_VALVE);

    mem_stats_dynamic(debug);

    debug->printf("Initialisation done (%f).\n\n", match.read());
    debug->set_current_level(Debug::DEBUG_DEBUG);

    wait_ms(500);

    // todo
    // while (true)
    {
        // if t > 1 sec
            // send can ping

        // if receive pong
            // break
    }

    // todo wait for tirette (can msg)

    // while (1)
        // ;

    /*
        Go!
    */

    match.reset();

    while (true)
    {
        loop->reset();
        debug->printf("[timer/match] %.3f\n", match.read());

        com_handle_can(debug, messenger, orders, &ax12_arm);

        // equiv MC::updateCurOrder
        queue.dispatch(0);  // non blocking dispatch

        // update the goals in function of the given order

        int time_since_last_order_finished = match.read() - last_order_executed_timestamp;

        switch (orders->current_order_.type)
        {
            case ORDER_EXE_TYPE_NONE:
                // nothing to do
                is_current_order_executed_ = 1;
                break;

            case ORDER_EXE_TYPE_DELAY:
                // todo wait -> timer_.read()-last_order_executed_timestamp_
                if (time_since_last_order_finished > orders->current_order_.delay)
                    is_current_order_executed_ = true;
                break;

            case ORDER_EXE_TYPE_WAIT_CQB_FINISHED:
            case ORDER_EXE_TYPE_WAIT_CQES_FINISHED:
            case ORDER_EXE_TYPE_POS:
            case ORDER_EXE_TYPE_ANGLE:
                // ignore on CQES
                break;

            case ORDER_EXE_TYPE_ARM_INIT:
                ax12_arm.seq_init();
                orders->current_order_.type = ORDER_EXE_TYPE_DELAY;
                orders->current_order_.delay = SLEEP_INIT;
                last_order_executed_timestamp = match.read();
                // is_current_order_executed_ = true;
                break;
            case ORDER_EXE_TYPE_ARM_GRAB:
                ax12_arm.seq_grab();
                orders->current_order_.type = ORDER_EXE_TYPE_DELAY;
                orders->current_order_.delay = SLEEP_GRAB;
                last_order_executed_timestamp = match.read();
                // is_current_order_executed_ = true;
                break;
            case ORDER_EXE_TYPE_ARM_MOVE_UP:
                ax12_arm.seq_move_up();
                orders->current_order_.type = ORDER_EXE_TYPE_DELAY;
                orders->current_order_.delay = SLEEP_MOVE;
                last_order_executed_timestamp = match.read();
                // is_current_order_executed_ = true;
                break;
            case ORDER_EXE_TYPE_ARM_RELEASE:
                ax12_arm.seq_release();
                orders->current_order_.type = ORDER_EXE_TYPE_DELAY;
                orders->current_order_.delay = SLEEP_RELEASE;
                last_order_executed_timestamp = match.read();
                // is_current_order_executed_ = true;
                break;
            case ORDER_EXE_TYPE_ARM_MOVE_DOWN:
                ax12_arm.seq_move_down();
                orders->current_order_.type = ORDER_EXE_TYPE_DELAY;
                orders->current_order_.delay = SLEEP_MOVE;
                last_order_executed_timestamp = match.read();
                // is_current_order_executed_ = true;
                break;

            case ORDER_EXE_TYPE_LAST:
                // nothing to do
                break;
        }

        // end equiv MC::updateCurOrder

        if (orders->current_order_.type == ORDER_EXE_TYPE_WAIT_CQES_FINISHED)
        {
            messenger->send_msg_CQES_finished();
            is_current_order_executed_ = true;
        }

        if (is_current_order_executed_)
        {
            // get the next order
            while (orders->next_order_execute())
                ;

            is_current_order_executed_ = false;
            last_order_executed_timestamp = match.read();
        }


        // main_sleep(debug, loop);
        Thread::wait(100);  // ms
    }

    /*
        Cleanup
    */

    debug->set_current_level(Debug::DEBUG_INITIALISATION);
    debug->printf("Cleaning...\n");

    delete messenger;
    delete debug;

    return 0;
}
