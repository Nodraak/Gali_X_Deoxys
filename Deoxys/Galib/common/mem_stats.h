#ifndef MEM_STATS_H_INCLUDED
#define MEM_STATS_H_INCLUDED

/*
    These functions prints information about memory usages.
    This can help debug memory problems and give some insight about the size of
    each object (class instance).
*/


#include "common/Debug.h"

// prints information about the size of each class or object that is instanciated
void mem_stats_objects(Debug *debug);

// prints information about the memory layout at runtime (heap and stack size)
void mem_stats_dynamic(Debug *debug);

// prints the values of the relevant defines
void mem_stats_settings(Debug *debug);

extern uint32_t __HeapLimit;
extern uint32_t __StackLimit;

#endif
