#pragma GCC diagnostic ignored "-Wunused-variable"

#include "unittests.h"
#include "boxing/string.h"
#include "boxing/utils.h"

#include <string.h>

// Tests for file boxing/string.h

//  
//  FUNCTIONS Tests
//


// Test function boxing_string_clone with input data equal to NULL
BOXING_START_TEST(boxing_string_clone_test1)
{
    const char * temp_char1 = NULL;
    const char * temp_char2 = boxing_string_clone(temp_char1);

    BOXING_ASSERT(temp_char2 == NULL);
}
END_TEST


// Test function boxing_string_clone with input data equal to empty string
BOXING_START_TEST(boxing_string_clone_test2)
{
    const char * temp_char1 = "";
    char * temp_char2 = boxing_string_clone(temp_char1);

    BOXING_ASSERT(strlen(temp_char1) == strlen(temp_char2));

    free(temp_char2);
}
END_TEST


// Test function boxing_string_clone with not empty input string
BOXING_START_TEST(boxing_string_clone_test3)
{
    const char * temp_char1 = "test_text!";
    char * temp_char2 = boxing_string_clone(temp_char1);

    size_t string_length = strlen(temp_char1);
    for (size_t i = 0; i < string_length; i++)
    {
        BOXING_ASSERT(temp_char1[i] == temp_char2[i]);
    }

    free(temp_char2);
}
END_TEST


// Test function boxing_string_clone with input data equal to empty string with control characters
BOXING_START_TEST(boxing_string_clone_test4)
{
    const char * temp_char1 = "\ttest\t_text!\n\n";
    char * temp_char2 = boxing_string_clone(temp_char1);

    size_t string_length = strlen(temp_char1);
    for (size_t i = 0; i < string_length; i++)
    {
        BOXING_ASSERT(temp_char1[i] == temp_char2[i]);
    }

    free(temp_char2);
}
END_TEST


// Test function boxing_string_split with string and separator equal to NULL
BOXING_START_TEST(boxing_string_split_test1)
{
    char * temp_string = NULL;
    const char * separator = NULL;

    gvector * return_vector = boxing_string_split(temp_string, separator);

    BOXING_ASSERT(return_vector == NULL);

    gvector_free(return_vector);
}
END_TEST


// Test function boxing_string_split with separator equal to NULL
BOXING_START_TEST(boxing_string_split_test2)
{
    char * temp_string = "Test string";
    const char * separator = NULL;

    gvector * return_vector = boxing_string_split(temp_string, separator);

    BOXING_ASSERT(return_vector == NULL);

    gvector_free(return_vector);
}
END_TEST


// Test function boxing_string_split with string equal to NULL
BOXING_START_TEST(boxing_string_split_test3)
{
    char * temp_string = NULL;
    const char * separator = "separator";

    gvector * return_vector = boxing_string_split(temp_string, separator);

    BOXING_ASSERT(return_vector == NULL);

    gvector_free(return_vector);
}
END_TEST


// Test function boxing_string_split with separator equal to empty string
BOXING_START_TEST(boxing_string_split_test4)
{
    const char * temp_string = "Test string";
    const char * separator = "";

    gvector * return_vector = boxing_string_split(temp_string, separator);

    BOXING_ASSERT(return_vector != NULL);
    BOXING_ASSERT(return_vector->size == 1);

    const char * return_string = GVECTORN(return_vector, char *, 0);
    BOXING_ASSERT(strcmp(temp_string, return_string) == 0);

    gvector_free(return_vector);
}
END_TEST


// Test function boxing_string_split with the separator bigger then input string
BOXING_START_TEST(boxing_string_split_test5)
{
    char * temp_string = "Test";
    const char * separator = "Test string";

    gvector * return_vector = boxing_string_split(temp_string, separator);

    BOXING_ASSERT(return_vector != NULL);
    BOXING_ASSERT(return_vector->size == 1);
    const char * return_string = GVECTORN(return_vector, char *, 0);
    BOXING_ASSERT(strcmp(temp_string, return_string) == 0);

    gvector_free(return_vector);
}
END_TEST


// Test function boxing_string_split with separator not existing in input string
BOXING_START_TEST(boxing_string_split_test6)
{
    char * temp_string = "Test string";
    const char * separator = "w";

    gvector * return_vector = boxing_string_split(temp_string, separator);

    BOXING_ASSERT(return_vector != NULL);
    BOXING_ASSERT(return_vector->size == 1);
    const char * return_string = GVECTORN(return_vector, char *, 0);
    BOXING_ASSERT(strcmp(temp_string, return_string) == 0);

    gvector_free(return_vector);
}
END_TEST


// Test function boxing_string_split with separator existing in input string many times
BOXING_START_TEST(boxing_string_split_test7)
{
    char * temp_string = "T_e_s_t_ _s_t_r_i_n_g";
    char * wanted_result = "Test string";
    const char * separator = "_";

    gvector * return_vector = boxing_string_split(temp_string, separator);

    BOXING_ASSERT(return_vector != NULL);
    BOXING_ASSERT(return_vector->size == 11);

    for (unsigned int i = 0; i < return_vector->size; i++)
    {
        const char * return_string = GVECTORN(return_vector, char *, i);
        BOXING_ASSERT(return_string[0] == wanted_result[i]);
    }

    gvector_free(return_vector);
}
END_TEST


// Test function boxing_string_split with separator existing in input string in the middle of string
BOXING_START_TEST(boxing_string_split_test8)
{
    char * temp_string = "Test string test";
    char * wanted_result1 = "Test";
    char * wanted_result2 = "test";
    const char * separator = " string ";

    gvector * return_vector = boxing_string_split(temp_string, separator);

    BOXING_ASSERT(return_vector != NULL);
    BOXING_ASSERT(return_vector->size == 2);

    const char * return_string = GVECTORN(return_vector, char *, 0);
    BOXING_ASSERT(strcmp(wanted_result1, return_string) == 0);
    return_string = GVECTORN(return_vector, char *, 1);
    BOXING_ASSERT(strcmp(wanted_result2, return_string) == 0);

    gvector_free(return_vector);
}
END_TEST


// Test function boxing_string_split with a string consisting only of separators
BOXING_START_TEST(boxing_string_split_test9)
{
    char * temp_string = "testtest";
    const char * separator = "test";

    gvector * return_vector = boxing_string_split(temp_string, separator);

    BOXING_ASSERT(return_vector != NULL);
    BOXING_ASSERT(return_vector->size == 3);

    for (unsigned int i = 0; i < return_vector->size; i++)
    {
        const char * return_string = GVECTORN(return_vector, char *, i);
        BOXING_ASSERT(strlen(return_string) == 0);
    }

    gvector_free(return_vector);
}
END_TEST


// Test function boxing_string_split with a string equal to separator
BOXING_START_TEST(boxing_string_split_test10)
{
    char * temp_string = "Test string";
    const char * separator = "Test string";

    gvector * return_vector = boxing_string_split(temp_string, separator);

    BOXING_ASSERT(return_vector != NULL);
    BOXING_ASSERT(return_vector->size == 2);
    for (unsigned int i = 0; i < return_vector->size; i++)
    {
        const char * return_string = GVECTORN(return_vector, char *, i);
        BOXING_ASSERT(strlen(return_string) == 0);
    }

    gvector_free(return_vector);
}
END_TEST


// Test function boxing_string_split with separator existing in begin of input string 
BOXING_START_TEST(boxing_string_split_test11)
{
    char * temp_string = "Test string";
    const char * separator = "Test";

    gvector * return_vector = boxing_string_split(temp_string, separator);

    BOXING_ASSERT(return_vector != NULL);
    BOXING_ASSERT(return_vector->size == 2);

    const char * return_string = GVECTORN(return_vector, char *, 0);
    BOXING_ASSERT(strlen(return_string) == 0);
    return_string = GVECTORN(return_vector, char *, 1);
    BOXING_ASSERT(strcmp(" string", return_string) == 0);

    gvector_free(return_vector);
}
END_TEST


// Test function boxing_string_split with separator existing in end of input string
BOXING_START_TEST(boxing_string_split_test12)
{
    char * temp_string = "Test string";
    const char * separator = "string";

    gvector * return_vector = boxing_string_split(temp_string, separator);

    BOXING_ASSERT(return_vector != NULL);
    BOXING_ASSERT(return_vector->size == 2);

    const char * return_string = GVECTORN(return_vector, char *, 0);
    BOXING_ASSERT(strcmp("Test ", return_string) == 0);
    return_string = GVECTORN(return_vector, char *, 1);
    BOXING_ASSERT(strlen(return_string) == 0);

    gvector_free(return_vector);
}
END_TEST


// Test function boxing_string_to_integer with value and string equal to NULL
BOXING_START_TEST(boxing_string_to_integer_test1)
{
    DBOOL result = boxing_string_to_integer(NULL, NULL);

    BOXING_ASSERT(result == DFALSE);
}
END_TEST


// Test function boxing_string_to_integer with value equal to NULL
BOXING_START_TEST(boxing_string_to_integer_test2)
{
    DBOOL result = boxing_string_to_integer(NULL, "100");

    BOXING_ASSERT(result == DFALSE);
}
END_TEST


// Test function boxing_string_to_integer with string equal to NULL
BOXING_START_TEST(boxing_string_to_integer_test3)
{
    int value;
    DBOOL result = boxing_string_to_integer(&value, NULL);

    BOXING_ASSERT(result == DFALSE);
}
END_TEST


// Test function boxing_string_to_integer with different values
BOXING_START_TEST(boxing_string_to_integer_test4)
{
    struct
    {
        const char* string;
        int         value;  // -1 if illegal
    } tests[] =
    {
        { "100", 100 },
        { "10000", 10000 },
        { "123456789", 123456789 },
        { "-100", -1 },
        { " 100", -1 },
        { "100 ", -1 },
        { "abdcd", -1 }
    };
    int test_count = sizeof(tests)/sizeof(tests[0]);

    for ( int i = 0; i < test_count; i++ )
    {
        int value;
        DBOOL result = boxing_string_to_integer(&value,tests[i].string);
        DBOOL wanted_result = tests[i].value == -1 ? DFALSE : DTRUE;
        BOXING_ASSERT( result == wanted_result );
        BOXING_ASSERT( wanted_result == DFALSE || value == tests[i].value );
    }
}
END_TEST


Suite * string_tests(void)
{
    TCase *tc_string_functions_tests;
    tc_string_functions_tests = tcase_create("functions_tests");
    // Test function boxing_string_clone
    tcase_add_test(tc_string_functions_tests, boxing_string_clone_test1);
    tcase_add_test(tc_string_functions_tests, boxing_string_clone_test2);
    tcase_add_test(tc_string_functions_tests, boxing_string_clone_test3);
    tcase_add_test(tc_string_functions_tests, boxing_string_clone_test4);
    // Test function boxing_string_split
    tcase_add_test(tc_string_functions_tests, boxing_string_split_test1);
    tcase_add_test(tc_string_functions_tests, boxing_string_split_test2);
    tcase_add_test(tc_string_functions_tests, boxing_string_split_test3);
    tcase_add_test(tc_string_functions_tests, boxing_string_split_test4);
    tcase_add_test(tc_string_functions_tests, boxing_string_split_test5);
    tcase_add_test(tc_string_functions_tests, boxing_string_split_test6);
    tcase_add_test(tc_string_functions_tests, boxing_string_split_test7);
    tcase_add_test(tc_string_functions_tests, boxing_string_split_test8);
    tcase_add_test(tc_string_functions_tests, boxing_string_split_test9);
    tcase_add_test(tc_string_functions_tests, boxing_string_split_test10);
    tcase_add_test(tc_string_functions_tests, boxing_string_split_test11);
    tcase_add_test(tc_string_functions_tests, boxing_string_split_test12);
    // Test function boxing_string_to_integer
    tcase_add_test(tc_string_functions_tests, boxing_string_to_integer_test1);
    tcase_add_test(tc_string_functions_tests, boxing_string_to_integer_test2);
    tcase_add_test(tc_string_functions_tests, boxing_string_to_integer_test3);
    tcase_add_test(tc_string_functions_tests, boxing_string_to_integer_test4);

    Suite *s;
    s = suite_create("string_test_util");
    suite_add_tcase(s, tc_string_functions_tests);

    return s;
}
