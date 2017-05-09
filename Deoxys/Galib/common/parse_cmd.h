#ifndef PARSE_CMD_H_INCLUDED
#define PARSE_CMD_H_INCLUDED

#include "common/Debug.h"

#if defined(IAM_QBOUGE)
#include "QBouge/MotionController.h"
#elif defined(IAM_QREFLECHI)
#include "common/Messenger.h"
#elif defined(IAM_QENTRESORT)
#include "common/OrdersFIFO.h"
#endif


#if defined(IAM_QBOUGE)
void parse_cmd(Debug *debug, MotionController *mc);
#elif defined(IAM_QREFLECHI)
void parse_cmd(Debug *debug, CanMessenger *messenger, EventQueue *queue);
#elif defined(IAM_QENTRESORT)
void parse_cmd(Debug *debug, OrdersFIFO *orders, Actuators *actuators);
#endif

#endif // #ifndef PARSE_CMD_H_INCLUDED
