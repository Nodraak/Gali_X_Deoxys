
#include "mbed.h"

#include "common/StatusLeds.h"
#include "common/Debug.h"
#include "common/Messenger.h"
#include "common/OrdersFIFO.h"
#include "common/mem_stats.h"
#include "common/sys.h"
#include "common/test.h"

#ifdef IAM_QBOUGE
#include "QBouge/MotionController.h"
#include "QBouge/test_mc.h"
#endif
#ifdef IAM_QREFLECHI
#include "demos.h"
#endif
#ifdef IAM_QENTRESORT
#include "QEntreQSort/Actuator.h"
#include "QEntreQSort/RoboticArm.h"
#endif

#include "config.h"

#include "common/init.h"


void init_common(
    Timer **_main_timer,
    StatusLeds **_sl,
    Debug **_debug,
    CanMessenger **_messenger,
    OrdersFIFO **_orders,
    EventQueue **_queue,
    Timer **_loop
)
{
    int ret = 0;

    Timer *main_timer = NULL;
    StatusLeds *sl = NULL;
    Debug *debug = NULL;
    CanMessenger *messenger = NULL;

    OrdersFIFO *orders = NULL;
    EventQueue *queue = NULL;
    Timer *loop = NULL;


    /*
        Basic subsystems
    */

    main_timer = new Timer;
    main_timer->start();

#ifdef IAM_QBOUGE
    sl = new StatusLeds(NC, NC, NC, NC, NC);
#endif
#ifdef IAM_QREFLECHI
    sl = new StatusLeds(A5, A4, NC, A3, A6);
#endif
#ifdef IAM_QENTRESORT
    sl = new StatusLeds(A5, NC, A4, NC, A6);
#endif

    debug = new Debug;
    debug_pre_init(debug);

    debug->printf("Initializing basic subsystems.\n");
    debug->printf("Timer main already initialized.\n");
    debug->printf("StatusLeds already initialized.\n");
    debug->printf("Debug already initialized.\n");
    debug->printf("t1=%.3f\n", main_timer->read());

    debug->printf("CanMessenger...\n");
    messenger = new CanMessenger;
    ret = 0;
    ret += messenger->on_receive_add(Message::MT_CQB_pong, callback(sl, &StatusLeds::on_CQB_pong));
    ret += messenger->on_receive_add(Message::MT_CQR_pong, callback(sl, &StatusLeds::on_CQR_pong));
    ret += messenger->on_receive_add(Message::MT_CQES_pong, callback(sl, &StatusLeds::on_CQES_pong));
    if (ret)
    {
        debug->printf("Error: CanMessenger::on_receive_add(). (ret=%d)\n", ret);
        while (1)
            ;
    }
    debug->printf("\tok.\n");

    debug->printf("Basic subsystems initialized successfully.\n");
    debug->printf("t2=%.3f\n", main_timer->read());
    debug->printf("Initializing remaining subsystems.\n");
    sl->init_half();

    mem_stats_objects(debug);
    mem_stats_settings(debug);
    test_run_all(debug);

    debug->printf("Interrupt Priorities...\n");
    sys_interrupt_priorities_init();
    debug->printf("\tok.\n");

    debug->printf("OrdersFIFO...\n");
    orders = new OrdersFIFO(ORDERS_COUNT);
    debug->printf("\tok.\n");

    debug->printf("EventQueue...\n");
    queue = new EventQueue;
    // todo define led blink + send ping (500 ms)
    queue->call_every(500, callback(sl, &StatusLeds::running));
    queue->call_every(1000/50, callback(sl, &StatusLeds::updateBO));
#ifdef IAM_QREFLECHI
    queue->call_every(500, callback(messenger, &CanMessenger::send_msg_CQR_ping));
#endif
    queue->call_every(500, callback(messenger, &CanMessenger::leave_the_bus_for_a_moment));
    debug->printf("\tok.\n");

    debug->printf("Timer loop...\n");
    loop = new Timer;
    loop->start();
    debug->printf("\tok.\n");

    debug->printf("Remaining subsystems initialized successfully.\n");
    debug->printf("t3=%.3f\n", main_timer->read());
    debug->printf("Initializing board-specific subsystems.\n");

    *_main_timer = main_timer;
    *_sl = sl;
    *_debug = debug;
    *_messenger = messenger;
    *_orders = orders;
    *_queue = queue;
    *_loop = loop;
}

#ifdef IAM_QBOUGE
void init_board_CQB(Debug *debug,
    MotionController **_mc
)
{
    MotionController *mc = NULL;
    Ticker *asserv_ticker = NULL;

    debug->printf("MotionController...\n");
    mc = new MotionController;
    // move in MC ?
    asserv_ticker = new Ticker;  // ISR. This uses the TIMER2 (TIM2_IRQn)
    asserv_ticker->attach(callback(mc, &MotionController::asserv), ASSERV_DELAY);
    debug->printf("\tok.\n");

    *_mc = mc;
}
#endif

#ifdef IAM_QREFLECHI
void init_board_CQR(Debug *debug,
    OrdersFIFO *orders
)
{
    int ret = 0;

    debug->printf("OrdersFIFO (demo_init)...\n");
    // ret += demo_load(demo_table, orders);
    // ret += demo_load(demo_test_enc_turn, orders);
    ret += demo_load(demo_table_arm, orders);
    if (ret != 0)
    {
        debug->printf("Error: demo_init()\n");
        while (1)
            ;
    }
    debug->printf("\tok.\n");
}
#endif

#ifdef IAM_QENTRESORT
void init_board_CQES(
    Debug *debug,
    AX12_arm ***_arms
)
{
    AX12 *ax12 = NULL;
    AX12_arm *arms[3] = {NULL, NULL, NULL};

    debug->printf("AX12_arm...\n");
    ax12 = new AX12;
    arms[0] = NULL;
    arms[1] = new AX12_arm(ax12, ACT_SIDE_LEFT, 1, 8, 9, AX12_L_PIN_SERVO, AX12_L_PIN_VALVE);
    arms[2] = new AX12_arm(ax12, ACT_SIDE_RIGHT, 6, 16, 5, AX12_R_PIN_SERVO, AX12_R_PIN_VALVE);
    arms[ACT_SIDE_LEFT]->write_speed_all(500);
    arms[ACT_SIDE_RIGHT]->write_speed_all(500);
    debug->printf("\tok.\n");

    *_arms = arms;
}
#endif

void init_finalize(Debug *debug, Timer *main_timer)
{
    debug->printf("Board-specific subsystems initialized successfully.\n");
    debug->printf("t4=%.3f\n", main_timer->read());

    mem_stats_dynamic(debug);

    debug->printf("Initialization successfull.\n");
    debug->printf("t5=%.3f\n", main_timer->read());

    debug->printf("Waiting until %.3f sec...\n", INIT_FINALIZE_WAIT_UNTIL);
    while (main_timer->read() < INIT_FINALIZE_WAIT_UNTIL)
        wait_ms(1);
    debug->printf("\tok.\n");
    debug->printf("t6=%.3f\n", main_timer->read());

    debug->set_current_level(Debug::DEBUG_DEBUG);
}
