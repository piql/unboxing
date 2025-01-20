#pragma GCC diagnostic ignored "-Wunused-variable"

#include "unittests.h"
#include "boxing/metadata.h"
#include "boxing/utils.h"

static void append_item_to_list(boxing_metadata_list *temp_list, boxing_metadata_type type)
{
    boxing_metadata_item *temp_item = boxing_metadata_item_create(type);
    boxing_metadata_list_append_item(temp_list, temp_item);
}


static void check_item(boxing_metadata_list *temp_list, boxing_metadata_type *key)
{
    boxing_metadata_item *temp_item = g_hash_table_lookup(temp_list, key);
    BOXING_ASSERT(temp_item != NULL);
    BOXING_ASSERT(temp_item->base.type == *key);
}



// Tests for file boxing\metadata.h

//  
//  FUNCTIONS Tests
// 

// Test to creating a new list of metadata
BOXING_START_TEST(BOXING_metadata_list_create_test1)
{
    boxing_metadata_list *temp_list;
    temp_list = boxing_metadata_list_create();

    BOXING_ASSERT(temp_list != NULL);
    BOXING_ASSERT(temp_list->nnodes == 0);
    BOXING_ASSERT(temp_list->hash_func != NULL);
    BOXING_ASSERT(temp_list->key_equal_func != NULL);
    BOXING_ASSERT(temp_list->key_destroy_func == NULL);
    BOXING_ASSERT(temp_list->value_destroy_func != NULL);

    boxing_metadata_list_free(temp_list);
}
END_TEST


// Test to creating a new item of metadata with all types
BOXING_START_TEST(BOXING_metadata_items_create)
{
    struct types_
    {
        uint16_t type;
        uint16_t size;
    } types[] =
    {
        { BOXING_METADATA_TYPE_UNKNOWN, 0 },
        { BOXING_METADATA_TYPE_ENDOFDATA, 0 },
        { BOXING_METADATA_TYPE_JOBID, 4 },
        { BOXING_METADATA_TYPE_FRAMENUMBER, 4 },
        { BOXING_METADATA_TYPE_FILEID, 4 },
        { BOXING_METADATA_TYPE_FILESIZE, 8 },
        { BOXING_METADATA_TYPE_DATACRC, 8 },
        { BOXING_METADATA_TYPE_DATASIZE, 4 },
        { BOXING_METADATA_TYPE_SYMBOLSPERPIXEL, 2 },
        { BOXING_METADATA_TYPE_CONTENTTYPE, 2 },
        { BOXING_METADATA_TYPE_CIPHERKEY, 4 },
        { BOXING_METADATA_TYPE_CONTENTSYMBOLSIZE, 2 },
        { BOXING_METADATA_TYPE_LASTTYPE, 0 },
        { 25, 0 }
    };

    int type_count = sizeof(types) / sizeof(types[0]);

    for (int i = 0; i < type_count; i++)
    {
        boxing_metadata_item *temp_item;
        temp_item = boxing_metadata_item_create(types[i].type);

        BOXING_ASSERT(temp_item->base.size == types[i].size);
        BOXING_ASSERT(temp_item->base.type == types[i].type);
        BOXING_ASSERT(temp_item->base.serialize != NULL);
        BOXING_ASSERT(temp_item->base.deserialize != NULL);
        BOXING_ASSERT(temp_item->base.free != NULL);

        boxing_metadata_item_free(temp_item);
    }
}
END_TEST


// Test to getting string representation of item type BOXING_METADATA_TYPE_UNKNOWN
BOXING_START_TEST(BOXING_metadata_type_name_test1)
{
    const char * temp_char = boxing_metadata_type_name(BOXING_METADATA_TYPE_UNKNOWN);

    BOXING_ASSERT_STR_EQ(temp_char, "Unknown");
}
END_TEST


// Test to getting string representation of item type when type is out of range
BOXING_START_TEST(BOXING_metadata_type_name_test2)
{
    const char * temp_char = boxing_metadata_type_name(18);

    BOXING_ASSERT_STR_EQ(temp_char, "Unknown");
}
END_TEST


// Test to getting string representation of item type BOXING_METADATA_TYPE_ENDOFDATA
BOXING_START_TEST(BOXING_metadata_type_name_test3)
{
    const char * temp_char = boxing_metadata_type_name(BOXING_METADATA_TYPE_ENDOFDATA);

    BOXING_ASSERT_STR_EQ(temp_char, "EndOfData");
}
END_TEST


// Test to getting string representation of item type BOXING_METADATA_TYPE_JOBID
BOXING_START_TEST(BOXING_metadata_type_name_test4)
{
    const char * temp_char = boxing_metadata_type_name(BOXING_METADATA_TYPE_JOBID);

    BOXING_ASSERT_STR_EQ(temp_char, "JobId");
}
END_TEST


// Test to getting string representation of item type BOXING_METADATA_TYPE_FRAMENUMBER
BOXING_START_TEST(BOXING_metadata_type_name_test5)
{
    const char * temp_char = boxing_metadata_type_name(BOXING_METADATA_TYPE_FRAMENUMBER);

    BOXING_ASSERT_STR_EQ(temp_char, "FrameNumber");
}
END_TEST


// Test to getting string representation of item type BOXING_METADATA_TYPE_FILEID
BOXING_START_TEST(BOXING_metadata_type_name_test6)
{
    const char * temp_char = boxing_metadata_type_name(BOXING_METADATA_TYPE_FILEID);

    BOXING_ASSERT_STR_EQ(temp_char, "FileId");
}
END_TEST


// Test to getting string representation of item type BOXING_METADATA_TYPE_FILESIZE
BOXING_START_TEST(BOXING_metadata_type_name_test7)
{
    const char * temp_char = boxing_metadata_type_name(BOXING_METADATA_TYPE_FILESIZE);

    BOXING_ASSERT_STR_EQ(temp_char, "FileSize");
}
END_TEST


// Test to getting string representation of item type BOXING_METADATA_TYPE_DATACRC
BOXING_START_TEST(BOXING_metadata_type_name_test8)
{
    const char * temp_char = boxing_metadata_type_name(BOXING_METADATA_TYPE_DATACRC);

    BOXING_ASSERT_STR_EQ(temp_char, "DataCrc");
}
END_TEST


// Test to getting string representation of item type BOXING_METADATA_TYPE_DATASIZE
BOXING_START_TEST(BOXING_metadata_type_name_test9)
{
    const char * temp_char = boxing_metadata_type_name(BOXING_METADATA_TYPE_DATASIZE);

    BOXING_ASSERT_STR_EQ(temp_char, "DataSize");
}
END_TEST


// Test to getting string representation of item type BOXING_METADATA_TYPE_SYMBOLSPERPIXEL
BOXING_START_TEST(BOXING_metadata_type_name_test10)
{
    const char * temp_char = boxing_metadata_type_name(BOXING_METADATA_TYPE_SYMBOLSPERPIXEL);

    BOXING_ASSERT_STR_EQ(temp_char, "SymbolsPerPixel");
}
END_TEST


// Test to getting string representation of item type BOXING_METADATA_TYPE_CONTENTTYPE
BOXING_START_TEST(BOXING_metadata_type_name_test11)
{
    const char * temp_char = boxing_metadata_type_name(BOXING_METADATA_TYPE_CONTENTTYPE);

    BOXING_ASSERT_STR_EQ(temp_char, "ContentType");
}
END_TEST


// Test to getting string representation of item type BOXING_METADATA_TYPE_CIPHERKEY
BOXING_START_TEST(BOXING_metadata_type_name_test12)
{
    const char * temp_char = boxing_metadata_type_name(BOXING_METADATA_TYPE_CIPHERKEY);

    BOXING_ASSERT_STR_EQ(temp_char, "CipherKey");
}
END_TEST


// Test to getting string representation of item type BOXING_METADATA_TYPE_CONTENTSYMBOLSIZE
BOXING_START_TEST(BOXING_metadata_type_name_test13)
{
    const char * temp_char = boxing_metadata_type_name(BOXING_METADATA_TYPE_CONTENTSYMBOLSIZE);

    BOXING_ASSERT_STR_EQ(temp_char, "ContentSymbolSize");
}
END_TEST


// Test to getting string representation of item type BOXING_METADATA_TYPE_LASTTYPE
BOXING_START_TEST(BOXING_metadata_type_name_test14)
{
    const char * temp_char = boxing_metadata_type_name(BOXING_METADATA_TYPE_LASTTYPE);

    BOXING_ASSERT_STR_EQ(temp_char, "Unknown");
}
END_TEST


// Test when adding to the hash table list and item are both NULL
BOXING_START_TEST(BOXING_metadata_list_append_item_test1)
{
    boxing_metadata_list *temp_list = NULL;
    boxing_metadata_item *temp_item = NULL;

    boxing_metadata_list_append_item(temp_list, temp_item);

    BOXING_ASSERT(temp_list == NULL);
    BOXING_ASSERT(temp_item == NULL);
}
END_TEST


// Test when adding to the hash table item is NULL
BOXING_START_TEST(BOXING_metadata_list_append_item_test2)
{
    boxing_metadata_list *temp_list = boxing_metadata_list_create();
    boxing_metadata_item *temp_item = NULL;

    boxing_metadata_list_append_item(temp_list, temp_item);

    BOXING_ASSERT(temp_list->nnodes == 0);
    BOXING_ASSERT(temp_item == NULL);

    boxing_metadata_list_free(temp_list);
}
END_TEST


// Test when adding to the hash table list is NULL
BOXING_START_TEST(BOXING_metadata_list_append_item_test3)
{
    boxing_metadata_list *temp_list = NULL;
    boxing_metadata_item *temp_item = boxing_metadata_item_create(BOXING_METADATA_TYPE_LASTTYPE);

    boxing_metadata_list_append_item(temp_list, temp_item);

    BOXING_ASSERT(temp_list == NULL);
    BOXING_ASSERT(temp_item->base.type == BOXING_METADATA_TYPE_LASTTYPE);

    boxing_metadata_item_free(temp_item);
}
END_TEST


// Test to add to a hash table one item with type BOXING_METADATA_TYPE_FILEID
BOXING_START_TEST(BOXING_metadata_list_append_item_test4)
{
    boxing_metadata_type key = BOXING_METADATA_TYPE_FILEID;

    boxing_metadata_list *temp_list = boxing_metadata_list_create();
    append_item_to_list(temp_list, key);

    BOXING_ASSERT(temp_list->nnodes == 1);

    check_item(temp_list, &key);

    boxing_metadata_list_free(temp_list);
}
END_TEST


// Test to add to a hash table one item with type BOXING_METADATA_TYPE_UNKNOWN
BOXING_START_TEST(BOXING_metadata_list_append_item_test5)
{
    boxing_metadata_type key = BOXING_METADATA_TYPE_UNKNOWN;

    boxing_metadata_list *temp_list = boxing_metadata_list_create();
    append_item_to_list(temp_list, key);

    BOXING_ASSERT(temp_list->nnodes == 1);

    check_item(temp_list, &key);

    boxing_metadata_list_free(temp_list);
}
END_TEST


// Test to add to a hash table two identical items with type BOXING_METADATA_TYPE_FRAMENUMBER
BOXING_START_TEST(BOXING_metadata_list_append_item_test6)
{
    boxing_metadata_type key1 = BOXING_METADATA_TYPE_FRAMENUMBER;
    boxing_metadata_type key2 = BOXING_METADATA_TYPE_FRAMENUMBER;

    boxing_metadata_list *temp_list = boxing_metadata_list_create();
    append_item_to_list(temp_list, key1);
    append_item_to_list(temp_list, key2);

    BOXING_ASSERT(temp_list->nnodes == 1);

    check_item(temp_list, &key1);

    boxing_metadata_list_free(temp_list);
}
END_TEST


// Test to add to a hash table two different items with types BOXING_METADATA_TYPE_FRAMENUMBER and BOXING_METADATA_TYPE_FILESIZE
BOXING_START_TEST(BOXING_metadata_list_append_item_test7)
{
    boxing_metadata_type key1 = BOXING_METADATA_TYPE_FRAMENUMBER;
    boxing_metadata_type key2 = BOXING_METADATA_TYPE_FILESIZE;

    boxing_metadata_list *temp_list = boxing_metadata_list_create();
    append_item_to_list(temp_list, key1);
    append_item_to_list(temp_list, key2);

    BOXING_ASSERT(temp_list->nnodes == 2);

    check_item(temp_list, &key1);
    check_item(temp_list, &key2);

    boxing_metadata_list_free(temp_list);
}
END_TEST


// Test to add to a hash table three items two of which are identical
BOXING_START_TEST(BOXING_metadata_list_append_item_test8)
{
    boxing_metadata_type key1 = BOXING_METADATA_TYPE_CONTENTTYPE;
    boxing_metadata_type key2 = BOXING_METADATA_TYPE_LASTTYPE;

    boxing_metadata_list *temp_list = boxing_metadata_list_create();
    append_item_to_list(temp_list, key1);
    append_item_to_list(temp_list, key2);
    append_item_to_list(temp_list, key2);

    BOXING_ASSERT(temp_list->nnodes == 2);

    check_item(temp_list, &key1);
    check_item(temp_list, &key2);

    boxing_metadata_list_free(temp_list);
}
END_TEST


// Test to add to a hash table all types of items
BOXING_START_TEST(BOXING_metadata_list_append_item_test9)
{
    boxing_metadata_list *temp_list = boxing_metadata_list_create();

    boxing_metadata_type type = BOXING_METADATA_TYPE_ENDOFDATA;
    while (type <= BOXING_METADATA_TYPE_LASTTYPE)
    {
        append_item_to_list(temp_list, type);
        type++;
    }
    append_item_to_list(temp_list, BOXING_METADATA_TYPE_UNKNOWN);

    BOXING_ASSERT(temp_list->nnodes == 13);
    boxing_metadata_type key = BOXING_METADATA_TYPE_ENDOFDATA;
    while (key <= BOXING_METADATA_TYPE_LASTTYPE)
    {
        check_item(temp_list, &key);
        key++;
    }
    key = BOXING_METADATA_TYPE_UNKNOWN;
    check_item(temp_list, &key);

    boxing_metadata_list_free(temp_list);
}
END_TEST


// Test to determine whether the item is contained in the list
BOXING_START_TEST(BOXING_metadata_list_contains_item_test1)
{
    boxing_metadata_list *temp_list = boxing_metadata_list_create();

    boxing_metadata_type type = BOXING_METADATA_TYPE_UNKNOWN;
    while (type <= BOXING_METADATA_TYPE_LASTTYPE)
    {
        boxing_metadata_item *temp_item = boxing_metadata_item_create(type);
        boxing_metadata_list_append_item(temp_list, temp_item);
        type++;
    }

    boxing_metadata_type key = BOXING_METADATA_TYPE_UNKNOWN;
    while (key <= BOXING_METADATA_TYPE_LASTTYPE)
    {
        BOXING_ASSERT(boxing_metadata_list_contains_item(temp_list, key) == 1);
        key++;
    }

    boxing_metadata_list_free(temp_list);
}
END_TEST


// Test to determine whether the item is not contained in the list
BOXING_START_TEST(BOXING_metadata_list_contains_item_test2)
{
    boxing_metadata_list *temp_list = boxing_metadata_list_create();

    boxing_metadata_type key = BOXING_METADATA_TYPE_UNKNOWN;

    while (key <= BOXING_METADATA_TYPE_LASTTYPE)
    {
        BOXING_ASSERT(boxing_metadata_list_contains_item(temp_list, key) == 0);
        key++;
    }

    boxing_metadata_list_free(temp_list);
}
END_TEST


// Test to determine whether the item is contained or not contained in the list
BOXING_START_TEST(BOXING_metadata_list_contains_item_test3)
{
    boxing_metadata_list *temp_list = boxing_metadata_list_create();
    boxing_metadata_item *temp_item = boxing_metadata_item_create(BOXING_METADATA_TYPE_CONTENTTYPE);
    boxing_metadata_list_append_item(temp_list, temp_item);

    boxing_metadata_type key = BOXING_METADATA_TYPE_UNKNOWN;
    while (key <= BOXING_METADATA_TYPE_LASTTYPE)
    {
        if (key == BOXING_METADATA_TYPE_CONTENTTYPE)
        {
            BOXING_ASSERT(boxing_metadata_list_contains_item(temp_list, key) == 1);
        }
        else
        {
            BOXING_ASSERT(boxing_metadata_list_contains_item(temp_list, key) == 0);
        }
        key++;
    }

    boxing_metadata_list_free(temp_list);
}
END_TEST


// Test to find all items in the list
BOXING_START_TEST(BOXING_metadata_list_find_item_test1)
{
    boxing_metadata_list *temp_list = boxing_metadata_list_create();

    boxing_metadata_type type = BOXING_METADATA_TYPE_UNKNOWN;
    while (type <= BOXING_METADATA_TYPE_LASTTYPE)
    {
        boxing_metadata_item *temp_item = boxing_metadata_item_create(type++);
        boxing_metadata_list_append_item(temp_list, temp_item);
    }

    boxing_metadata_type key = BOXING_METADATA_TYPE_UNKNOWN;
    while (key <= BOXING_METADATA_TYPE_LASTTYPE)
    {
        boxing_metadata_item *temp_item = boxing_metadata_list_find_item(temp_list, key);
        BOXING_ASSERT(temp_item != NULL);
        BOXING_ASSERT(temp_item->base.type == key);
        key++;
    }

    boxing_metadata_list_free(temp_list);
}
END_TEST


// Test to not find items in the list
BOXING_START_TEST(BOXING_metadata_list_find_item_test2)
{
    boxing_metadata_list *temp_list = boxing_metadata_list_create();

    boxing_metadata_type key = BOXING_METADATA_TYPE_UNKNOWN;

    while (key <= BOXING_METADATA_TYPE_LASTTYPE)
    {
        boxing_metadata_item *temp_item = boxing_metadata_list_find_item(temp_list, key);
        BOXING_ASSERT(temp_item == NULL);
        key++;
    }

    boxing_metadata_list_free(temp_list);
}
END_TEST


// Mixed test to find or not find items in the list
BOXING_START_TEST(BOXING_metadata_list_find_item_test3)
{
    boxing_metadata_list *temp_list = boxing_metadata_list_create();
    boxing_metadata_item *temp_item = boxing_metadata_item_create(BOXING_METADATA_TYPE_DATACRC);
    boxing_metadata_list_append_item(temp_list, temp_item);

    boxing_metadata_type key = BOXING_METADATA_TYPE_UNKNOWN;
    while (key <= BOXING_METADATA_TYPE_LASTTYPE)
    {
        boxing_metadata_item *temp_item = boxing_metadata_list_find_item(temp_list, key);

        if (key == BOXING_METADATA_TYPE_DATACRC)
        {
            BOXING_ASSERT(temp_item != NULL);
            BOXING_ASSERT(temp_item->base.type == key);
        }
        else
        {
            BOXING_ASSERT(temp_item == NULL);
        }

        key++;
    }

    boxing_metadata_list_free(temp_list);
}
END_TEST


// Test to clear empty list
BOXING_START_TEST(BOXING_metadata_list_clear_test1)
{
    boxing_metadata_list *temp_list = boxing_metadata_list_create();

    boxing_metadata_list_clear(temp_list);
    BOXING_ASSERT(temp_list != NULL);
    BOXING_ASSERT(temp_list->nnodes == 0);

    boxing_metadata_list_free(temp_list);
}
END_TEST


// Test to clear filled list, and check items after it
BOXING_START_TEST(BOXING_metadata_list_clear_test2)
{
    boxing_metadata_list *temp_list = boxing_metadata_list_create();
    boxing_metadata_item *temp_item[12];

    boxing_metadata_type type = BOXING_METADATA_TYPE_ENDOFDATA;
    while (type <= BOXING_METADATA_TYPE_LASTTYPE)
    {
        temp_item[type] = boxing_metadata_item_create(type);
        boxing_metadata_list_append_item(temp_list, temp_item[type]);
        type++;
    }

    BOXING_ASSERT(temp_list->nnodes == 12);

    boxing_metadata_list_clear(temp_list);

    BOXING_ASSERT(temp_list != NULL);
    BOXING_ASSERT(temp_list->nnodes == 0);

    boxing_metadata_list_free(temp_list);
}
END_TEST


// Test to destroy empty list
BOXING_START_TEST(BOXING_metadata_list_free_test1)
{
    boxing_metadata_list *temp_list = boxing_metadata_list_create();

    int size1 = g_hash_table_size(temp_list);
    BOXING_ASSERT(size1 == 0);
    boxing_metadata_list_free(temp_list);
}
END_TEST


// Test to destroy list with twelve items
BOXING_START_TEST(BOXING_metadata_list_free_test2)
{
    boxing_metadata_list *temp_list = boxing_metadata_list_create(); // Create a new list

    boxing_metadata_type type = BOXING_METADATA_TYPE_ENDOFDATA; // Set start type

    // Filling the list of items
    while (type <= BOXING_METADATA_TYPE_LASTTYPE)
    {
        append_item_to_list(temp_list, type);
        type++;
    }

    int size1 = g_hash_table_size(temp_list);
    BOXING_ASSERT(size1 == 12); // Check number of items in the list

    boxing_metadata_list_free(temp_list); // Destroy list
}
END_TEST


// Test to serialize and deserialize empty list
BOXING_START_TEST(BOXING_serialize_deserialize_test1)
{
    boxing_metadata_list *temp_list = boxing_metadata_list_create();

    gvector * temp_vector = boxing_metadata_list_serialize(temp_list);

    boxing_metadata_list *temp_list2 = boxing_metadata_list_create();
    unsigned int item_count = boxing_metadata_list_deserialize(temp_list2, (char *)temp_vector->buffer, temp_vector->item_size * temp_vector->size);

    BOXING_ASSERT(g_hash_table_size(temp_list) == item_count);

    boxing_metadata_list_free(temp_list);
    boxing_metadata_list_free(temp_list2);
    gvector_free(temp_vector);
}
END_TEST


// Test to serialize and deserialize list with one item
BOXING_START_TEST(BOXING_serialize_deserialize_test2)
{
    boxing_metadata_list *temp_list = boxing_metadata_list_create();
    boxing_metadata_item *temp_item = boxing_metadata_item_create(BOXING_METADATA_TYPE_DATACRC);
    boxing_metadata_list_append_item(temp_list, temp_item);

    gvector * temp_vector = boxing_metadata_list_serialize(temp_list);

    boxing_metadata_list *temp_list2 = boxing_metadata_list_create();
    unsigned int item_count = boxing_metadata_list_deserialize(temp_list2, (char *)temp_vector->buffer, temp_vector->item_size * temp_vector->size);

    BOXING_ASSERT(g_hash_table_size(temp_list) == item_count);
    BOXING_ASSERT(boxing_metadata_list_contains_item(temp_list2, BOXING_METADATA_TYPE_DATACRC) == 1);

    boxing_metadata_list_free(temp_list);
    boxing_metadata_list_free(temp_list2);
    gvector_free(temp_vector);
}
END_TEST


// Test to serialize and deserialize list with all types of item
BOXING_START_TEST(BOXING_serialize_deserialize_test3)
{
    boxing_metadata_list *temp_list = boxing_metadata_list_create(); // Create a new list
    boxing_metadata_item *temp_item[BOXING_METADATA_TYPE_LASTTYPE];
    unsigned int type_count = 0;

    boxing_metadata_type type = 0;
    while (type < BOXING_METADATA_TYPE_LASTTYPE)
    {
        if (type != BOXING_METADATA_TYPE_ENDOFDATA)
        {
            temp_item[type_count] = boxing_metadata_item_create(type);
            boxing_metadata_list_append_item(temp_list, temp_item[type_count]);
            type_count++;
        }

        type++;
    }
    BOXING_ASSERT(BOXING_METADATA_TYPE_LASTTYPE-1 == type_count);
    BOXING_ASSERT(g_hash_table_size(temp_list) == type_count);

    gvector * temp_vector = boxing_metadata_list_serialize(temp_list);

    boxing_metadata_list *temp_list2 = boxing_metadata_list_create();
    unsigned int item_count = boxing_metadata_list_deserialize(temp_list2, (char *)temp_vector->buffer, temp_vector->item_size * temp_vector->size);

    BOXING_ASSERT(g_hash_table_size(temp_list) == item_count);

    GHashTableIter iter1, iter2;
    gpointer key1, key2, value1, value2;

    g_hash_table_iter_init(&iter1, temp_list);
    g_hash_table_iter_init(&iter2, temp_list2);

    while (g_hash_table_iter_next(&iter1, &key1, &value1) && g_hash_table_iter_next(&iter2, &key2, &value2))
    {
        boxing_metadata_item * item1 = (boxing_metadata_item *)value1;
        boxing_metadata_item * item2 = (boxing_metadata_item *)value2;

        BOXING_ASSERT(item1->base.type == item2->base.type);
        BOXING_ASSERT(item1->base.size == item2->base.size);
    }

    BOXING_ASSERT(g_hash_table_size(temp_list) == type_count);
    BOXING_ASSERT(g_hash_table_size(temp_list2) == type_count);
    for (int t=1; t < BOXING_METADATA_TYPE_LASTTYPE; t++)
    {
        BOXING_ASSERT_MESSAGE(boxing_metadata_list_contains_item(temp_list, t) != 0, "source list: type=%d not found", t);
        BOXING_ASSERT_MESSAGE(boxing_metadata_list_contains_item(temp_list2, t) != 0, "destination list: type=%d not found", t);
    }
    
    boxing_metadata_list_free(temp_list);
    boxing_metadata_list_free(temp_list2);
    gvector_free(temp_vector);
}
END_TEST



Suite * metadata_tests(void)
{
    TCase *tc_meta_data_functions_tests;
    tc_meta_data_functions_tests = tcase_create("functions_tests");
    // Test function boxing_metadata_list_create
    tcase_add_test(tc_meta_data_functions_tests, BOXING_metadata_list_create_test1);

    // Test function boxing_metadata_item_create
    tcase_add_test(tc_meta_data_functions_tests, BOXING_metadata_items_create);

    // Test function boxing_metadata_type_name
    tcase_add_test(tc_meta_data_functions_tests, BOXING_metadata_type_name_test1);
    tcase_add_test(tc_meta_data_functions_tests, BOXING_metadata_type_name_test2);
    tcase_add_test(tc_meta_data_functions_tests, BOXING_metadata_type_name_test3);
    tcase_add_test(tc_meta_data_functions_tests, BOXING_metadata_type_name_test4);
    tcase_add_test(tc_meta_data_functions_tests, BOXING_metadata_type_name_test5);
    tcase_add_test(tc_meta_data_functions_tests, BOXING_metadata_type_name_test6);
    tcase_add_test(tc_meta_data_functions_tests, BOXING_metadata_type_name_test7);
    tcase_add_test(tc_meta_data_functions_tests, BOXING_metadata_type_name_test8);
    tcase_add_test(tc_meta_data_functions_tests, BOXING_metadata_type_name_test9);
    tcase_add_test(tc_meta_data_functions_tests, BOXING_metadata_type_name_test10);
    tcase_add_test(tc_meta_data_functions_tests, BOXING_metadata_type_name_test11);
    tcase_add_test(tc_meta_data_functions_tests, BOXING_metadata_type_name_test12);
    tcase_add_test(tc_meta_data_functions_tests, BOXING_metadata_type_name_test13);
    tcase_add_test(tc_meta_data_functions_tests, BOXING_metadata_type_name_test14);

    // Test function boxing_metadata_list_append_item
    tcase_add_test(tc_meta_data_functions_tests, BOXING_metadata_list_append_item_test1);
    tcase_add_test(tc_meta_data_functions_tests, BOXING_metadata_list_append_item_test2);
    tcase_add_test(tc_meta_data_functions_tests, BOXING_metadata_list_append_item_test3);
    tcase_add_test(tc_meta_data_functions_tests, BOXING_metadata_list_append_item_test4);
    tcase_add_test(tc_meta_data_functions_tests, BOXING_metadata_list_append_item_test5);
    tcase_add_test(tc_meta_data_functions_tests, BOXING_metadata_list_append_item_test6);
    tcase_add_test(tc_meta_data_functions_tests, BOXING_metadata_list_append_item_test7);
    tcase_add_test(tc_meta_data_functions_tests, BOXING_metadata_list_append_item_test8);
    tcase_add_test(tc_meta_data_functions_tests, BOXING_metadata_list_append_item_test9);

    // Test function boxing_metadata_list_contains_item
    tcase_add_test(tc_meta_data_functions_tests, BOXING_metadata_list_contains_item_test1);
    tcase_add_test(tc_meta_data_functions_tests, BOXING_metadata_list_contains_item_test2);
    tcase_add_test(tc_meta_data_functions_tests, BOXING_metadata_list_contains_item_test3);

    // Test function boxing_metadata_list_find_item
    tcase_add_test(tc_meta_data_functions_tests, BOXING_metadata_list_find_item_test1);
    tcase_add_test(tc_meta_data_functions_tests, BOXING_metadata_list_find_item_test2);
    tcase_add_test(tc_meta_data_functions_tests, BOXING_metadata_list_find_item_test3);

    // Test function boxing_metadata_list_clear
    tcase_add_test(tc_meta_data_functions_tests, BOXING_metadata_list_clear_test1);
    tcase_add_test(tc_meta_data_functions_tests, BOXING_metadata_list_clear_test2);

    // Test function boxing_metadata_list_free
    tcase_add_test(tc_meta_data_functions_tests, BOXING_metadata_list_free_test1);
    tcase_add_test(tc_meta_data_functions_tests, BOXING_metadata_list_free_test2);

    // Test function boxing_metadata_list_serialize and boxing_metadata_list_deserialize
    tcase_add_test(tc_meta_data_functions_tests, BOXING_serialize_deserialize_test1);
    tcase_add_test(tc_meta_data_functions_tests, BOXING_serialize_deserialize_test2);
    tcase_add_test(tc_meta_data_functions_tests, BOXING_serialize_deserialize_test3);

    Suite *s;
    s = suite_create("meta_data_test_util");
    suite_add_tcase(s, tc_meta_data_functions_tests);

    return s;
}
