
#include "mbed.h"

#include "common/StatusLeds.h"
#include "common/Debug.h"
#include "common/Messenger.h"
#include "common/Monitoring.h"
#include "common/OrdersFIFO.h"
#include "common/main_sleep.h"
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
#include "QEntreQSort/Actuators.h"
#endif

#include "config.h"
#include "pinout.h"

#include "common/init.h"

#ifdef IAM_QENTRESORT
extern Ax12Driver *ax12_driver;
#endif

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

    sl = new StatusLeds(LED_RUNNING, LED_PONG_CQB, LED_PONG_CQR, LED_PONG_CQES, LED_BUS_OFF);

    debug = new Debug;
    g_debug = debug;
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

    debug->printf("Interrupt Priorities...\n");
    sys_interrupt_priorities_init();
    debug->printf("\tok.\n");

    debug->printf("OrdersFIFO...\n");
    orders = new OrdersFIFO(ORDERS_COUNT);
    debug->printf("\tok.\n");

    debug->printf("EventQueue...\n");
    queue = new EventQueue;
    queue->call_every(INIT_DELAY_LED_RUNNING, callback(sl, &StatusLeds::running));
    queue->call_every(INIT_DELAY_LED_UPDATE_BO, callback(sl, &StatusLeds::updateBO));
#ifdef PRINT_TIME
    queue->call_every(INIT_DELAY_PRINT_TIME, callback(main_print_time), debug, main_timer);
#endif
#ifdef PRINT_SYS_CAN_STATS
    queue->call_every(INIT_DELAY_DEBUG_SYS_CAN, sys_debug_can);
#endif
    debug->printf("\tok.\n");

    debug->printf("Timer loop...\n");
    loop = new Timer;
    loop->start();
    debug->printf("\tok.\n");

    debug->printf("Monitoring...\n");
    g_mon = new Monitoring;
    queue->call_every(INIT_DELAY_MONITORING_RESET, callback(g_mon, &Monitoring::reset));
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
    EventQueue *queue,
    CanMessenger *messenger,
    MotionController **_mc
)
{
    MotionController *mc = NULL;

    debug->printf("MotionController...\n");
    mc = new MotionController;
    debug->printf("\tok.\n");

    debug->printf("Debug MotionController via EventQueue...\n");
    queue->call_every(INIT_DELAY_DEBUG_MC_SERIAL, callback(mc, &MotionController::debug_serial), debug);
    queue->call_every(INIT_DELAY_DEBUG_MC_CAN, callback(mc, &MotionController::debug_can), messenger);
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
    // ret += demo_load(demo_basic_test, orders);
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
void init_board_CQES(Debug *debug,
    EventQueue *queue,
    Actuators **_actuators
)
{
    Ax12Driver *ax12 = NULL;
    Actuators *actuators = NULL;

    debug->printf("Ax12Driver...\n");
    ax12 = new Ax12Driver;
    ax12_driver = ax12;
    debug->printf("\tok\n");

// todo define ax12 id ?
    debug->printf("Actuators...\n");
    actuators = new Actuators(
        // left
        OneSideCylindersActuators("left",
            ArmActuator(
                Ax12Actuator("height", ax12,  6, ACT_L_ARM_HEIGHT_R, ACT_L_ARM_HEIGHT_N, ACT_L_ARM_HEIGHT_E),
                Ax12Actuator("vert",   ax12, 16, ACT_L_ARM_VERT_R,   ACT_L_ARM_VERT_N,   ACT_L_ARM_VERT_E),
                Ax12Actuator("horiz",  ax12,  5, ACT_L_ARM_HORIZ_R,  ACT_L_ARM_HORIZ_N,  ACT_L_ARM_HORIZ_E),
                ServoActuator("clamp",  PwmOut(ACT_L_CLAMP),    ACT_L_ARM_CLAMP_R, ACT_L_ARM_CLAMP_E),
                BooleanActuator("pump", DigitalOut(ACT_L_PUMP), ACT_L_ARM_PUMP_R,  ACT_L_ARM_PUMP_E)
            ),
            ServoActuator("flap", PwmOut(ACT_L_FLAP), ACT_L_FLAP_R, ACT_L_FLAP_E),
            ServoActuator("prog_disp", PwmOut(ACT_L_PROG), ACT_L_PROG_R, ACT_L_PROG_E),
            CylinderRotationSystem(ax12_driver, queue, COLOR_L_GREEN, COLOR_L_BLUE, COLOR_L_AX12_ID, COLOR_L_REVERSED)
        ),
        // right
        OneSideCylindersActuators("right",
            ArmActuator(
                Ax12Actuator("height", ax12, 10, ACT_R_ARM_HEIGHT_R, ACT_R_ARM_HEIGHT_N, ACT_R_ARM_HEIGHT_E),
                Ax12Actuator("vert",   ax12, 8, ACT_R_ARM_VERT_R,   ACT_R_ARM_VERT_N,   ACT_R_ARM_VERT_E),
                Ax12Actuator("horiz",  ax12, 9, ACT_R_ARM_HORIZ_R,  ACT_R_ARM_HORIZ_N,  ACT_R_ARM_HORIZ_E),
                ServoActuator("clamp",  PwmOut(ACT_R_CLAMP),    ACT_R_ARM_CLAMP_R, ACT_R_ARM_CLAMP_E),
                BooleanActuator("pump", DigitalOut(ACT_R_PUMP), ACT_R_ARM_PUMP_R,  ACT_R_ARM_PUMP_E)
            ),
            ServoActuator("flap", PwmOut(ACT_R_FLAP), ACT_R_FLAP_R, ACT_R_FLAP_E),
            ServoActuator("prog_disp", PwmOut(ACT_R_PROG), ACT_R_PROG_R, ACT_R_PROG_E),
            CylinderRotationSystem(ax12_driver, queue, COLOR_R_GREEN, COLOR_R_BLUE, COLOR_R_AX12_ID, COLOR_R_REVERSED)
        )
    );
    debug->printf("\tok.\n");

    *_actuators = actuators;
}
#endif

#ifdef IAM_QREFLECHI
void init_finalize(Debug *debug, Timer *main_timer, EventQueue *queue, CanMessenger *messenger)
#else
void init_finalize(Debug *debug, Timer *main_timer, EventQueue *queue)
#endif
{
    debug->printf("Board-specific subsystems initialized successfully.\n");
    debug->printf("t4=%.3f\n", main_timer->read());

    mem_stats_dynamic(debug);

    debug->printf("Initialization successfull.\n");
    debug->printf("t5=%.3f\n", main_timer->read());

    debug->printf("Running tests...\n");
    test_run_all(debug);
    debug->printf("Tests ran successfull.\n");
    debug->printf("t6=%.3f\n", main_timer->read());

    debug->printf("Waiting until %.3f sec...\n", INIT_FINALIZE_WAIT_UNTIL);
    while (main_timer->read() < INIT_FINALIZE_WAIT_UNTIL)
    {
        queue->dispatch(0);
        wait_ms(1);
    }
    debug->printf("\tok.\n");
    debug->printf("t7=%.3f\n", main_timer->read());

#ifdef IAM_QREFLECHI
    queue->call_every(INIT_DELAY_CQR_PING, callback(messenger, &CanMessenger::send_msg_CQR_ping));
#endif

    debug->set_current_level(Debug::DEBUG_DEBUG);
}
