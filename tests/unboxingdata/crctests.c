#pragma GCC diagnostic ignored "-Wunused-variable"
/*****************************************************************************
**
**  crc32 unittests
**
**  Creation date:  2016/11/17
**  Created by:     Alexander Silokhin
**
**
**  Copyright (c) 2016 Piql AS. All rights reserved.
**
**  This file is part of the boxing library
**
*****************************************************************************/

#include "unittests.h"
#include "boxing/math/crc32.h"
#include "boxing/math/crc64.h"
#include "boxing/utils.h"
#include <stdio.h>

BOXING_START_TEST(boxing_crc32_create_def_test0)
{
    dcrc32 * calc_crc = boxing_math_crc32_create_def();

    BOXING_ASSERT(NULL != calc_crc);

    boxing_math_crc32_free(calc_crc);
}
END_TEST

BOXING_START_TEST(boxing_crc32_calc_crc_test0)
{
    dcrc32 * calc_crc = boxing_math_crc32_create_def();
    char text[] = "Hello world!";
    const unsigned int buffer_size = sizeof(text);
    const boxing_uint32 checksum = boxing_math_crc32_calc_crc(calc_crc, text, (unsigned int)buffer_size - 1);
    boxing_math_crc32_free(calc_crc);

    BOXING_ASSERT(0 != checksum);
}
END_TEST

BOXING_START_TEST(boxing_crc32_calc_crc_test1)
{
    boxing_uint32 buffer[5];
    buffer[0] = 1;
    buffer[1] = 3;
    buffer[2] = 5;
    buffer[3] = 7;

    dcrc32 * calc_crc = boxing_math_crc32_create_def();

    // Append CRC to end of buffer, crc(buffer + CRC) => 0
    boxing_uint32 c = boxing_math_crc32_calc_crc(calc_crc, (const char *)buffer, sizeof(boxing_uint32)*4);
    unsigned char* b = (unsigned char*)(buffer + 4);
    for(int i = 0; i < 0x4; i++)
    {
        unsigned char ch = (c >> (8*(3-i))) & 0xff;
        b[i] = ch;
    }

    boxing_math_crc32_reset(calc_crc, 0);
    const boxing_uint32 total_checksum = boxing_math_crc32_calc_crc(calc_crc, (const char *)buffer, sizeof(boxing_uint32)*5);
    boxing_math_crc32_free(calc_crc);

    BOXING_ASSERT(0 == total_checksum);
}
END_TEST

BOXING_START_TEST(boxing_crc32_get_crc_test0)
{
    dcrc32 * calc_crc = boxing_math_crc32_create(255, POLY_CRC_32);
    const boxing_uint32 crc = boxing_math_crc32_get_crc(calc_crc);
    boxing_math_crc32_free(calc_crc);

    BOXING_ASSERT(255 == crc);
}
END_TEST

BOXING_START_TEST(boxing_crc32_reset_crc_test0)
{
    dcrc32 * calc_crc = boxing_math_crc32_create_def();
    char buffer[] = "Hello world!";
    boxing_math_crc32_calc_crc(calc_crc, buffer, sizeof(buffer));
    const boxing_uint32 crc = boxing_math_crc32_get_crc(calc_crc);
    boxing_math_crc32_reset(calc_crc, 0);
    const boxing_uint32 new_crc = boxing_math_crc32_get_crc(calc_crc);
    boxing_math_crc32_free(calc_crc);

    BOXING_ASSERT(crc != new_crc);
}
END_TEST

BOXING_START_TEST(boxing_crc64_create_def_test0)
{
    dcrc64 * calc_crc = boxing_math_crc64_create_def();

    BOXING_ASSERT(NULL != calc_crc);

    boxing_math_crc64_free(calc_crc);
}
END_TEST

BOXING_START_TEST(boxing_crc64_calc_crc_test0)
{
    dcrc64 * calc_crc = boxing_math_crc64_create_def();
    char text[] = "Hello world!";
    const unsigned int buffer_size = sizeof(text);
    const boxing_uint64 checksum = boxing_math_crc64_calc_crc(calc_crc, text, buffer_size - 1);
    boxing_math_crc64_free(calc_crc);

    BOXING_ASSERT(0 != checksum);
}
END_TEST

BOXING_START_TEST(boxing_crc64_calc_crc_test1)
{
    boxing_uint64 buffer[5];
    buffer[0] = 1;
    buffer[1] = 3;
    buffer[2] = 5;
    buffer[3] = 7;

    dcrc64 * calc_crc = boxing_math_crc64_create_def();

    // Append CRC to end of buffer, crc(buffer + CRC) => 0
    boxing_uint64 c = boxing_math_crc64_calc_crc(calc_crc, (const char *)buffer, sizeof(boxing_uint64) * 4);
    unsigned char* b = (unsigned char*)(buffer + 4);
    for(int i = 0; i < 0x8; i++)
    {
        unsigned char ch = (c >> (8*(7-i))) & 0xff;
        b[i] = ch;
    }

    boxing_math_crc64_reset(calc_crc, 0);
    const boxing_uint64 total_checksum = boxing_math_crc64_calc_crc(calc_crc, (const char *)buffer, sizeof(boxing_uint64) * 5);
    boxing_math_crc64_free(calc_crc);

    //printf("buffer[4] = %#llx, total checksum = %#llx\n", buffer[4], total_checksum);

    BOXING_ASSERT(0 == total_checksum);
}
END_TEST

BOXING_START_TEST(boxing_crc64_get_crc_test0)
{
    dcrc64 * calc_crc = boxing_math_crc64_create(255, POLY_CRC_32);
    const boxing_uint64 crc = boxing_math_crc64_get_crc(calc_crc);
    boxing_math_crc64_free(calc_crc);

    BOXING_ASSERT(255 == crc);
}
END_TEST

BOXING_START_TEST(boxing_crc64_reset_crc_test0)
{
    dcrc64 * calc_crc = boxing_math_crc64_create_def();
    char buffer[] = "Hello world!";
    boxing_math_crc64_calc_crc(calc_crc, buffer, sizeof(buffer));
    const boxing_uint64 crc = boxing_math_crc64_get_crc(calc_crc);
    boxing_math_crc64_reset(calc_crc, 0);
    const boxing_uint64 new_crc = boxing_math_crc64_get_crc(calc_crc);
    boxing_math_crc64_free(calc_crc);

    BOXING_ASSERT(crc != new_crc);
}
END_TEST


Suite * crc32_tests(void)
{
    TCase * tc_crc32_utils_tests = tcase_create("tc_crc32_util_tests");
    tcase_add_test(tc_crc32_utils_tests, boxing_crc32_create_def_test0);
    tcase_add_test(tc_crc32_utils_tests, boxing_crc32_calc_crc_test0);
    tcase_add_test(tc_crc32_utils_tests, boxing_crc32_calc_crc_test1);
    tcase_add_test(tc_crc32_utils_tests, boxing_crc32_get_crc_test0);
    tcase_add_test(tc_crc32_utils_tests, boxing_crc32_reset_crc_test0);

    TCase * tc_crc64_utils_tests = tcase_create("tc_crc64_util_tests");
    tcase_add_test(tc_crc64_utils_tests, boxing_crc64_create_def_test0);
    tcase_add_test(tc_crc64_utils_tests, boxing_crc64_calc_crc_test0);
    tcase_add_test(tc_crc64_utils_tests, boxing_crc64_calc_crc_test1);
    tcase_add_test(tc_crc64_utils_tests, boxing_crc64_get_crc_test0);
    tcase_add_test(tc_crc64_utils_tests, boxing_crc64_reset_crc_test0);

    Suite * s = suite_create("crc_test_util");
    suite_add_tcase(s, tc_crc32_utils_tests);
    suite_add_tcase(s, tc_crc64_utils_tests);

    return s;
}
