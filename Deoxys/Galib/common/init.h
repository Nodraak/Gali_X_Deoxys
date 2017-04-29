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
#ifdef IAM_QENTREQSORT
#include "QBouge/RoboticArm.h"
#endif


#define INIT_FINALIZE_WAIT_UNTIL 1.500  // sec


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
    AX12_arm ***_arms
);
#endif

void init_finalize(Debug *debug, Timer *main_timer);

#endif // #ifndef INIT_H_INCLUDED
