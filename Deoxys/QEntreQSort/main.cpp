
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


float last_order_executed_timestamp = -1;


void main_do_com(Debug *debug, Actuators *actuators)
{
    char buffer[BUFFER_SIZE] = "";

    if (debug->get_line(buffer, BUFFER_SIZE) != -1)
    {
        /*
            p: ping
            ls: left seq
            rs: right seq
            aws: a write speed
            arp: all read pos
            act: actuator
        */
        if (strncmp(buffer, "p", 1) == 0)
        {
            actuators->left_.arm_.ping_all();
            actuators->right_.arm_.ping_all();
        }
        else if (strncmp(buffer, "ls", 2) == 0)
        {
            actuators->left_.arm_.init();
            wait(SLEEP_INIT);
            actuators->left_.arm_.grab();
            wait(SLEEP_GRAB);
            actuators->left_.arm_.move_up();
            wait(SLEEP_MOVE);
            actuators->left_.arm_.release();
            wait(SLEEP_RELEASE);
            actuators->left_.arm_.move_down();
            wait(SLEEP_MOVE);
        }
        else if (strncmp(buffer, "rs", 2) == 0)
        {
            actuators->right_.arm_.init();
            wait(SLEEP_INIT);
            actuators->right_.arm_.grab();
            wait(SLEEP_GRAB);
            actuators->right_.arm_.move_up();
            wait(SLEEP_MOVE);
            actuators->right_.arm_.release();
            wait(SLEEP_RELEASE);
            actuators->right_.arm_.move_down();
            wait(SLEEP_MOVE);
        }
        else if (strncmp(buffer, "aws", 3) == 0)
        {
            char *ptr = buffer+3+1;
            int a = 0;

            a = atoi(ptr);

            debug->printf("set speed %d\n", a);
            actuators->left_.arm_.write_speed_all(a);
            actuators->right_.arm_.write_speed_all(a);
        }
        else if (strncmp(buffer, "arp", 3) == 0)
        {
            actuators->left_.arm_.read_pos_all(debug);
            actuators->right_.arm_.read_pos_all(debug);
        }
        else if (strncmp(buffer, "act", 3) == 0)
        {
            char *ptr = buffer;

            while (ptr[0] != ' ')
                ptr += 1;
            ptr += 1;

            // parse

            t_act act = 0;

            if (strncmp(ptr, "l", 1) == 0)
                act |= ACT_SIDE_LEFT;
            else if (strncmp(ptr, "r", 1) == 0)
                act |= ACT_SIDE_RIGHT;

            while (ptr[0] != ' ')
                ptr += 1;
            ptr += 1;

            if (strncmp(ptr, "height", 6) == 0)
                act |= ACT_ACTUATOR_HEIGHT;
            else if (strncmp(ptr, "vert", 4) == 0)
                act |= ACT_ACTUATOR_VERT;
            else if (strncmp(ptr, "horiz", 5) == 0)
                act |= ACT_ACTUATOR_HORIZ;
            else if (strncmp(ptr, "clamp", 5) == 0)
                act |= ACT_ACTUATOR_CLAMP;
            else if (strncmp(ptr, "pump", 4) == 0)
                act |= ACT_ACTUATOR_PUMP;
            else if (strncmp(ptr, "flap", 4) == 0)
                act |= ACT_ACTUATOR_FLAP;
            else if (strncmp(ptr, "prog", 4) == 0)
                act |= ACT_ACTUATOR_PROG;

            while (ptr[0] != ' ')
                ptr += 1;
            ptr += 1;

            if (strncmp(ptr, "e", 1) == 0)
                act |= ACT_CONF_EXTENDED;
            else if (strncmp(ptr, "r", 1) == 0)
                act |= ACT_CONF_RETRACTED;

            while (ptr[0] != ' ')
                ptr += 1;
            ptr += 1;

            // exe

            actuators->set(act, ptr);
            actuators->print(debug, 0);
        }
        else
            debug->printf("unknown cmd\n");

    }
}

bool main_update_cur_order(Actuators *actuators, OrdersFIFO *orders, Timer *match)
{
    bool is_current_order_executed_ = false;
    int time_since_last_order_finished = match->read() - last_order_executed_timestamp;

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
        case ORDER_EXE_TYPE_WAIT_CQES_FINISHED:  // todo send send_msg_CQES_finished and remove it from main()
        case ORDER_EXE_TYPE_MOV_POS:
        case ORDER_EXE_TYPE_MOV_ANGLE:
            // ignore on CQES
            break;

        case ORDER_EXE_TYPE_ACT_ARM_INIT:
            actuators->side(orders->current_order_.act_param)->arm_.init();
            orders->current_order_.type = ORDER_EXE_TYPE_DELAY;
            orders->current_order_.delay = SLEEP_INIT;
            last_order_executed_timestamp = match->read();
            // is_current_order_executed_ = true;
            break;

        case ORDER_EXE_TYPE_ACT_ARM_GRAB:
            actuators->side(orders->current_order_.act_param)->arm_.grab();
            orders->current_order_.type = ORDER_EXE_TYPE_DELAY;
            orders->current_order_.delay = SLEEP_GRAB;
            last_order_executed_timestamp = match->read();
            // is_current_order_executed_ = true;
            break;

        case ORDER_EXE_TYPE_ACT_ARM_MOVE_UP:
            actuators->side(orders->current_order_.act_param)->arm_.move_up();
            orders->current_order_.type = ORDER_EXE_TYPE_DELAY;
            orders->current_order_.delay = SLEEP_MOVE;
            last_order_executed_timestamp = match->read();
            // is_current_order_executed_ = true;
            break;

        case ORDER_EXE_TYPE_ACT_ARM_RELEASE:
            actuators->side(orders->current_order_.act_param)->arm_.release();
            orders->current_order_.type = ORDER_EXE_TYPE_DELAY;
            orders->current_order_.delay = SLEEP_RELEASE;
            last_order_executed_timestamp = match->read();
            // is_current_order_executed_ = true;
            break;

        case ORDER_EXE_TYPE_ACT_ARM_MOVE_DOWN:
            actuators->side(orders->current_order_.act_param)->arm_.move_down();
            orders->current_order_.type = ORDER_EXE_TYPE_DELAY;
            orders->current_order_.delay = SLEEP_MOVE;
            last_order_executed_timestamp = match->read();
            // is_current_order_executed_ = true;
            break;

        case ORDER_EXE_TYPE_ACT_FLAP:
            if (orders->current_order_.act_param & ACT_CONF_OPEN)
            {
                actuators->side(orders->current_order_.act_param)->flap_.open();

                orders->prepend(OrderCom_makeFlap(
                    (orders->current_order_.act_param & ACT_SIDE_MASK) | ACT_CONF_CLOSED
                ));
            }
            if (orders->current_order_.act_param & ACT_CONF_CLOSED)
                actuators->side(orders->current_order_.act_param)->flap_.close();
            orders->current_order_.type = ORDER_EXE_TYPE_DELAY;
            orders->current_order_.delay = 0.500;  // todo define
            last_order_executed_timestamp = match->read();
            // is_current_order_executed_ = true;

            break;

        case ORDER_EXE_TYPE_ACT_PROGRADE_DISPENSER:
            if (orders->current_order_.act_param & ACT_CONF_OPEN)
            {
                actuators->prograde_dispenser_.open();
                orders->prepend(OrderCom_makeProgradeDispenser(
                    ACT_CONF_CLOSED
                ));
            }
            if (orders->current_order_.act_param & ACT_CONF_CLOSED)
                actuators->prograde_dispenser_.close();
            orders->current_order_.type = ORDER_EXE_TYPE_DELAY;
            orders->current_order_.delay = 1.000;  // todo define
            last_order_executed_timestamp = match->read();
            // is_current_order_executed_ = true;

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

    Actuators *actuators;

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
    init_finalize(debug, main_timer);

    bool cqb_finished = false;


    /*
        Go!
    */

    main_timer->reset();

    while (true)
    {
        g_mon->main_loop.start_new();
        loop->reset();

        main_do_com(debug, actuators);

        queue->dispatch(0);  // non blocking dispatch
        com_handle_can(debug, messenger, orders, &cqb_finished);

        // equiv MC::updateCurOrder
        // update the goals in function of the given order
        bool is_current_order_executed_ = main_update_cur_order(actuators, orders, main_timer);

        if (cqb_finished && (orders->current_order_.type == ORDER_EXE_TYPE_WAIT_CQB_FINISHED))
        {
            orders->current_order_.type = ORDER_EXE_TYPE_NONE;
            cqb_finished = false;
        }

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
