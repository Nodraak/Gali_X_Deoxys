
#include "mbed.h"

#include "common/Debug.h"
#include "common/Messenger.h"
#include "common/Monitoring.h"
#include "common/OrdersFIFO.h"
#include "common/StatusLeds.h"
#include "common/com.h"
#include "common/init.h"
#include "common/main_sleep.h"
#include "common/parse_cmd.h"
#include "common/sys.h"
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

    Message rec_msg;

    main_timer->reset();
    Timer print_timer;
    print_timer.start();
    while (true)
    {
        g_mon->main_loop.start_new();
        loop->reset();

        parse_cmd(debug, messenger, queue);

        queue->dispatch(0);  // non blocking dispatch

        while (messenger->read_msg(&rec_msg))
        {
#ifdef PRINT_COM_CAN_REC
        debug->printf("[CAN/rec] %d %s\n", rec_msg.id, e2s_message_type(rec_msg.id));
#endif
            if (rec_msg.id == Message::MT_CQB_pong)
            {
                last_ping_CQB = main_timer->read();
                debug->printf("pong cqb\n");
            }
            else if (rec_msg.id == Message::MT_CQES_pong)
            {
                last_ping_CQES = main_timer->read();
                debug->printf("pong cqb\n");
            }
        }

        bool ping_CQB = (main_timer->read()-last_ping_CQB) < 1.000;
        bool ping_CQES = (main_timer->read()-last_ping_CQES) < 1.000;

        if (print_timer.read() > 0.500)
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

            print_timer.reset();
        }

        g_mon->main_loop.stop_and_save();
        main_sleep(debug, loop);
    }

    debug->printf("[CAN] sending we_are_at\n");
    messenger->send_msg_CQR_we_are_at(MC_START_X, MC_START_Y, MC_START_ANGLE);

    /*
        Ready, wait for tirette
    */

    wait_ms(100);

    // todo wait for tirette

    /*
        Go!
    */

    debug->printf("\n\n\n==================== Go! ====================\n\n\n");

    messenger->send_msg_CQR_match_start();

    main_timer->reset();
    while (true)  // todo main_timer->read() < 90
    {
        g_mon->main_loop.start_new();
        loop->reset();

        parse_cmd(debug, messenger, queue);

        queue->dispatch(0);  // non blocking dispatch
        com_handle_serial(debug, messenger);
        com_handle_can(debug, messenger, orders);

        g_mon->main_loop.stop_and_save();
        main_sleep(debug, loop);
    }

    // messenger->send_msg_CQR_match_stop();
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
