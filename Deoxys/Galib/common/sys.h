#ifndef SYS_H_INCLUDED
#define SYS_H_INCLUDED

#include "common/Debug.h"

void sys_print_reset_source(Debug *debug);
void sys_interrupt_priorities_init(void);
void sys_debug_can(void);

/*
    Disable and enable the Interrupt TIM2 used by the Ticker to call asserv().
    Theses functions must be used by every function that is called from the
    main thread (non interrupt) and that modify private attributes. Otherwise
    data corruption might happen.
*/
void ticker_lock(void);
void ticker_unlock(void);

#endif // SYS_H_INCLUDED
