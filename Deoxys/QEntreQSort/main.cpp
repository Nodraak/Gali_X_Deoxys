
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
            arms[ARM_LEFT]->ping_all();
            arms[ARM_RIGHT]->ping_all();
        }
        else if (strncmp(buffer, "ls", 2) == 0)
        {
            arms[ARM_LEFT]->seq_init();
            wait(SLEEP_INIT);
            arms[ARM_LEFT]->seq_grab();
            wait(SLEEP_GRAB);
            arms[ARM_LEFT]->seq_move_up();
            wait(SLEEP_MOVE);
            arms[ARM_LEFT]->seq_release();
            wait(SLEEP_RELEASE);
            arms[ARM_LEFT]->seq_move_down();
            wait(SLEEP_MOVE);
        }
        else if (strncmp(buffer, "rs", 2) == 0)
        {
            arms[ARM_RIGHT]->seq_init();
            wait(SLEEP_INIT);
            arms[ARM_RIGHT]->seq_grab();
            wait(SLEEP_GRAB);
            arms[ARM_RIGHT]->seq_move_up();
            wait(SLEEP_MOVE);
            arms[ARM_RIGHT]->seq_release();
            wait(SLEEP_RELEASE);
            arms[ARM_RIGHT]->seq_move_down();
            wait(SLEEP_MOVE);
        }
        else if (strncmp(buffer, "aws", 3) == 0)
        {
            char *ptr = buffer+3+1;
            int a = 0;

            a = atoi(ptr);

            debug->printf("set speed %d\n", a);
            arms[ARM_LEFT]->write_speed_all(a);
            arms[ARM_RIGHT]->write_speed_all(a);
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

            arms[ARM_LEFT]->write_pos_all(posL[0], posL[1], posL[2]);
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

            arms[ARM_RIGHT]->write_pos_all(posR[0], posR[1], posR[2]);
        }
        else if (strncmp(buffer, "arp", 3) == 0)
        {
            arms[ARM_LEFT]->read_pos_all();
            arms[ARM_RIGHT]->read_pos_all();
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
    Debug *debug = NULL;
    CanMessenger *messenger = NULL;
    Timer *loop = NULL;
    Timer match;
    match.start();

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
    debug->printf("t=%f\n", match.read());

    mem_stats_objects(debug);
    mem_stats_settings(debug);
    test_run_all(debug);
    debug->printf("CAN_FRAME_BUS_OCCUPATION %.3f ms\n", CAN_FRAME_BUS_OCCUPATION*1000);
    debug->printf("CAN_MAX_MSG_PER_SEC %.1f\n", CAN_MAX_MSG_PER_SEC);
    debug->printf("CAN_MAX_MSG_PER_200Hz_FRAME %.1f\n", CAN_MAX_MSG_PER_200Hz_FRAME);

    debug->printf("Timer...\n");
    loop = new Timer;
    loop->start();

    debug->printf("OrdersFIFO...\n");
    OrdersFIFO *orders = new OrdersFIFO(ORDERS_COUNT);

    debug->printf("EventQueue...\n");

    EventQueue queue;

    int led_id = queue.call_every(500, callback(&sl, &StatusLeds::running));  // 500 ms

    messenger->on_receive_add(Message::MT_CQB_pong, callback(&sl, &StatusLeds::on_CQB_pong));
    messenger->on_receive_add(Message::MT_CQES_pong, callback(&sl, &StatusLeds::on_CQES_pong));

    debug->printf("AX12_arm...\n");

    AX12 *ax12 = new AX12;
    AX12_arm *arms[3] = {
        NULL,
        new AX12_arm(ax12, ARM_LEFT, 1, 8, 9, AX12_L_PIN_SERVO, AX12_L_PIN_VALVE),
        new AX12_arm(ax12, ARM_RIGHT, 6, 16, 5, AX12_R_PIN_SERVO, AX12_R_PIN_VALVE)
    };
    arms[ARM_LEFT]->write_speed_all(500);
    arms[ARM_RIGHT]->write_speed_all(500);

    debug->printf("interrupt_priorities...\n");
    sys_interrupt_priorities_init(debug);

    mem_stats_dynamic(debug);

    debug->printf("Initialisation done (%f).\n\n", match.read());
    debug->set_current_level(Debug::DEBUG_DEBUG);

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

        main_do_com(debug, arms);

        queue.dispatch(0);  // non blocking dispatch

        com_handle_can(debug, messenger, orders);

        // equiv MC::updateCurOrder
        // update the goals in function of the given order
        bool is_current_order_executed_ = main_update_cur_order(arms, orders, &match);

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
