
#include "test.h"

void _ft_assert(Debug *debug, int cond, const char *file, int line) {
    if (!cond)
    {
        debug->printf("FAILED ASSERT in %s line %d\n", file, line);
        while (1)
            ;
    }
}

void test_sizes(Debug *debug) {
    ft_assert(sizeof(int16_t) == 2);
    ft_assert(sizeof(int32_t) == 4);
    ft_assert(sizeof(int64_t) == 8);
}
