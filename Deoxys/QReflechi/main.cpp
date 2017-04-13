
#include "mbed.h"

#include "common/Debug.h"
#include "common/Messenger.h"
#include "common/OrdersFIFO.h"
#include "common/StatusLeds.h"
#include "common/com.h"
#include "common/main_sleep.h"
#include "common/mem_stats.h"
#include "common/utils.h"
#include "common/sys.h"

#include "common/test.h"

#include "demos.h"
#include "config.h"
#include "pinout.h"


int main(void)
{
    Debug *debug = NULL;
    Timer *loop = NULL;
    Timer match;
    match.start();

    /*
        Initializing
    */

    StatusLeds sl(A5, A4, NC, A3);

    debug = new Debug;
    debug_pre_init(debug);

    debug->printf("Initializing\n");

    debug->printf("CanMessenger...\n");
    CanMessenger *messenger = new CanMessenger;

    sl.init_half();
    debug->printf("t=%f\n", match.read());

    mem_stats_objects(debug);
    mem_stats_settings(debug);
    test_run_all(debug);
    debug->printf("CAN_FRAME_BUS_OCCUPATION %.3f ms\n", CAN_FRAME_BUS_OCCUPATION*1000);
    debug->printf("CAN_MAX_MSG_PER_SEC %.1f\n", CAN_MAX_MSG_PER_SEC);
    debug->printf("CAN_MAX_MSG_PER_200Hz_FRAME %.1f\n", CAN_MAX_MSG_PER_200Hz_FRAME);

    debug->printf("Timer (loop)...\n");
    loop = new Timer;
    loop->start();

    debug->printf("demo_init ...\n");
    OrdersFIFO *orders = demo_init();
    if (orders == NULL)
    {
        debug->printf("ERROR demo_init()\n");
        while (1)
            ;
    }

    // init sharp + other sensors
    // init servos + other actuators

    // init ia ?

    debug->printf("interrupt_priorities...\n");
    sys_interrupt_priorities_init();

    mem_stats_dynamic(debug);

    debug->printf("Initialisation done (%f).\n\n", match.read());
    debug->set_current_level(Debug::DEBUG_DEBUG);

    // wait for other boards to be alive
    debug->printf("Waiting for other boards...\n");

    float last_ping_CQB = -1;
    float last_ping_CQES = -1;

    messenger->on_receive_add(Message::MT_CQB_pong, callback(&sl, &StatusLeds::on_CQB_pong));
    messenger->on_receive_add(Message::MT_CQES_pong, callback(&sl, &StatusLeds::on_CQES_pong));

    EventQueue queue;
    int led_id = queue.call_every(500, callback(&sl, &StatusLeds::running));  // 500 ms
    int ping_id = queue.call_every(500, callback(messenger, &CanMessenger::send_msg_ping));  // 500 ms

    Message rec_msg;
    while (true)
    {
        debug->printf("[CAN] ping...\n");

        queue.dispatch(0);  // non blocking dispatch

        while (messenger->read_msg(&rec_msg))
        {
            debug->printf("[CAN/rec] id=%d\n", rec_msg.id);

            if (rec_msg.id == Message::MT_CQB_pong)
                last_ping_CQB = match.read();
            if (rec_msg.id == Message::MT_CQES_pong)
                last_ping_CQES = match.read();
        }

        bool ping_CQB = (match.read()-last_ping_CQB) < 1.000;
        bool ping_CQES = (match.read()-last_ping_CQES) < 1.000;

        if (ping_CQB && ping_CQES)
        {
            debug->printf("[CAN/rec] pong from all, breaking\n");
            break;
        }
        else
        {
            debug->printf("[CAN] alive status : CQB=%d CQES=%d\n", ping_CQB, ping_CQES);
        }

        Thread::wait(100);  // ms - dont flood the can bus and wait a litle for the reply
    }

    debug->printf("[CAN] sending reset + we_are_at\n");
    messenger->send_msg_CQR_reset();
    messenger->send_msg_we_are_at(MC_START_X, MC_START_Y, MC_START_ANGLE);

    wait_ms(200);

    /*
        Ready, wait for tirette
    */

    // todo wait for tirette

    /*
        Go!
    */

    debug->printf("\nGo!\n");

    match.reset();
    messenger->send_msg_CQR_match_start();

    while (true)  // todo match.read() < 90
    {
        loop->reset();
        debug->printf("[timer/match] %.3f\n", match.read());

        // todo ping/pong each board -> if no response since XX, then do something

        // if (match->read_ms() > 90*1000)  // todo define
        //     messenger->send_msg_CQR_match_stop();

        queue.dispatch(0);  // non blocking dispatch

        com_handle_serial(debug, messenger);
        com_handle_can(debug, messenger, orders);

        main_sleep(debug, loop);
    }

    // todo stop motors over can
    // todo funny action

    /*
        Cleanup
    */

    debug->set_current_level(Debug::DEBUG_INITIALISATION);
    debug->printf("Cleaning...\n");

    delete orders;
    delete loop;
    delete messenger;
    delete debug;

    return 0;
}
