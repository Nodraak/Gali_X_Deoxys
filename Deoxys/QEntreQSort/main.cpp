
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
#include "common/utils.h"
#include "QEntreQSort/Ax12Driver.h"
#include "QEntreQSort/Actuators.h"
#include "QEntreQSort/CylinderRotationSystem.h"


#include "config.h"
#include "pinout.h"


// move in Actuators ?? or OrdersFIFO ??
bool main_update_cur_order(Actuators *actuators, OrdersFIFO *orders, float time_since_last_order_finished, bool *cqb_finished, bool *cqr_finished)
{
    bool is_current_order_executed_ = false;

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
            if (*cqb_finished)
            {
                *cqb_finished = false;
                is_current_order_executed_ = true;
            }
            break;

        case ORDER_EXE_TYPE_WAIT_CQR_FINISHED:
            if (*cqr_finished)
            {
                *cqr_finished = false;
                is_current_order_executed_ = true;
            }
            break;

        case ORDER_EXE_TYPE_WAIT_CQES_FINISHED:
            // ignore here
            break;

        case ORDER_EXE_TYPE_MOV_POS:
        case ORDER_EXE_TYPE_MOV_ANGLE:
            // ignore on CQES
            break;

        case ORDER_EXE_TYPE_ACTUATOR:
            actuators->activate(orders->current_order_.act_param);
            if (orders->current_order_.act_param & ACT_ACTUATOR_COLOR)
                orders->current_order_.type = ORDER_EXE_TYPE_COLOR_WAIT;
            else
                is_current_order_executed_ = true;
            break;

        case ORDER_EXE_TYPE_COLOR_WAIT:
            if (actuators->is_color_done(orders->current_order_.act_param))
                is_current_order_executed_ = true;
            break;

        case ORDER_EXE_TYPE_LAST:
            // nothing to do
            break;
    }

    return is_current_order_executed_;
}




// #include "QEntreQSort/CollisionDetectionSystem.h"


Ax12Driver *ax12_driver = NULL;


int main(void)
{
    Timer *main_timer = NULL;
    StatusLeds *sl = NULL;
    Debug *debug = NULL;
    CanMessenger *messenger = NULL;
    OrdersFIFO *orders = NULL;
    EventQueue *queue = NULL;
    Timer *loop = NULL;

    Actuators *actuators = NULL;
    float last_order_executed_timestamp = -1;

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
        queue,
        &actuators
    );
    init_finalize(debug, main_timer, queue);

    bool cqb_finished = false, cqr_finished = false;

    // CollisionDetectionSystem *cds = new CollisionDetectionSystem(queue, messenger);


#if 0
debug->printf("1\n");

DigitalIn sw1(SWITCH_1);
DigitalIn sw2(SWITCH_2);
DigitalIn sw3(SWITCH_3);
DigitalIn sw4(SWITCH_4);
debug->printf("2\n");

DigitalOut srf1_t(SRF1_T);
InterruptIn *srf1_e = new InterruptIn(SRF1_E);
DigitalOut srf2_t(SRF2_T);
InterruptIn *srf2_e = new InterruptIn(SRF2_E);
DigitalOut srf3_t(SRF3_T);
InterruptIn *srf3_e = new InterruptIn(SRF3_E);
DigitalOut srf4_t(SRF4_T);
InterruptIn *srf4_e = new InterruptIn(SRF4_E);
debug->printf("3\n");

AnalogIn a1(SHARP1);
AnalogIn a2(SHARP2);
AnalogIn a3(SHARP3);
AnalogIn a4(SHARP4);
AnalogIn a5(SHARP5);
debug->printf("4\n");

AnalogIn c1a(COLOR_1A);
AnalogIn c1b(COLOR_1B);
AnalogIn c2a(COLOR_2A);
AnalogIn c2b(COLOR_2B);
debug->printf("5\n");

DigitalIn di1(LASER_1);
DigitalIn di2(LASER_2);
debug->printf("6\n");

PwmOut po7(PWM_7);
PwmOut po8(PWM_8);
PwmOut poC1(PWM_C1);
PwmOut poC2(PWM_C2);
PwmOut poa(PWM_a);
PwmOut pob(PWM_b);
// PwmOut poc(PWM_c);
debug->printf("7\n");

wait_ms(100);
#include "common/mem_stats.h"
mem_stats_dynamic(debug);
wait_ms(100);

/*
debug->printf("AnalogIn %d\n", sizeof(AnalogIn));
debug->printf("DigitalIn %d\n", sizeof(DigitalIn));
debug->printf("DigitalOut %d\n", sizeof(DigitalOut));
debug->printf("InterruptIn %d\n", sizeof(InterruptIn));
debug->printf("PwmOut %d\n", sizeof(PwmOut));
AnalogIn 16
DigitalIn 28
DigitalOut 28
InterruptIn 80
PwmOut 24
*/

#endif


    orders->append(OrderCom_makeActuator(ACT_SIDE_RIGHT | ACT_ACTUATOR_COLOR));
    orders->append(OrderCom_makeSequence(ORDER_COM_TYPE_SEQ_PROGRADE_DISPENSER, ACT_SIDE_RIGHT));

    /*
        Go!
    */

    main_timer->reset();

    while (true)
    {
        g_mon->main_loop.start_new();
        loop->reset();

// debug->printf("a\n");
        queue->dispatch(0);  // non blocking dispatch
// debug->printf("b\n");
        com_handle_can(debug, messenger, orders, &cqb_finished, &cqr_finished, actuators);
// debug->printf("c\n");

        parse_cmd(debug, orders, actuators);

        // equiv MC::updateCurOrder
        // update the goals in function of the given order
        bool is_current_order_executed_ = main_update_cur_order(
            actuators, orders, main_timer->read()-last_order_executed_timestamp, &cqb_finished, &cqr_finished
        );

// debug->printf("d\n");

        if (orders->current_order_.type == ORDER_EXE_TYPE_WAIT_CQES_FINISHED)
        {
            messenger->send_msg_CQES_finished();
            is_current_order_executed_ = true;
        }

        if (is_current_order_executed_)
        {
            // if (orders->current_order_.type != ORDER_EXE_TYPE_NONE)
                // debug->printf("cur order %s done\n", e2s_order_exe_type[orders->current_order_.type]);

            // get the next order
            while (orders->next_order_execute())
                ;

            if (orders->current_order_.type != ORDER_EXE_TYPE_NONE)
                debug->printf("next order %s - %.3f\n", e2s_order_exe_type[orders->current_order_.type], orders->current_order_.delay);

            is_current_order_executed_ = false;
            last_order_executed_timestamp = main_timer->read();
        }

        // messenger->send_msg_I_am_doing(orders->current_order_.type);

        // if (orders->next_order_should_request())
        // {
        //     debug->printf("[CAN] send next_order_request\n");
        //     messenger->send_msg_CQB_next_order_request(1); NOPE NOPE CAN ERROR
        // }

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
