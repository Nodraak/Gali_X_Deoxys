#ifndef COM_H_INCLUDED
#define COM_H_INCLUDED

#include "Debug.h"
#include "Messenger.h"

#ifdef TARGET_NUCLEO_F303K8

#include "MotionController.h"

void com_handle_serial(Debug *debug, CanMessenger *messenger, MotionController *mc);
void com_handle_can(Debug *debug, CanMessenger *messenger, MotionController *mc);

#else

void com_handle_serial(Debug *debug, CanMessenger *messenger);
void com_handle_can(Debug *debug, CanMessenger *messenger);

#endif

#endif
