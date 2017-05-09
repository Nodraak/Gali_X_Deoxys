#ifndef INIT_H_INCLUDED
#define INIT_H_INCLUDED

#include "mbed.h"

#include "common/StatusLeds.h"
#include "common/Debug.h"
#include "common/Messenger.h"
#include "common/OrdersFIFO.h"

#ifdef IAM_QBOUGE
#include "QBouge/MotionController.h"
#endif
#ifdef IAM_QENTRESORT
#include "QEntreQSort/Actuators.h"
#endif

#define INIT_DELAY_LED_RUNNING          500         // ms
#define INIT_DELAY_LED_UPDATE_BO        (1000/100)  // ms

#define INIT_DELAY_CQR_PING             500         // ms
#define INIT_DELAY_LEAVE_THE_BUS        1000        // ms

#define INIT_DELAY_PRINT_TIME           250         // ms
#define INIT_DELAY_MONITORING_RESET     1000        // ms
#define INIT_DELAY_DEBUG_MC_SERIAL      1000        // ms
#define INIT_DELAY_DEBUG_MC_CAN         1000        // ms
#define INIT_DELAY_DEBUG_SYS_CAN        500         // ms

#define INIT_FINALIZE_WAIT_UNTIL        1.500       // sec


void init_common(
    Timer **_main_timer,
    StatusLeds **_sl,
    Debug **_debug,
    CanMessenger **_messenger,
    OrdersFIFO **_orders,
    EventQueue **_queue,
    Timer **_loop
);

#ifdef IAM_QBOUGE
void init_board_CQB(Debug *debug,
    EventQueue *queue,
    CanMessenger *messenger,
    MotionController **_mc
);
#endif

#ifdef IAM_QREFLECHI
void init_board_CQR(Debug *debug, OrdersFIFO *orders);
#endif

#ifdef IAM_QENTRESORT
void init_board_CQES(Debug *debug,
    Actuators **_actuators
);
#endif

#ifdef IAM_QREFLECHI
void init_finalize(Debug *debug, Timer *main_timer, EventQueue *queue, CanMessenger *messenger);
#else
void init_finalize(Debug *debug, Timer *main_timer, EventQueue *queue);
#endif

#endif // #ifndef INIT_H_INCLUDED
