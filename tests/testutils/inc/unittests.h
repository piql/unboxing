#ifndef BOXING_UNITTESTS_H
#define BOXING_UNITTESTS_H

#ifdef NDEBUG
#undef NDEBUG
#endif


#ifdef _WIN32
typedef int pid_t;
#endif

#ifdef UNIT_TESTS_ENABLED
#include <check.h>
#error
#define BOXING_ASSERT(x) ck_assert(x)
#define BOXING_ASSERT_STR_EQ(a, b) ck_assert_str_eq(a, b)
#define BOXING_ASSERT_MESSAGE ck_assert_msg

#else
#include <assert.h>
#include <stdio.h>
#define START_TEST( name ) void name(void) { printf("%s\n", #name); int _i;
#define END_TEST }
typedef void TCase;
#define tcase_create( s ) (void*)printf("%s\n", s)
#define tcase_add_test(a, func) func();

typedef void Suite;
#define suite_create(a) (void*)1
#define suite_add_tcase(s, a)

#define ck_assert assert

typedef void SRunner;
#define srunner_create(a) (void*)1
#define srunner_set_xml( a, b )
#define srunner_add_suite(a, func) func
#define srunner_run_all(a, b)
#define CK_NORMAL 0

#define ck_assert_msg( a, b ) assert(a)
#define ck_assert_str_eq( a, b) 

#define BOXING_ASSERT(x) assert(x)
#define BOXING_ASSERT_STR_EQ(a, b) assert(strcmp(a,b) == 0)
#define BOXING_ASSERT_MESSAGE( a, ... ) if (!(a)) { assert(a); printf( __VA_ARGS__ ); }  

#define srunner_ntests_failed(a)  (void*)0;
#define srunner_free(a)
#endif


#define BOXING_START_TEST( name ) \
    START_TEST( name ) \
    BOXING_UNUSED_PARAMETER(_i); 

#endif
