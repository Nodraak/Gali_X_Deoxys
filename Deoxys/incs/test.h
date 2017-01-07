#ifndef TEST_H_INCLUDED
#define TEST_H_INCLUDED

#include "Debug.h"

void _ft_assert(Debug *debug, int cond, const char *s_cond, const char *file, const char *func, int line);
void test_sizes(Debug *debug);
void test_run_all(Debug *debug, bool infinite_loop_after_tests);

// TODO http://stackoverflow.com/questions/5459868/c-preprocessor-concatenate-int-to-string#5459929
#define ft_assert(cond) _ft_assert(debug, cond, #cond, __FILE__, __FUNCTION__, __LINE__)
#define ft_assert_equal(f1, f2) ft_assert(ABS(f1-f2) < 0.000001)

#endif
