
#include "test_mc.h"
#include "test.h"

void test_run_all(Debug *debug)
{
#ifdef DISABLE_TESTS
    debug->printf("[Tests] skipping tests (DISABLE_TESTS is defined)\n");
#else // DISABLE_TESTS
    debug->printf("[Tests] Starting\n");

    test_sizes(debug);
    test_mc_calcNewPos(debug);
    test_mc_calcDistThetaOrderPos(debug);
    test_mc_updateCurOrder(debug);

    debug->printf("[Tests] Done.\n");

#ifdef INFINITE_LOOP_AFTER_TESTS
    while (1)
    {
        debug->printf("I'm alive\n");
        wait(1);
    }
#endif // INFINITE_LOOP_AFTER_TESTS
#endif // DISABLE_TESTS
}

#ifndef DISABLE_TESTS

void _ft_assert(Debug *debug, int cond, const char *s_cond, const char *file, const char *func, int line) {
    if (!cond)
    {
        debug->printf("FAILED ASSERT \"%s\" in %s (%s) at line %d.\n", s_cond, file, func, line);
        while (1)
            ;
    }
}

void test_sizes(Debug *debug) {
    debug->printf("Testing sizes\n");

    ft_assert(sizeof(int16_t) == 2);
    ft_assert(sizeof(int32_t) == 4);
    ft_assert(sizeof(int64_t) == 8);
}

#endif // DISABLE_TESTS
