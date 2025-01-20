#pragma GCC diagnostic ignored "-Wunused-variable"

#include "unittests.h"
#include "boxing/math/math.h"
#include "boxing/utils.h"


const float allowableErrorFloat = 0.00000001f; // Allowable error for float
const double allowableErrorDouble = 0.0000000001; // Allowable error for double

// Tests for file boxing/math/math.h

//  
//  MACROS Tests
//

// Dot product test for two points (boxing_pointi)
BOXING_START_TEST(BOXING_MATH_DOT_PRODUCT_int_test1)
{
    boxing_pointi point1 = { 1, 2 };
    boxing_pointi point2 = { 3, 4 };

    int result = BOXING_MATH_DOT_PRODUCT(point1, point2);

    BOXING_ASSERT(result == 11);
}
END_TEST


// Dot product test for two points (boxing_pointi)
BOXING_START_TEST(BOXING_MATH_DOT_PRODUCT_int_test2)
{
    boxing_pointi point1 = { 1, 2 };
    boxing_pointi point2 = { -2, 1 };

    int result = BOXING_MATH_DOT_PRODUCT(point1, point2);

    BOXING_ASSERT(result == 0);
}
END_TEST


// Dot product test for two points (boxing_pointf)
BOXING_START_TEST(BOXING_MATH_DOT_PRODUCT_float_test1)
{
    boxing_pointf point1 = { 1.5f, 2.2f };
    boxing_pointf point2 = { 3.8f, 4.0f };

    float result = BOXING_MATH_DOT_PRODUCT(point1, point2);

    BOXING_ASSERT(result == 14.5f);
}
END_TEST


// Dot product test for two points (boxing_pointf)
BOXING_START_TEST(BOXING_MATH_DOT_PRODUCT_float_test2)
{
    boxing_pointf point1 = { 1.5f, 2.2f };
    boxing_pointf point2 = { -2.2f, 1.5f };

    float result = BOXING_MATH_DOT_PRODUCT(point1, point2);

    BOXING_ASSERT(result == 0.0f);
}
END_TEST


// The test for determining the maximum value of the two variables
BOXING_START_TEST(BOXING_MATH_MAX_test)
{
    BOXING_ASSERT(BOXING_MATH_MAX(5, 4) == 5);
    BOXING_ASSERT(BOXING_MATH_MAX(4, 5) == 5);
    BOXING_ASSERT(BOXING_MATH_MAX(-5, -10) == -5);
}
END_TEST


// The test for determining the minimum value of the two variables
BOXING_START_TEST(BOXING_MATH_MIN_test)
{
    BOXING_ASSERT(BOXING_MATH_MIN(5, 4) == 4);
    BOXING_ASSERT(BOXING_MATH_MIN(4, 5) == 4);
    BOXING_ASSERT(BOXING_MATH_MIN(-5, -10) == -10);
}
END_TEST


// Test cutoff value of the variable by specified limits
BOXING_START_TEST(BOXING_MATH_CLAMP_test)
{
    BOXING_ASSERT(BOXING_MATH_CLAMP(-5, 5, 1) == 1);
    BOXING_ASSERT(BOXING_MATH_CLAMP(4, 5, 10) == 5);
    BOXING_ASSERT(BOXING_MATH_CLAMP(-10, -5, -11) == -10);
}
END_TEST


// Test for occurrence of a variable in the specified limits
BOXING_START_TEST(BOXING_MATH_IS_WITHIN_LIMIT_test)
{
    BOXING_ASSERT(BOXING_MATH_IS_WITHIN_LIMIT(-5, 5, 1));
    BOXING_ASSERT(!BOXING_MATH_IS_WITHIN_LIMIT(4, 5, 10));
    BOXING_ASSERT(!BOXING_MATH_IS_WITHIN_LIMIT(-10, -5, -11));
}
END_TEST


//  
//  FUNCTIONS Tests
//

// Invariant matrix calculation test with determinant equal to zero
BOXING_START_TEST(boxing_math_matrix3x3_invariant_test1)
{
    boxing_double m1[3][3] = { { 1.0, 2.0, 3.0 }, { 4.0, 5.0, 6.0 }, { 7.0, 8.0, 9.0 } };
    boxing_double mInv[3][3] = { { 0.0, 0.0, 0.0 }, { 0.0, 0.0, 0.0 }, { 0.0, 0.0, 0.0 } };

    DBOOL returnValue = boxing_math_matrix3x3_invariant(m1, mInv);

    BOXING_ASSERT(returnValue == DFALSE);
    BOXING_ASSERT(mInv[0][0] == 0.0);
    BOXING_ASSERT(mInv[0][1] == 0.0);
    BOXING_ASSERT(mInv[0][2] == 0.0);
    BOXING_ASSERT(mInv[1][0] == 0.0);
    BOXING_ASSERT(mInv[1][1] == 0.0);
    BOXING_ASSERT(mInv[1][2] == 0.0);
    BOXING_ASSERT(mInv[2][0] == 0.0);
    BOXING_ASSERT(mInv[2][1] == 0.0);
    BOXING_ASSERT(mInv[2][2] == 0.0);
}
END_TEST


// Invariant matrix calculation test with determinant is not equal to zero
BOXING_START_TEST(boxing_math_matrix3x3_invariant_test2)
{
    boxing_double m1[3][3] = { { 1.0, 2.0, 3.0 }, { 4.0, 1.0, 6.0 }, { 7.0, 77.0, 1.0 } };
    boxing_double mInv[3][3];

    boxing_double mResult[3][3] = { { -0.88996138996138996, 0.44208494208494209, 0.017374517374517374 },
    { 0.073359073359073365, -0.038610038610038609, 0.011583011583011584 },
    { 0.58108108108108114, -0.12162162162162163, -0.013513513513513514 } };

    DBOOL returnValue = boxing_math_matrix3x3_invariant(m1, mInv);

    BOXING_ASSERT(returnValue == DTRUE);
    BOXING_ASSERT(fabs(mInv[0][0] - mResult[0][0]) < allowableErrorDouble);
    BOXING_ASSERT(fabs(mInv[0][1] - mResult[0][1]) < allowableErrorDouble);
    BOXING_ASSERT(fabs(mInv[0][2] - mResult[0][2]) < allowableErrorDouble);
    BOXING_ASSERT(fabs(mInv[1][0] - mResult[1][0]) < allowableErrorDouble);
    BOXING_ASSERT(fabs(mInv[1][1] - mResult[1][1]) < allowableErrorDouble);
    BOXING_ASSERT(fabs(mInv[1][2] - mResult[1][2]) < allowableErrorDouble);
    BOXING_ASSERT(fabs(mInv[2][0] - mResult[2][0]) < allowableErrorDouble);
    BOXING_ASSERT(fabs(mInv[2][1] - mResult[2][1]) < allowableErrorDouble);
    BOXING_ASSERT(fabs(mInv[2][2] - mResult[2][2]) < allowableErrorDouble);
}
END_TEST


// Vector normalization test
BOXING_START_TEST(boxing_math_pointf_normalize_test1)
{
    boxing_pointf point = { 1.5f, 2.2f };
    boxing_pointf resultPoint = { 0.563336790f, 0.826227367f };

    boxing_pointf normallizedPoint = boxing_math_pointf_normalize(&point);

    BOXING_ASSERT(fabs(resultPoint.x - normallizedPoint.x) < allowableErrorFloat);
    BOXING_ASSERT(fabs(resultPoint.y - normallizedPoint.y) < allowableErrorFloat);
}
END_TEST


// Vector zero normalization test
BOXING_START_TEST(boxing_math_pointf_normalize_test2)
{
    boxing_pointf point = { 0.0f, 0.0f };
    boxing_pointf resultPoint = { 0.0f, 0.0f };

    boxing_pointf normallizedPoint = boxing_math_pointf_normalize(&point);

    BOXING_ASSERT(fabs(resultPoint.x - normallizedPoint.x) < allowableErrorFloat);
    BOXING_ASSERT(fabs(resultPoint.y - normallizedPoint.y) < allowableErrorFloat);
}
END_TEST


// Determining the length of a vector test
BOXING_START_TEST(boxing_math_pointf_length_test)
{
    boxing_pointf point = { 1.5f, 2.2f };

    float lenght = boxing_math_pointf_length(&point);

    BOXING_ASSERT(fabs(lenght - 2.66270542f) < allowableErrorFloat);
}
END_TEST


// Test setting point coordinates
BOXING_START_TEST(boxing_math_pointi_set_test)
{
    boxing_pointi point = { 0, 0 };

    boxing_math_pointi_set(&point, 5, 3);

    BOXING_ASSERT(point.x == 5);
    BOXING_ASSERT(point.y == 3);
}
END_TEST


// Test comparing two points 1
BOXING_START_TEST(boxing_math_pointi_equal_test1)
{
    boxing_pointi point1 = { 2, 8 };
    boxing_pointi point2 = { 2, 8 };

    BOXING_ASSERT(boxing_math_pointi_equal(&point1, &point2));
}
END_TEST


// Test comparing two points 2
BOXING_START_TEST(boxing_math_pointi_equal_test2)
{
    boxing_pointi point1 = { 2, 8 };
    boxing_pointi point2 = { 0, 0 };

    BOXING_ASSERT(!boxing_math_pointi_equal(&point1, &point2));
}
END_TEST


// Test to identify the region of overlap of two intersecting rectangles
BOXING_START_TEST(boxing_math_recti_intersect_test1)
{
    boxing_recti rect1 = { 1, 1, 4, 2 };
    boxing_recti rect2 = { 3, 2, 4, 2 };

    boxing_math_recti_intersect(&rect1, &rect2);

    BOXING_ASSERT(rect1.x == 3);
    BOXING_ASSERT(rect1.y == 2);
    BOXING_ASSERT(rect1.width == 2);
    BOXING_ASSERT(rect1.height == 1);
}
END_TEST

// Test to identify the region of overlap of two intersecting rectangles
BOXING_START_TEST(boxing_math_recti_intersect_test2)
{
    boxing_recti rect1 = { 1, 1, 4, 2 };
    boxing_recti rect2 = { 3, 2, 4, 2 };

    boxing_math_recti_intersect(&rect2, &rect1);

    BOXING_ASSERT(rect2.x == 3);
    BOXING_ASSERT(rect2.y == 2);
    BOXING_ASSERT(rect2.width == 2);
    BOXING_ASSERT(rect2.height == 1);
}
END_TEST


// Test to identify the region of overlap of two intersecting rectangles
BOXING_START_TEST(boxing_math_recti_intersect_test3)
{
    boxing_recti rect1 = { 1, 1, 2, 2 };
    boxing_recti rect2 = { 4, 4, 2, 2 };

    boxing_math_recti_intersect(&rect1, &rect2);

    BOXING_ASSERT(rect1.x == 4);
    BOXING_ASSERT(rect1.y == 4);
    BOXING_ASSERT(rect1.width == 0);
    BOXING_ASSERT(rect1.height == 0);
}
END_TEST


// Test to identify the region of overlap of two intersecting rectangles
BOXING_START_TEST(boxing_math_recti_intersect_test4)
{
    boxing_recti rect1 = { 1, 1, 5, 4 };
    boxing_recti rect2 = { 4, 2, 2, 2 };

    boxing_math_recti_intersect(&rect1, &rect2);

    BOXING_ASSERT(rect1.x == 4);
    BOXING_ASSERT(rect1.y == 2);
    BOXING_ASSERT(rect1.width == 2);
    BOXING_ASSERT(rect1.height == 2);
}
END_TEST


// Test to identify the region of overlap of two intersecting rectangles
BOXING_START_TEST(boxing_math_recti_intersect_test5)
{
    boxing_recti rect1 = { 1, 1, 5, 4 };
    boxing_recti rect2 = { 4, 2, 2, 2 };

    boxing_math_recti_intersect(&rect2, &rect1);

    BOXING_ASSERT(rect2.x == 4);
    BOXING_ASSERT(rect2.y == 2);
    BOXING_ASSERT(rect2.width == 2);
    BOXING_ASSERT(rect2.height == 2);
}
END_TEST


// Test to identify the region of overlap of two intersecting rectangles
BOXING_START_TEST(boxing_math_recti_intersect_test6)
{
    boxing_recti rect1 = { 1, 2, 4, 2 };
    boxing_recti rect2 = { 3, 1, 4, 2 };

    boxing_math_recti_intersect(&rect1, &rect2);

    BOXING_ASSERT(rect1.x == 3);
    BOXING_ASSERT(rect1.y == 2);
    BOXING_ASSERT(rect1.width == 2);
    BOXING_ASSERT(rect1.height == 1);
}
END_TEST


// Test to identify the region of overlap of two intersecting rectangles
BOXING_START_TEST(boxing_math_recti_intersect_test7)
{
    boxing_recti rect1 = { 1, 2, 4, 2 };
    boxing_recti rect2 = { 3, 1, 4, 2 };

    boxing_math_recti_intersect(&rect2, &rect1);

    BOXING_ASSERT(rect2.x == 3);
    BOXING_ASSERT(rect2.y == 2);
    BOXING_ASSERT(rect2.width == 2);
    BOXING_ASSERT(rect2.height == 1);
}
END_TEST


Suite * math_tests(void)
{
    TCase *tc_math_macros_tests;
    tc_math_macros_tests = tcase_create("macros_tests");
    tcase_add_test(tc_math_macros_tests, BOXING_MATH_DOT_PRODUCT_int_test1);
    tcase_add_test(tc_math_macros_tests, BOXING_MATH_DOT_PRODUCT_int_test2);
    tcase_add_test(tc_math_macros_tests, BOXING_MATH_DOT_PRODUCT_float_test1);
    tcase_add_test(tc_math_macros_tests, BOXING_MATH_DOT_PRODUCT_float_test2);
    tcase_add_test(tc_math_macros_tests, BOXING_MATH_MAX_test);
    tcase_add_test(tc_math_macros_tests, BOXING_MATH_MIN_test);
    tcase_add_test(tc_math_macros_tests, BOXING_MATH_CLAMP_test);
    tcase_add_test(tc_math_macros_tests, BOXING_MATH_IS_WITHIN_LIMIT_test);

    TCase *tc_math_functions_tests;
    tc_math_functions_tests = tcase_create("functions_tests");
    tcase_add_test(tc_math_functions_tests, boxing_math_matrix3x3_invariant_test1);
    tcase_add_test(tc_math_functions_tests, boxing_math_matrix3x3_invariant_test2);
    tcase_add_test(tc_math_functions_tests, boxing_math_pointf_normalize_test1);
    tcase_add_test(tc_math_functions_tests, boxing_math_pointf_normalize_test2);
    tcase_add_test(tc_math_functions_tests, boxing_math_pointf_length_test);
    tcase_add_test(tc_math_functions_tests, boxing_math_pointi_set_test);
    tcase_add_test(tc_math_functions_tests, boxing_math_pointi_equal_test1);
    tcase_add_test(tc_math_functions_tests, boxing_math_pointi_equal_test2);
    tcase_add_test(tc_math_functions_tests, boxing_math_recti_intersect_test1);
    tcase_add_test(tc_math_functions_tests, boxing_math_recti_intersect_test2);
    tcase_add_test(tc_math_functions_tests, boxing_math_recti_intersect_test3);
    tcase_add_test(tc_math_functions_tests, boxing_math_recti_intersect_test4);
    tcase_add_test(tc_math_functions_tests, boxing_math_recti_intersect_test5);
    tcase_add_test(tc_math_functions_tests, boxing_math_recti_intersect_test6);
    tcase_add_test(tc_math_functions_tests, boxing_math_recti_intersect_test7);

    Suite *s;
    s = suite_create("math_test_util");
    suite_add_tcase(s, tc_math_macros_tests);
    suite_add_tcase(s, tc_math_functions_tests);

    return s;
}
