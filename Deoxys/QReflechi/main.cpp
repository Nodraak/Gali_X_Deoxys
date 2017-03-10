
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


int main(void)
{
    Debug *debug = NULL;
    Timer *loop = NULL, *match = NULL;

    /*
        Initializing
    */

    PwmOut buzzer_(PC_8);

    buzzer_.period(1./4000);
    buzzer_.write(0.50);
    wait_ms(200);
    buzzer_.period(1./3000);
    buzzer_.write(0.50);
    wait_ms(200);
    buzzer_.period(1./2000);
    buzzer_.write(0.50);
    wait_ms(400);
    buzzer_.period_us(1);

    debug = new Debug;

    debug->printf("Initializing\n");

    debug->printf("CanMessenger...\n");
    CanMessenger *messenger = new CanMessenger;

    mem_stats_objects(debug);
    mem_stats_settings(debug);
    test_run_all(debug);

    debug->printf("Timer (loop)...\n");
    loop = new Timer;
    loop->start();

    debug->printf("OrdersFIFO...\n");
    OrdersFIFO *orders = new OrdersFIFO(ORDERS_COUNT);

    debug->printf("Timer (match)...\n");
    match = new Timer;
    match->start();

    debug->printf("demo_load()...\n");
    int ret = 0;
    ret = demo_load(orders, demo_table, DEMO_TABLE_SIZE);
    if (ret != 0)
    {
        debug->printf("ERROR when filling OrdersFIFO (%d)\n", ret);
        while (1)
            ;
    }

    // init sharp + other sensors
    // init servos + other actuators

    // init ia ?

    mem_stats_dynamic(debug);

    debug->printf("Initialisation done.\n\n");
    debug->set_current_level(Debug::DEBUG_DEBUG);


    // wait for other boards to be alive
    debug->printf("Waiting for other boards...\n");

    float last_ping_CQB = -1;
    float last_ping_CQES = -1;

    Message rec_msg;
    while (true)
    {
        debug->printf("[CAN] ping...\n");

        messenger->send_msg_ping();

        wait_ms(100);  // dont flood the can bus and wait a litle for the reply

        while (messenger->read_msg(&rec_msg))
        {
            if (rec_msg.id == Message::MT_CQB_pong)
                last_ping_CQB = match->read();
            if (rec_msg.id == Message::MT_CQES_pong)
                last_ping_CQES = match->read();
        }

        if ((match->read()-last_ping_CQB < 0.500) && (match->read()-last_ping_CQES < 0.500))
        {
            debug->printf("[CAN/rec] pong from all, breaking\n");
            break;
        }
        else
        {
            debug->printf("[CAN] alive status : CQB=%d CQES=%d\n", match->read()-last_ping_CQB < 0.500, match->read()-last_ping_CQES < 0.500);
        }

        messenger->set_silent(true);
        wait_ms(5);
        messenger->set_silent(false);
    }

    debug->printf("[CAN] sending reset + we_are_at\n");
    messenger->send_msg_CQR_reset();
    messenger->send_msg_CQR_we_are_at(MC_START_X, MC_START_Y, MC_START_ANGLE);

    wait_ms(200);

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
