#pragma GCC diagnostic ignored "-Wunused-variable"

#include "unittests.h"
#include "configtests.h"
#include "boxing/config.h"
#include "boxing/globals.h"
#include "boxing/utils.h"
#include "boxing/string.h"

static void setup();
static void tearDown();
static void do_test();

static const char * test_group = "test_group";
static const char * test_key_string = "test_key_string";
static const char * test_key_string_value = "test";
static const char * test_key_uint  = "test_key_uint";
static unsigned int test_key_uint_value = 10;
static const char * test_key_pointi = "test_key_pointi";
static const char * test_key_pointi_value = "-12,10";
static const char * test_key_pointf = "test_key_pointf";
static const char * test_key_pointf_value = "-12.0f,10.0f";

BOXING_START_TEST( config_test_base )
{
    setup();
    do_test();
    tearDown();
}
END_TEST

static void do_test()
{
    boxing_config * config = boxing_config_create();
    BOXING_ASSERT(config != NULL);

    boxing_config_set_property(config, test_group, test_key_string, test_key_string_value);
    boxing_config_set_property_uint(config, test_group, test_key_uint, test_key_uint_value);
    boxing_config_set_property(config, test_group, test_key_pointi, test_key_pointi_value);
    boxing_config_set_property(config, test_group, test_key_pointf, test_key_pointf_value);
    
    BOXING_ASSERT(boxing_config_property(config, test_group, "nonexisting") == NULL);
    BOXING_ASSERT(strcmp(boxing_config_property(config, test_group, test_key_string), test_key_string_value ) == 0);
    BOXING_ASSERT(boxing_config_property_int(config, test_group, test_key_uint) == (int)test_key_uint_value);
    BOXING_ASSERT(boxing_config_property_uint(config, test_group, test_key_uint) == test_key_uint_value);
    BOXING_ASSERT(boxing_config_is_set(config, test_group, test_key_string));
    BOXING_ASSERT(boxing_config_is_set(config, test_group, test_key_uint));
    BOXING_ASSERT(boxing_config_is_set(config, test_group, "nonexisting") == DFALSE);

    DBOOL found;
    boxing_pointi point_i = boxing_config_property_pointi(config, test_group, test_key_pointi, &found);
    BOXING_ASSERT(found);
    BOXING_ASSERT(point_i.x == -12);
    BOXING_ASSERT(point_i.y ==  10);
    boxing_pointf point_f = boxing_config_property_pointf(config, test_group, test_key_pointf, &found);
    BOXING_ASSERT(found);
    BOXING_ASSERT(point_f.x == -12.0f);
    BOXING_ASSERT(point_f.y ==  10.0f);
        
    const GHashTable * properties;
    boxing_config_properties(config, test_group, &properties);
    BOXING_ASSERT(g_hash_table_size((GHashTable*)properties) == 4);
    
    boxing_config * config_clone = boxing_config_clone(config);
    BOXING_ASSERT(boxing_config_is_equal(config, config_clone));

    gvector * vector = boxing_config_parse_list_properties(config, test_group, test_key_pointi);
    BOXING_ASSERT(vector->size == 2);
    gvector_free(vector);
    
    boxing_config_free(config);
    boxing_config_free(config_clone);
}

static void setup()
{
}

static void tearDown()
{
}

Suite * config_test(void)
{
    TCase *tc;
    tc = tcase_create("base");
    tcase_add_test(tc, config_test_base);

    Suite *s;
    s = suite_create("config");
    suite_add_tcase(s, tc);
 
    return s;
}
