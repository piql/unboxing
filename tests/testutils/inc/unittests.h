#ifndef BOXING_UNITTESTS_H
#define BOXING_UNITTESTS_H

#ifdef NDEBUG
#undef NDEBUG
#endif


#ifdef _WIN32
typedef int pid_t;
#endif

#include <check.h>
#define BOXING_ASSERT(x) ck_assert(x)
#define BOXING_ASSERT_STR_EQ(a, b) ck_assert_str_eq(a, b)
#define BOXING_ASSERT_MESSAGE ck_assert_msg

#define BOXING_START_TEST( name ) \
    START_TEST( name )
#endif
