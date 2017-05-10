#ifndef TEST_H_INCLUDED
#define TEST_H_INCLUDED

#include "common/Debug.h"

void test_run_all(Debug *debug);

/*
    Public
*/

// assert a condition is true
void ft_assert(bool cond);
// assert two floats are equals
void ft_assert_equal(float f1, float f2);
// check sizes (int*_t)
void test_sizes(Debug *debug);

void test_CAN_payload_sizes(Debug *debug);

/*
    Private
*/

void _ft_assert(Debug *debug, int cond, const char *s_cond, const char *file, const char *func, int line);

#define ft_assert(cond) _ft_assert(debug, cond, #cond, __FILE__, __FUNCTION__, __LINE__)
#define ft_assert_equal(f1, f2) ft_assert(ABS(f1-f2) < 0.000001)

#endif
