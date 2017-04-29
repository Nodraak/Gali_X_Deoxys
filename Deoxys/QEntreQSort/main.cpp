
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
#include "QEntreQSort/Actuator.h"
#include "QEntreQSort/RoboticArm.h"

#include "config.h"
#include "pinout.h"


float last_order_executed_timestamp = -1;


void main_do_com(Debug *debug, AX12_arm **arms)
{
    char buffer[BUFFER_SIZE] = "";

    static int posL[] = {500, 500, 500};
    static int posR[] = {500, 500, 500};

#if 1
    if (debug->get_line(buffer, BUFFER_SIZE) != -1)
    {
        /*
            p: ping
            ls: left seq
            rs: right seq
            aws: a write speed
            lwp: left write pos
            rwp: right write pos
            arp: all read pos
        */

#if 0
        if (strncmp(buffer, "pingid", 6) == 0)
        {
            char *ptr = buffer+6+1;
            int a = 0;

            a = atoi(ptr);

            debug->printf("ping id %d\n", a);
            // ax12.send_ping(a);
        }
        else if (strncmp(buffer, "setid", 5) == 0)
        {
            char *ptr = buffer+5+1;
            int a = 0, b = 0;

            a = atoi(ptr);
            while (*ptr != ' ')
                ++ptr;
            ++ptr;
            b = atoi(ptr);

            debug->printf("set id %d -> %d\n", a, b);
            // ax12.write_id(a, b);
        }
        else if (strncmp(buffer, "testid", 6) == 0)
        {
            char *ptr = buffer+6+1;
            int a = 0;

            a = atoi(ptr);

            debug->printf("test id %d\n", a);
            // ax12.write_pos(a, 500);
            wait_ms(500);
            // ax12.write_pos(a, 100);
            wait_ms(500);
            // ax12.write_pos(a, 1000);
            wait_ms(500);
        }
        else if (strncmp(buffer, "writebaud", 9) == 0)
        {
            char *ptr = buffer+9+1;
            int a = 0, b = 0;

            a = atoi(ptr);
            while (*ptr != ' ')
                ++ptr;
            ++ptr;
            b = atoi(ptr);

            debug->printf("write baud %d -> %d\n", a, b);
            // ax12.write_baud_rate(a, b);
        }
        else if (strncmp(buffer, "setbaud", 7) == 0)
        {
            char *ptr = buffer+7+1;
            int a = 0;

            a = atoi(ptr);

            debug->printf("set baud %d\n", a);
            // ax12.set_baud(a);
        }
        else if (strncmp(buffer, "writepos", 8) == 0)
        {
            char *ptr = buffer+8+1;
            int a = 0, b = 0;

            a = atoi(ptr);
            while (*ptr != ' ')
                ++ptr;
            ++ptr;
            b = atoi(ptr);

            debug->printf("write pos %d -> %d\n", a, b);
            // ax12.write_pos(a, b);
        }
        else if (strncmp(buffer, "rd", 2) == 0)
        {
            char *ptr = buffer+2+1;
            int a = 0;

            a = atoi(ptr);

            debug->printf("read delay %d\n", a);
            // debug->printf("-> %d\n", ax12.read_delay(a));
        }
        else if (strncmp(buffer, "wd", 2) == 0)
        {
            char *ptr = buffer+2+1;
            int a = 0, b = 0;

            a = atoi(ptr);
            while (*ptr != ' ')
                ++ptr;
            ++ptr;
            b = atoi(ptr);
            debug->printf("write delay %d -> %d\n", a, b);
            // ax12.write_delay(a, b);
        }
        else
#endif
        if (strncmp(buffer, "p", 1) == 0)
        {
            arms[ACT_SIDE_LEFT]->ping_all();
            arms[ACT_SIDE_RIGHT]->ping_all();
        }
        else if (strncmp(buffer, "ls", 2) == 0)
        {
            arms[ACT_SIDE_LEFT]->seq_init();
            wait(SLEEP_INIT);
            arms[ACT_SIDE_LEFT]->seq_grab();
            wait(SLEEP_GRAB);
            arms[ACT_SIDE_LEFT]->seq_move_up();
            wait(SLEEP_MOVE);
            arms[ACT_SIDE_LEFT]->seq_release();
            wait(SLEEP_RELEASE);
            arms[ACT_SIDE_LEFT]->seq_move_down();
            wait(SLEEP_MOVE);
        }
        else if (strncmp(buffer, "rs", 2) == 0)
        {
            arms[ACT_SIDE_RIGHT]->seq_init();
            wait(SLEEP_INIT);
            arms[ACT_SIDE_RIGHT]->seq_grab();
            wait(SLEEP_GRAB);
            arms[ACT_SIDE_RIGHT]->seq_move_up();
            wait(SLEEP_MOVE);
            arms[ACT_SIDE_RIGHT]->seq_release();
            wait(SLEEP_RELEASE);
            arms[ACT_SIDE_RIGHT]->seq_move_down();
            wait(SLEEP_MOVE);
        }
        else if (strncmp(buffer, "aws", 3) == 0)
        {
            char *ptr = buffer+3+1;
            int a = 0;

            a = atoi(ptr);

            debug->printf("set speed %d\n", a);
            arms[ACT_SIDE_LEFT]->write_speed_all(a);
            arms[ACT_SIDE_RIGHT]->write_speed_all(a);
        }
        else if (strncmp(buffer, "lwp", 3) == 0)
        {
            char *ptr = buffer+3+1;
            int a = 0, b = 0;

            a = atoi(ptr);
            while (*ptr != ' ')
                ++ptr;
            ++ptr;
            b = atoi(ptr);

            posL[a] = b;
            debug->printf("have pos L %d %d %d\n", posL[0], posL[1], posL[2]);

            arms[ACT_SIDE_LEFT]->write_pos_all(posL[0], posL[1], posL[2]);
        }
        else if (strncmp(buffer, "rwp", 3) == 0)
        {
            char *ptr = buffer+3+1;
            int a = 0, b = 0;

            a = atoi(ptr);
            while (*ptr != ' ')
                ++ptr;
            ++ptr;
            b = atoi(ptr);

            posR[a] = b;
            debug->printf("have pos R %d %d %d\n", posR[0], posR[1], posR[2]);

            arms[ACT_SIDE_RIGHT]->write_pos_all(posR[0], posR[1], posR[2]);
        }
        else if (strncmp(buffer, "arp", 3) == 0)
        {
            arms[ACT_SIDE_LEFT]->read_pos_all();
            arms[ACT_SIDE_RIGHT]->read_pos_all();
        }
        else
            debug->printf("unknown cmd\n");
    }
#endif
}

bool main_update_cur_order(AX12_arm **arms, OrdersFIFO *orders, Timer *match)
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
        case ORDER_EXE_TYPE_WAIT_CQES_FINISHED:
        case ORDER_EXE_TYPE_POS:
        case ORDER_EXE_TYPE_ANGLE:
            // ignore on CQES
            break;

        case ORDER_EXE_TYPE_ARM_INIT:
            arms[orders->current_order_.which_arm]->seq_init();
            orders->current_order_.type = ORDER_EXE_TYPE_DELAY;
            orders->current_order_.delay = SLEEP_INIT;
            last_order_executed_timestamp = match->read();
            // is_current_order_executed_ = true;
            break;
        case ORDER_EXE_TYPE_ARM_GRAB:
            arms[orders->current_order_.which_arm]->seq_grab();
            orders->current_order_.type = ORDER_EXE_TYPE_DELAY;
            orders->current_order_.delay = SLEEP_GRAB;
            last_order_executed_timestamp = match->read();
            // is_current_order_executed_ = true;
            break;
        case ORDER_EXE_TYPE_ARM_MOVE_UP:
            arms[orders->current_order_.which_arm]->seq_move_up();
            orders->current_order_.type = ORDER_EXE_TYPE_DELAY;
            orders->current_order_.delay = SLEEP_MOVE;
            last_order_executed_timestamp = match->read();
            // is_current_order_executed_ = true;
            break;
        case ORDER_EXE_TYPE_ARM_RELEASE:
            arms[orders->current_order_.which_arm]->seq_release();
            orders->current_order_.type = ORDER_EXE_TYPE_DELAY;
            orders->current_order_.delay = SLEEP_RELEASE;
            last_order_executed_timestamp = match->read();
            // is_current_order_executed_ = true;
            break;
        case ORDER_EXE_TYPE_ARM_MOVE_DOWN:
            arms[orders->current_order_.which_arm]->seq_move_down();
            orders->current_order_.type = ORDER_EXE_TYPE_DELAY;
            orders->current_order_.delay = SLEEP_MOVE;
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

    AX12_arm **arms;

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
        &arms
    );
    init_finalize(debug, main_timer);

    bool cqb_finished = false;

arms[ACT_SIDE_RIGHT]->seq_move_down();

    /*
        Go!
    */

    main_timer->reset();

    while (true)
    {
        g_mon->main_loop.start_new();
        loop->reset();

        main_do_com(debug, arms);

        queue->dispatch(0);  // non blocking dispatch
        com_handle_can(debug, messenger, orders, &cqb_finished);

        // equiv MC::updateCurOrder
        // update the goals in function of the given order
        bool is_current_order_executed_ = main_update_cur_order(arms, orders, main_timer);

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
