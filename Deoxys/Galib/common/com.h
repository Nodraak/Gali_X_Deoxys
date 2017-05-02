#ifndef COM_H_INCLUDED
#define COM_H_INCLUDED

#include "common/Debug.h"
#include "common/Messenger.h"
#include "common/OrdersFIFO.h"

#ifdef IAM_QBOUGE

#include "QBouge/MotionController.h"

void com_handle_can(Debug *debug, CanMessenger *messenger, OrdersFIFO *orders, bool *cqes_finished, MotionController *mc);

#endif

#ifdef IAM_QREFLECHI

void com_handle_serial(Debug *debug, CanMessenger *messenger);
void com_handle_can(Debug *debug, CanMessenger *messenger, OrdersFIFO *orders);

#endif

#ifdef IAM_QENTRESORT

void com_handle_can(Debug *debug, CanMessenger *messenger, OrdersFIFO *orders, bool *cqb_finished);

#endif

#endif
