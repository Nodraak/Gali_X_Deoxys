
#include "mbed.h"

#include "common/Debug.h"
#include "common/Messenger.h"
#include "common/Monitoring.h"
#include "common/OrdersFIFO.h"
#include "common/StatusLeds.h"
#include "common/com.h"
#include "common/init.h"
#include "common/main_sleep.h"
#include "common/utils.h"
#include "QEntreQSort/Ax12Driver.h"
#include "QEntreQSort/Actuators.h"

#include "config.h"
#include "pinout.h"


// move in Actuators ?? or OrdersFIFO ??
bool main_update_cur_order(Actuators *actuators, OrdersFIFO *orders, float time_since_last_order_finished, bool *cqb_finished, bool *cqr_finished)
{
    bool is_current_order_executed_ = false;

    switch (orders->current_order_.type)
    {
        case ORDER_EXE_TYPE_NONE:
            // nothing to do
            is_current_order_executed_ = true;
            break;

        case ORDER_EXE_TYPE_DELAY:
            if (time_since_last_order_finished > orders->current_order_.delay)
                is_current_order_executed_ = true;
            break;

        case ORDER_EXE_TYPE_WAIT_CQB_FINISHED:
            if (*cqb_finished)
            {
                *cqb_finished = false;
                is_current_order_executed_ = true;
            }
            break;

        case ORDER_EXE_TYPE_WAIT_CQR_FINISHED:
            if (*cqr_finished)
            {
                *cqr_finished = false;
                is_current_order_executed_ = true;
            }
            break;

        case ORDER_EXE_TYPE_WAIT_CQES_FINISHED:
            // ignore here
            break;

        case ORDER_EXE_TYPE_MOV_POS:
        case ORDER_EXE_TYPE_MOV_ANGLE:
            // ignore on CQES
            break;

        case ORDER_EXE_TYPE_ACTUATOR:
            actuators->activate(orders->current_order_.act_param);
            is_current_order_executed_ = true;
            break;

        case ORDER_EXE_TYPE_LAST:
            // nothing to do
            break;
    }

    return is_current_order_executed_;
}


int main(void)
{
    Timer *main_timer = NULL;
    StatusLeds *sl = NULL;
    Debug *debug = NULL;
    CanMessenger *messenger = NULL;
    OrdersFIFO *orders = NULL;
    EventQueue *queue = NULL;
    Timer *loop = NULL;

    Actuators *actuators = NULL;
    float last_order_executed_timestamp = -1;

    init_common(
        &main_timer,
        &sl,
        &debug,
        &messenger,
        &orders,
        &queue,
        &loop
    );
    init_board_CQES(debug,
        &actuators
    );
    init_finalize(debug, main_timer, queue);

    bool cqb_finished = false, cqr_finished = false;


    /*
        Go!
    */

    main_timer->reset();

    while (true)
    {
        g_mon->main_loop.start_new();
        loop->reset();

        queue->dispatch(0);  // non blocking dispatch
        com_handle_can(debug, messenger, orders, &cqb_finished, &cqr_finished, actuators);

        // equiv MC::updateCurOrder
        // update the goals in function of the given order
        bool is_current_order_executed_ = main_update_cur_order(
            actuators, orders, main_timer->read()-last_order_executed_timestamp, &cqb_finished, &cqr_finished
        );

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
            last_order_executed_timestamp = main_timer->read();
        }

        messenger->send_msg_I_am_doing(orders->current_order_.type);

        g_mon->main_loop.stop_and_save();
        main_sleep(debug, loop);
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
