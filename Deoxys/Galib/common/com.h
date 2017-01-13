#ifndef COM_H_INCLUDED
#define COM_H_INCLUDED

#include "common/Debug.h"
#include "common/Messenger.h"

#ifdef IAM_QBOUGE

#include "QBouge/MotionController.h"

void com_handle_serial(Debug *debug, CanMessenger *messenger, MotionController *mc);
void com_handle_can(Debug *debug, CanMessenger *messenger, MotionController *mc);

#endif

#ifdef IAM_QREFLECHI

void com_handle_serial(Debug *debug, CanMessenger *messenger);
void com_handle_can(Debug *debug, CanMessenger *messenger);

#endif

#endif
