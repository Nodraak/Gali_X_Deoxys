#ifndef SYS_H_INCLUDED
#define SYS_H_INCLUDED

#include "common/Debug.h"

void sys_print_reset_source(Debug *debug);
void sys_interrupt_priorities_init(void);

void sys_debug_can(void);

#endif // SYS_H_INCLUDED
