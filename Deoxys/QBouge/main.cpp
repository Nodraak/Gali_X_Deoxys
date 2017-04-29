
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
    init_finalize(debug, main_timer);

    bool cqes_finished = false;

    /*
        Go!
    */

    main_timer->reset();

    while (true)
    {
        g_mon->main_loop.start_new();
        loop->reset();

        queue->dispatch(0);  // non blocking dispatch
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
            messenger->send_msg_CQB_next_order_request();

        messenger->send_msg_I_am_doing(orders->current_order_.type);

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
