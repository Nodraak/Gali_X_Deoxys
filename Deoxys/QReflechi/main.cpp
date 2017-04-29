
#include "mbed.h"

#include "common/Debug.h"
#include "common/Messenger.h"
#include "common/OrdersFIFO.h"
#include "common/StatusLeds.h"
#include "common/com.h"
#include "common/init.h"
#include "common/main_sleep.h"
#include "common/utils.h"

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

    init_common(
        &main_timer,
        &sl,
        &debug,
        &messenger,
        &orders,
        &queue,
        &loop
    );
    init_board_CQR(debug, orders);
    init_finalize(debug, main_timer);

    // wait for other boards to be alive
    debug->printf("Waiting for other boards...\n");

    float last_ping_CQB = -1;
    float last_ping_CQES = -1;

    loop->reset();

    Message rec_msg;
    while (true)
    {
        queue->dispatch(0);  // non blocking dispatch

        while (messenger->read_msg(&rec_msg))
        {
            debug->printf("[CAN/rec] id=%d\n", rec_msg.id);

            if (rec_msg.id == Message::MT_CQB_pong)
            {
                last_ping_CQB = main_timer->read();
                debug->printf("pong cqb\n");
            }
            if (rec_msg.id == Message::MT_CQES_pong)
            {
                last_ping_CQES = main_timer->read();
                debug->printf("pong cqb\n");
            }
        }

        bool ping_CQB = (main_timer->read()-last_ping_CQB) < 1.000;
        bool ping_CQES = (main_timer->read()-last_ping_CQES) < 1.000;

        if (loop->read() > 0.500)
        {
            if (ping_CQB && ping_CQES)
            {
                debug->printf("[CAN/rec] pong from all, breaking\n");
                break;
            }
            else
            {
                debug->printf("[CAN] alive status : CQB=%d CQES=%d\n", ping_CQB, ping_CQES);
            }

            loop->reset();
        }

        Thread::wait(1000/100);  // ms
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

    main_timer->reset();
    messenger->send_msg_CQR_match_start();

    while (true)  // todo main_timer->read() < 90
    {
        loop->reset();
        debug->printf("[timer/match] %.3f\n", main_timer->read());

        // todo ping/pong each board -> if no response since XX, then do something

        // if (main_timer->read_ms() > 90*1000)  // todo define
        //     messenger->send_msg_CQR_match_stop();

        queue->dispatch(0);  // non blocking dispatch

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
