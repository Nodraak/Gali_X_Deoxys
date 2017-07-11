
#include "mbed.h"

#include "common/Debug.h"
#include "common/Messenger.h"
#include "common/Monitoring.h"
#include "common/OrdersFIFO.h"
#include "common/com.h"
#include "common/init.h"
#include "common/main_sleep.h"
#include "common/utils.h"
#include "QBouge/MotionController.h"

#include "config.h"
#include "pinout.h"


int main(void)
{
    Timer *main_timer = NULL;
    StatusLeds *sl = NULL;
    Debug *debug = NULL;
    CanMessenger *messenger = NULL;
    OrdersFIFO *orders = NULL;
    EventQueue *queue = NULL;
    Timer *loop = NULL;

    MotionController *mc = NULL;

    init_common(
        &main_timer,
        &sl,
        &debug,
        &messenger,
        &orders,
        &queue,
        &loop
    );
    init_board_CQB(debug,
        queue,
        messenger,
        &mc
    );
    init_finalize(debug, main_timer, queue);

    bool cqes_finished = false, cqr_finished = false;

    /*
        Go!
    */

#define FUNNY_ACTION_OFF        0.02
#define FUNNY_ACTION_ON         0.07
#define PIN_TIRETTE             A4
#define PIN_FUNNY_ACTION        A5
#define TIME_FUNNY_ACTION       92.000
#define MOVE_TIME               2.000
DigitalIn tirette(PIN_TIRETTE);
Timer t_move;
t_move.start();
Timer t_funny_action;
PwmOut funny_action(PIN_FUNNY_ACTION);
funny_action.period(1.0/50);
funny_action.write(FUNNY_ACTION_OFF);

debug->printf("======= TIRETTE =======\n");

while (tirette.read() == 1)
    queue->dispatch(0);
t_funny_action.start();

Timer t_print;
t_print.start();

if (THIS_IS_THE_CUP)
    mc->this_is_the_cup_and_nothing_is_working(0.4);

debug->printf("======= GO =======\n");

    main_timer->reset();

    while (true)
    {
        g_mon->main_loop.start_new();
        loop->reset();

        queue->dispatch(0);  // non blocking dispatch
        com_handle_can(debug, messenger, orders, &cqes_finished, &cqr_finished, mc);

if (!THIS_IS_THE_CUP)
{
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
        if (cqr_finished && (mc->current_order_.type == ORDER_EXE_TYPE_WAIT_CQR_FINISHED))
        {
            mc->is_current_order_executed_ = true;
            cqr_finished = false;
        }

        if (mc->is_current_order_executed_)
        {
            // get the next order
            while (orders->next_order_execute())
                ;

            mc->update_current_order(&orders->current_order_);
            mc->is_current_order_executed_ = false;
        }

        if (orders->next_order_should_request())
            messenger->send_msg_CQB_next_order_request();

        // messenger->send_msg_I_am_doing(orders->current_order_.type);
}

if (THIS_IS_THE_CUP)
{
    if (t_funny_action.read() >= MOVE_TIME)
        mc->this_is_the_cup_and_nothing_is_working(0);
}

if (t_funny_action.read() >= TIME_FUNNY_ACTION)
{
    funny_action.write(FUNNY_ACTION_ON);
}

        g_mon->main_loop.stop_and_save();
        main_sleep(debug, loop);
    }

    /*
        Cleanup
    */

    debug->set_current_level(Debug::DEBUG_INITIALISATION);
    debug->printf("Cleaning...\n");

    delete loop;
    delete messenger;
    delete mc;
    delete debug;

    return 0;
}
