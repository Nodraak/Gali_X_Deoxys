
#include "mbed.h"

#include "common/Debug.h"
#include "common/Messenger.h"
#include "common/OrdersFIFO.h"
#include "common/com.h"
#include "common/main_sleep.h"
#include "common/mem_stats.h"
#include "common/utils.h"
#include "common/sys.h"
#include "QBouge/MotionController.h"

#include "common/test.h"
#include "QBouge/test_mc.h"

#include "config.h"
#include "pinout.h"


MotionController *mc = NULL;
bool request_next_order = false;
void asserv_main(void);


PwmOut buzzer_(BUZZER_PIN);

void bip(void)
{
    buzzer_.period(1./4000);
    buzzer_.write(0.50);
    wait_ms(200);

    buzzer_.period_us(1);
    wait_ms(200);

    buzzer_.period(1./4000);
    buzzer_.write(0.50);
    wait_ms(200);

    buzzer_.period_us(1);
}


int demo_load(MotionController *mc, s_order_com *demo, int demo_size)
{
    int e = 0, i = 0;

    for (i = 0; i < demo_size; ++i)
        e += mc->orders_->push(demo[i]);

    return e;
}


int main(void)
{
    Debug *debug = NULL;
    CanMessenger *messenger = NULL;
    Timer *loop = NULL;
    Ticker *asserv_ticker = NULL;

    PwmOut *ml = new PwmOut(MOTOR_L_PWM);
    ml->period(0.001 * 0.05);
    ml->write(0);
    PwmOut *mr = new PwmOut(MOTOR_R_PWM);
    mr->period(0.001 * 0.05);
    mr->write(0);

    buzzer_.period(1./2000);
    buzzer_.write(0.50);
    wait_ms(200);
    buzzer_.period(1./3000);
    buzzer_.write(0.50);
    wait_ms(200);
    buzzer_.period(1./4000);
    buzzer_.write(0.50);
    wait_ms(400);

    /*
        Initializing
    */

    debug = new Debug;
    debug_pre_init(debug);

    debug->printf("Initializing\n");

    debug->printf("CanMessenger...\n");
    messenger = new CanMessenger;

    mem_stats_objects(debug);
    mem_stats_settings(debug);
    test_run_all(debug);

    debug->printf("Timer...\n");
    loop = new Timer;
    loop->start();

    debug->printf("MotionController...\n");
    mc = new MotionController;

    debug->printf("Ticker...\n");
    asserv_ticker = new Ticker;
    asserv_ticker->attach(mc, &MotionController::asserv, ASSERV_DELAY);

    debug->printf("interrupt_priorities...\n");
    sys_interrupt_priorities_init(debug);

    mem_stats_dynamic(debug);

    debug->printf("Initialisation done.\n\n");
    debug->set_current_level(Debug::DEBUG_DEBUG);

    bip();

    wait(1);

    delete ml;
    delete mr;
    // todo wait for tirette (can msg)

    /*
        Go!
    */

    Timer match;
    match.start();

    while (true)
    {
        loop->reset();
        debug->printf("[timer/match] %.3f\n", match.read());

        com_handle_can(debug, messenger, mc);

        if (mc->should_request_next_order(debug))
        {
            debug->printf("[CAN] send next_order_request\n");
            messenger->send_msg_CQB_next_order_request(ORDERS_COUNT - mc->orders_->size());
        }

        if (mc->should_send_can_bus_sleeping())
        {
            messenger->send_msg_CQB_sleeping_a_bit();
        }

        mc->debug(debug);
        mc->debug(messenger);

        main_sleep(debug, loop);
    }

    /*
        Cleanup
    */

    debug->set_current_level(Debug::DEBUG_INITIALISATION);
    debug->printf("Cleaning...\n");

    asserv_ticker->detach();
    delete asserv_ticker;
    delete loop;
    delete messenger;
    delete mc;
    delete debug;

    return 0;
}
