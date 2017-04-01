
#include "mbed.h"

#include "common/Debug.h"
#include "common/Messenger.h"
#include "common/OrdersFIFO.h"
#include "common/com.h"
#include "common/main_sleep.h"
#include "common/mem_stats.h"
#include "common/utils.h"

#include "common/test.h"

#include "demos.h"
#include "config.h"
#include "pinout.h"


DigitalOut led_status(A5);
DigitalOut led_ping_CQB(A4);
DigitalOut led_ping_CQES(A3);


int main(void)
{
    Debug *debug = NULL;
    Timer *loop = NULL, *match = NULL;

    /*
        Initializing
    */

    led_status = 1;
    led_ping_CQB = 0;
    led_ping_CQES = 0;

    debug = new Debug;
    debug_pre_init(debug);

    debug->printf("Initializing\n");

    debug->printf("CanMessenger...\n");
    CanMessenger *messenger = new CanMessenger;

    led_status = 0;

    mem_stats_objects(debug);
    mem_stats_settings(debug);
    test_run_all(debug);

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

    debug->printf("Timer (match)...\n");
    match = new Timer;
    match->start();

    // init sharp + other sensors
    // init servos + other actuators

    // init ia ?

    mem_stats_dynamic(debug);

    debug->printf("Initialisation done.\n\n");
    debug->set_current_level(Debug::DEBUG_DEBUG);


#define ASSERV_FPS 200
    debug->printf("CAN_FRAME_BUS_OCCUPATION %.3f ms\n", CAN_FRAME_BUS_OCCUPATION*1000);
    debug->printf("CAN_MAX_MSG_PER_SEC %.1f\n", CAN_MAX_MSG_PER_SEC);
    debug->printf("CAN_MAX_MSG_PER_ASSERV_FRAME %.1f\n", CAN_MAX_MSG_PER_ASSERV_FRAME);

    // wait for other boards to be alive
    debug->printf("Waiting for other boards...\n");

    float last_ping_CQB = -1;
    float last_ping_CQES = -1;

    Message rec_msg;
    while (true)
    {
        led_status = ((int)match->read()) % 2;

        debug->printf("[CAN] ping...\n");

        messenger->send_msg_ping();

        wait_ms(250);  // dont flood the can bus and wait a litle for the reply

        while (messenger->read_msg(&rec_msg))
        {
            debug->printf("[CAN/rec] id=%d\n", rec_msg.id);

            if (rec_msg.id == Message::MT_CQB_pong)
                last_ping_CQB = match->read();
            if (rec_msg.id == Message::MT_CQES_pong)
                last_ping_CQES = match->read();
        }

        bool ping_CQB = (match->read()-last_ping_CQB) < 0.500;
        bool ping_CQES = (match->read()-last_ping_CQES) < 0.500;

        led_ping_CQB = ping_CQB;
        led_ping_CQES = ping_CQES;

        if (ping_CQB && ping_CQES)
        {
            debug->printf("[CAN/rec] pong from all, breaking\n");
            break;
        }
        else
        {
            debug->printf("[CAN] alive status : CQB=%d CQES=%d\n", ping_CQB, ping_CQES);
        }

        messenger->set_silent(true);
        wait_ms(10);

        // 1/(200*1000) * (128*11)  bus off recovery time (let other boards to initialise)
        messenger->set_silent(false);
    }

    debug->printf("[CAN] sending reset + we_are_at\n");
    messenger->send_msg_CQR_reset();
    messenger->send_msg_we_are_at(MC_START_X, MC_START_Y, MC_START_ANGLE);

    wait_ms(200);

    led_ping_CQB = 1;
    led_ping_CQES = 1;

    /*
        Ready, wait for tirette
    */

    // todo wait for other boards -> ping msg over can -> or wait to be in while to find out | + setup isr to ping ?
    // todo wait for tirette

    /*
        Go!
    */

    debug->printf("\nGo!\n");

    match->reset();
    messenger->send_msg_CQR_match_start();

    while (true)  // todo match.read() < 90
    {
        loop->reset();
        debug->printf("[timer/match] %.3f\n", match->read());

        led_status = ((int)match->read()) % 2;

        // todo ping/pong each board -> if no response since XX, then do something

        if (match->read_ms() > 90*1000)  // todo define
            messenger->send_msg_CQR_match_stop();

        // update sharp + other sensors

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

    delete match;
    delete orders;
    delete loop;
    delete messenger;
    delete debug;

    return 0;
}
