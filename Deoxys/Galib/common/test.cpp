
#include "common/Debug.h"
#include "common/Messenger.h"
#include "common/OrdersFIFO.h"
#ifdef IAM_QBOUGE
#include "QBouge/test_mc.h"
#endif
#include "common/test.h"

#ifndef DISABLE_TESTS

void test_run_all(Debug *debug)
{
    debug->printf("[Tests] Starting\n");

    test_sizes(debug);
#ifdef IAM_QBOUGE
    test_mc_calcNewPos(debug);
    test_mc_calcDistThetaOrderPos(debug);
    test_mc_updateCurOrder(debug);
#endif
    test_CAN_payload_sizes(debug);
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

    ft_assert(sizeof(s_order_com) == 8);

    debug->printf("\tok.\n");
}

void test_CAN_payload_sizes(Debug *debug) {
    debug->printf("Testing CAN payload size\n");

    ft_assert(sizeof(Message::CP_CQR_ping) <= 8);
    ft_assert(sizeof(Message::CP_pong) <= 8);
    ft_assert(sizeof(Message::CP_CQR_match_start) <= 8);
    ft_assert(sizeof(Message::CP_CQR_match_stop) <= 8);
    ft_assert(sizeof(Message::CP_CQR_we_are_at) <= 8);
    ft_assert(sizeof(Message::CP_CQR_order) <= 8);
    ft_assert(sizeof(Message::CP_CQB_next_order_request) <= 8);
    ft_assert(sizeof(Message::CP_CQES_next_order_request) <= 8);
    ft_assert(sizeof(Message::CP_CQR_settings_CQB) <= 8);
    ft_assert(sizeof(Message::CP_CQR_settings_CQES) <= 8);
    ft_assert(sizeof(Message::CP_I_am_doing) <= 8);
    ft_assert(sizeof(Message::CP_CQB_MC_pos_angle) <= 8);
    ft_assert(sizeof(Message::CP_CQB_MC_speeds) <= 8);
    ft_assert(sizeof(Message::CP_CQB_MC_pids) <= 8);
    ft_assert(sizeof(Message::CP_CQB_MC_motors) <= 8);
    ft_assert(sizeof(Message::CP_CQB_MC_encs) <= 8);
    ft_assert(sizeof(Message::u_payload) <= 8);

    debug->printf("\tok.\n");
}

#else // #ifndef DISABLE_TESTS

void test_run_all(Debug *debug) {
    debug->printf("[Tests] skipping tests (DISABLE_TESTS is defined)\n");
}
void _ft_assert(Debug *debug, int cond, const char *s_cond, const char *file, const char *func, int line) {}
void test_sizes(Debug *debug) {}

#endif // #ifndef DISABLE_TESTS
