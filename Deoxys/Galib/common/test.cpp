
#include "common/OrdersFIFO.h"
#ifdef IAM_QBOUGE
#include "QBouge/test_mc.h"
#include "QBouge/test_motor.h"
#endif
#include "test.h"

#ifndef DISABLE_TESTS

void test_run_all(Debug *debug)
{
    debug->printf("[Tests] Starting\n");

    test_sizes(debug);
#ifdef IAM_QBOUGE
    test_mc_calcNewPos(debug);
    test_mc_calcDistThetaOrderPos(debug);
    test_mc_updateCurOrder(debug);
    test_motor_cap_pwm(debug);
#endif
    debug->printf("[Tests] Done.\n");

#ifdef INFINITE_LOOP_AFTER_TESTS
    while (1)
    {
        debug->printf("I'm alive\n");
        wait(1);
    }
#endif // INFINITE_LOOP_AFTER_TESTS
}

void _ft_assert(Debug *debug, int cond, const char *s_cond, const char *file, const char *func, int line) {
    if (!cond)
    {
        debug->printf("ASSERT FAILED at %s:%d (func %s) \"%s\"\n", file, line, func, s_cond);
        while (1)
            ;
    }
}

void test_sizes(Debug *debug) {
    debug->printf("Testing sizes\n");

    ft_assert(sizeof(int16_t) == 2);
    ft_assert(sizeof(int32_t) == 4);
    ft_assert(sizeof(int64_t) == 8);

    ft_assert(sizeof(s_order_com) == 8); // todo test_(orders|messenger) ??
}

#else // #ifndef DISABLE_TESTS

void test_run_all(Debug *debug) {
    debug->printf("[Tests] skipping tests (DISABLE_TESTS is defined)\n");
}
void _ft_assert(Debug *debug, int cond, const char *s_cond, const char *file, const char *func, int line) {}
void test_sizes(Debug *debug) {}

#endif // #ifndef DISABLE_TESTS
