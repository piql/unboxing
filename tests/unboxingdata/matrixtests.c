#pragma GCC diagnostic ignored "-Wunused-variable"

#include "unittests.h"
#include "boxing/matrix.h"
#include "boxing/utils.h"


static void fill_matrixf_random(boxing_matrixf *temp_matrix)
{
    int size1 = temp_matrix->width * temp_matrix->height;
    for (int i = 0; i < size1; i++)
    {
        temp_matrix->data[i].x = ((float)rand() / (float)(RAND_MAX)) * 255.0f;
        temp_matrix->data[i].y = ((float)rand() / (float)(RAND_MAX)) * 255.0f;
    }
}


static void check_matrixf(boxing_matrixf *temp_matrix, unsigned int width, unsigned int height, DBOOL is_owning_data)
{
    BOXING_ASSERT(temp_matrix != NULL);
    BOXING_ASSERT(temp_matrix->width == width);
    BOXING_ASSERT(temp_matrix->height == height);
    BOXING_ASSERT(temp_matrix->is_owning_data == is_owning_data);
    BOXING_ASSERT(temp_matrix->data != NULL);
}


static void check_matrixf_width_height(boxing_matrixf *temp_matrix, unsigned int width, unsigned int height)
{
    BOXING_ASSERT(temp_matrix != NULL);
    BOXING_ASSERT(temp_matrix->width == width);
    BOXING_ASSERT(temp_matrix->height == height);
}


static void check_data_equal_matricesf(boxing_matrixf *temp_matrix1, boxing_matrixf *temp_matrix2)
{
    BOXING_ASSERT(temp_matrix1->width == temp_matrix2->width);
    BOXING_ASSERT(temp_matrix1->height == temp_matrix2->height);

    int size = temp_matrix1->width * temp_matrix1->height;

    for (int i = 0; i < size; i++)
    {
        BOXING_ASSERT(temp_matrix1->data[i].x == temp_matrix2->data[i].x);
        BOXING_ASSERT(temp_matrix1->data[i].y == temp_matrix2->data[i].y);
    }
}


static void fill_matrixi_random(boxing_matrixi *temp_matrix)
{
    int size1 = temp_matrix->width * temp_matrix->height;
    for (int i = 0; i < size1; i++)
    {
        temp_matrix->data[i].x = rand() % 256;
        temp_matrix->data[i].y = rand() % 256;
    }
}


static void check_matrixi(boxing_matrixi *temp_matrix, unsigned int width, unsigned int height, DBOOL is_owning_data)
{
    BOXING_ASSERT(temp_matrix != NULL);
    BOXING_ASSERT(temp_matrix->width == width);
    BOXING_ASSERT(temp_matrix->height == height);
    BOXING_ASSERT(temp_matrix->is_owning_data == is_owning_data);
    BOXING_ASSERT(temp_matrix->data != NULL);
}


static void check_matrixi_width_height(boxing_matrixi *temp_matrix, unsigned int width, unsigned int height)
{
    BOXING_ASSERT(temp_matrix != NULL);
    BOXING_ASSERT(temp_matrix->width == width);
    BOXING_ASSERT(temp_matrix->height == height);
}


static void check_data_equal_matricesi(boxing_matrixi *temp_matrix1, boxing_matrixi *temp_matrix2)
{
    BOXING_ASSERT(temp_matrix1->width == temp_matrix2->width);
    BOXING_ASSERT(temp_matrix1->height == temp_matrix2->height);

    int size = temp_matrix1->width * temp_matrix1->height;

    for (int i = 0; i < size; i++)
    {
        BOXING_ASSERT(temp_matrix1->data[i].x == temp_matrix2->data[i].x);
        BOXING_ASSERT(temp_matrix1->data[i].y == temp_matrix2->data[i].y);
    }
}


// Tests for file boxing/matrix.h

//  
//  FUNCTIONS Matrix Tests
// 

// Test to creating a new boxing_matrixf object with width and height equal to zero
BOXING_START_TEST(BOXING_matrixf_create_test1)
{
    boxing_matrixf *temp_matrixf = boxing_matrixf_create(0, 0);

    BOXING_ASSERT(temp_matrixf == NULL);
}
END_TEST


// Test to creating a new boxing_matrixf object with width equal to zero
BOXING_START_TEST(BOXING_matrixf_create_test2)
{
    boxing_matrixf *temp_matrixf = boxing_matrixf_create(0, 100);

    BOXING_ASSERT(temp_matrixf == NULL);
}
END_TEST


// Test to creating a new boxing_matrixf object with height equal to zero
BOXING_START_TEST(BOXING_matrixf_create_test3)
{
    boxing_matrixf *temp_matrixf = boxing_matrixf_create(100, 0);

    BOXING_ASSERT(temp_matrixf == NULL);
}
END_TEST


// Test to creating a new boxing_matrixf object with width and height not equal to zero
BOXING_START_TEST(BOXING_matrixf_create_test4)
{
    boxing_matrixf *temp_matrixf = boxing_matrixf_create(101, 102);

    check_matrixf(temp_matrixf, 101, 102, DTRUE);

    boxing_matrixf_free(temp_matrixf);
}
END_TEST


// Test to creating a new boxing_matrixf object with width and height equal to max unsigned int value
BOXING_START_TEST(BOXING_matrixf_create_test5)
{
    boxing_matrixf *temp_matrixf = boxing_matrixf_create(4294967295, 4294967295);

    check_matrixf(temp_matrixf, 4294967295, 4294967295, DTRUE);

    boxing_matrixf_free(temp_matrixf);
}
END_TEST


// Test to creating a new boxing_matrixi object with width and height equal to zero
BOXING_START_TEST(BOXING_matrixi_create_test1)
{
    boxing_matrixi *temp_matrixi = boxing_matrixi_create(0, 0);

    BOXING_ASSERT(temp_matrixi == NULL);
}
END_TEST


// Test to creating a new boxing_matrixi object with width equal to zero
BOXING_START_TEST(BOXING_matrixi_create_test2)
{
    boxing_matrixi *temp_matrixi = boxing_matrixi_create(0, 100);

    BOXING_ASSERT(temp_matrixi == NULL);
}
END_TEST


// Test to creating a new boxing_matrixi object with height equal to zero
BOXING_START_TEST(BOXING_matrixi_create_test3)
{
    boxing_matrixi *temp_matrixi = boxing_matrixi_create(100, 0);

    BOXING_ASSERT(temp_matrixi == NULL);
}
END_TEST


// Test to creating a new boxing_matrixi object with width and height not equal to zero
BOXING_START_TEST(BOXING_matrixi_create_test4)
{
    boxing_matrixi *temp_matrixi = boxing_matrixi_create(101, 102);

    check_matrixi(temp_matrixi, 101, 102, DTRUE);

    boxing_matrixi_free(temp_matrixi);
}
END_TEST


// Test to creating a new boxing_matrixi object with width and height equal to max unsigned int value
BOXING_START_TEST(BOXING_matrixi_create_test5)
{
    boxing_matrixi *temp_matrixi = boxing_matrixi_create(4294967295, 4294967295);

    check_matrixi(temp_matrixi, 4294967295, 4294967295, DTRUE);

    boxing_matrixi_free(temp_matrixi);
}
END_TEST


// Test to recreating a new boxing_matrixf object with width and height equal to zero
BOXING_START_TEST(BOXING_matrixf_recreate_test1)
{
    boxing_matrixf *temp_matrix1 = boxing_matrixf_create(101, 102);

    BOXING_ASSERT(temp_matrix1 != NULL);

    boxing_matrixf *temp_matrix2 = boxing_matrixf_recreate(temp_matrix1, 0, 0);

    BOXING_ASSERT(temp_matrix2 == NULL);

    boxing_matrixf_free(temp_matrix1);
}
END_TEST


// Test to recreating a new boxing_matrixf object with width equal to zero
BOXING_START_TEST(BOXING_matrixf_recreate_test2)
{
    boxing_matrixf *temp_matrix1 = boxing_matrixf_create(101, 102);

    BOXING_ASSERT(temp_matrix1 != NULL);

    boxing_matrixf *temp_matrix2 = boxing_matrixf_recreate(temp_matrix1, 0, 50);

    BOXING_ASSERT(temp_matrix2 == NULL);

    boxing_matrixf_free(temp_matrix1);
}
END_TEST


// Test to recreating a new boxing_matrixf object with height equal to zero
BOXING_START_TEST(BOXING_matrixf_recreate_test3)
{
    boxing_matrixf *temp_matrix1 = boxing_matrixf_create(101, 102);

    BOXING_ASSERT(temp_matrix1 != NULL);

    boxing_matrixf *temp_matrix2 = boxing_matrixf_recreate(temp_matrix1, 50, 0);

    BOXING_ASSERT(temp_matrix2 == NULL);

    boxing_matrixf_free(temp_matrix1);
}
END_TEST


// Test to recreating a new boxing_matrixf object when the previous matrix equal to NULL and width and height are equal to zero
BOXING_START_TEST(BOXING_matrixf_recreate_test4)
{
    boxing_matrixf *temp_matrix1 = boxing_matrixf_create(0, 0);

    BOXING_ASSERT(temp_matrix1 == NULL);

    boxing_matrixf *temp_matrix2 = boxing_matrixf_recreate(temp_matrix1, 0, 0);

    BOXING_ASSERT(temp_matrix2 == NULL);
}
END_TEST


// Test to recreating a new boxing_matrixf object when the previous matrix equal to NULL and width and height are not equal to zero
BOXING_START_TEST(BOXING_matrixf_recreate_test5)
{
    boxing_matrixf *temp_matrix1 = boxing_matrixf_create(0, 0);

    BOXING_ASSERT(temp_matrix1 == NULL);

    boxing_matrixf *temp_matrix2 = boxing_matrixf_recreate(temp_matrix1, 50, 40);

    check_matrixf(temp_matrix2, 50, 40, DTRUE);

    boxing_matrixf_free(temp_matrix2);
}
END_TEST


// Test to recreating a new boxing_matrixf object when the previous matrix not equal to NULL and width and height not equal to zero
BOXING_START_TEST(BOXING_matrixf_recreate_test6)
{
    boxing_matrixf *temp_matrix1 = boxing_matrixf_create(20, 10);

    check_matrixf_width_height(temp_matrix1, 20, 10);

    boxing_matrixf *temp_matrix2 = boxing_matrixf_recreate(temp_matrix1, 22, 33);

    check_matrixf(temp_matrix2, 22, 33, DTRUE);

    BOXING_ASSERT(temp_matrix1 == temp_matrix2);

    boxing_matrixf_free(temp_matrix2);
}
END_TEST


// Test to recreating a new boxing_matrixf object when the previous matrix not equal to NULL, data pointer of previous matrix equal to null and width and height not equal to zero
BOXING_START_TEST(BOXING_matrixf_recreate_test7)
{
    boxing_matrixf *temp_matrix1 = boxing_matrixf_create(20, 10);
    free(temp_matrix1->data);
    temp_matrix1->data = NULL;

    check_matrixf_width_height(temp_matrix1, 20, 10);

    BOXING_ASSERT(temp_matrix1->data == NULL);

    boxing_matrixf *temp_matrix2 = boxing_matrixf_recreate(temp_matrix1, 24, 35);

    check_matrixf(temp_matrix2, 24, 35, DTRUE);

    BOXING_ASSERT(temp_matrix1 == temp_matrix2);

    boxing_matrixf_free(temp_matrix2);
}
END_TEST


// Test to recreating a new boxing_matrixf object when the previous matrix not equal to NULL, data pointer of previous matrix not equal to null and width and height not equal to zero
// is_owning_data == DFALSE
BOXING_START_TEST(BOXING_matrixf_recreate_test8)
{
    boxing_matrixf *temp_matrix1 = boxing_matrixf_create(20, 10);
    boxing_pointf * data = temp_matrix1->data;
    temp_matrix1->is_owning_data = DFALSE;

    check_matrixf(temp_matrix1, 20, 10, DFALSE);

    boxing_matrixf *temp_matrix2 = boxing_matrixf_recreate(temp_matrix1, 24, 35);

    check_matrixf(temp_matrix2, 24, 35, DTRUE);

    BOXING_ASSERT(temp_matrix1 == temp_matrix2);

    boxing_matrixf_free(temp_matrix2);
    free(data);
}
END_TEST


// Test to recreating a new boxing_matrixi object with width and height equal to zero
BOXING_START_TEST(BOXING_matrixi_recreate_test1)
{
    boxing_matrixi *temp_matrix1 = boxing_matrixi_create(101, 102);

    BOXING_ASSERT(temp_matrix1 != NULL);

    boxing_matrixi *temp_matrix2 = boxing_matrixi_recreate(temp_matrix1, 0, 0);

    BOXING_ASSERT(temp_matrix2 == NULL);

    boxing_matrixi_free(temp_matrix1);
}
END_TEST


// Test to recreating a new boxing_matrixi object with width equal to zero
BOXING_START_TEST(BOXING_matrixi_recreate_test2)
{
    boxing_matrixi *temp_matrix1 = boxing_matrixi_create(101, 102);

    BOXING_ASSERT(temp_matrix1 != NULL);

    boxing_matrixi *temp_matrix2 = boxing_matrixi_recreate(temp_matrix1, 0, 50);

    BOXING_ASSERT(temp_matrix2 == NULL);

    boxing_matrixi_free(temp_matrix1);
}
END_TEST


// Test to recreating a new boxing_matrixi object with height equal to zero
BOXING_START_TEST(BOXING_matrixi_recreate_test3)
{
    boxing_matrixi *temp_matrix1 = boxing_matrixi_create(101, 102);

    BOXING_ASSERT(temp_matrix1 != NULL);

    boxing_matrixi *temp_matrix2 = boxing_matrixi_recreate(temp_matrix1, 50, 0);

    BOXING_ASSERT(temp_matrix2 == NULL);

    boxing_matrixi_free(temp_matrix1);
}
END_TEST


// Test to recreating a new boxing_matrixf object when the previous matrix equal to NULL and width and height are equal to zero
BOXING_START_TEST(BOXING_matrixi_recreate_test4)
{
    boxing_matrixi *temp_matrix1 = boxing_matrixi_create(0, 0);

    BOXING_ASSERT(temp_matrix1 == NULL);

    boxing_matrixi *temp_matrix2 = boxing_matrixi_recreate(temp_matrix1, 0, 0);

    BOXING_ASSERT(temp_matrix2 == NULL);
}
END_TEST


// Test to recreating a new boxing_matrixi object when the previous matrix equal to NULL and width and height are not equal to zero
BOXING_START_TEST(BOXING_matrixi_recreate_test5)
{
    boxing_matrixi *temp_matrix1 = boxing_matrixi_create(0, 0);

    BOXING_ASSERT(temp_matrix1 == NULL);

    boxing_matrixi *temp_matrix2 = boxing_matrixi_recreate(temp_matrix1, 50, 40);

    check_matrixi(temp_matrix2, 50, 40, DTRUE);

    boxing_matrixi_free(temp_matrix2);
}
END_TEST


// Test to recreating a new boxing_matrixf object when the previous matrix not equal to NULL and width and height not equal to zero
BOXING_START_TEST(BOXING_matrixi_recreate_test6)
{
    boxing_matrixi *temp_matrix1 = boxing_matrixi_create(20, 10);

    check_matrixi_width_height(temp_matrix1, 20, 10);

    boxing_matrixi *temp_matrix2 = boxing_matrixi_recreate(temp_matrix1, 22, 33);

    check_matrixi(temp_matrix2, 22, 33, DTRUE);

    BOXING_ASSERT(temp_matrix1 == temp_matrix2);

    boxing_matrixi_free(temp_matrix2);
}
END_TEST


// Test to recreating a new boxing_matrixi object when the previous matrix not equal to NULL, data pointer of previous matrix equal to null and width and height not equal to zero
BOXING_START_TEST(BOXING_matrixi_recreate_test7)
{
    boxing_matrixi *temp_matrix1 = boxing_matrixi_create(20, 10);
    free(temp_matrix1->data);
    temp_matrix1->data = NULL;

    check_matrixi_width_height(temp_matrix1, 20, 10);

    BOXING_ASSERT(temp_matrix1->data == NULL);

    boxing_matrixi *temp_matrix2 = boxing_matrixi_recreate(temp_matrix1, 24, 35);

    check_matrixi(temp_matrix2, 24, 35, DTRUE);

    BOXING_ASSERT(temp_matrix1 == temp_matrix2);

    boxing_matrixi_free(temp_matrix2);
}
END_TEST


// Test to recreating a new boxing_matrixi object when the previous matrix not equal to NULL, data pointer of previous matrix not equal to null and width and height not equal to zero
// is_owning_data == DFALSE
BOXING_START_TEST(BOXING_matrixi_recreate_test8)
{
    boxing_matrixi *temp_matrix1 = boxing_matrixi_create(20, 10);
    temp_matrix1->is_owning_data = DFALSE;
    boxing_pointi * data = temp_matrix1->data;

    check_matrixi(temp_matrix1, 20, 10, DFALSE);

    boxing_matrixi *temp_matrix2 = boxing_matrixi_recreate(temp_matrix1, 24, 35);

    check_matrixi(temp_matrix2, 24, 35, DTRUE);

    BOXING_ASSERT(temp_matrix1 == temp_matrix2);

    boxing_matrixi_free(temp_matrix2);
    free(data);
}
END_TEST


// Test function boxing_matrixf_init_in_place with matrix pointer equal to null
BOXING_START_TEST(BOXING_matrixf_init_in_place_test1)
{
    boxing_matrixf *temp_matrix = NULL;

    boxing_matrixf_init_in_place(temp_matrix, 0, 0);

    BOXING_ASSERT(temp_matrix == NULL);
}
END_TEST


// Test function boxing_matrixf_init_in_place with matrix pointer equal to null and width and height not equal to zero
BOXING_START_TEST(BOXING_matrixf_init_in_place_test2)
{
    boxing_matrixf *temp_matrix = NULL;

    boxing_matrixf_init_in_place(temp_matrix, 100, 110);

    BOXING_ASSERT(temp_matrix == NULL);
}
END_TEST


// Test function boxing_matrixf_init_in_place with matrix pointer not equal to null and width and height equal to zero
BOXING_START_TEST(BOXING_matrixf_init_in_place_test3)
{
    boxing_matrixf *temp_matrix = boxing_matrixf_create(20, 10);

    check_matrixf_width_height(temp_matrix, 20, 10);

    boxing_matrixf_init_in_place(temp_matrix, 0, 0);

    check_matrixf(temp_matrix, 20, 10, DTRUE);

    boxing_matrixf_free(temp_matrix);
}
END_TEST


// Test function boxing_matrixf_init_in_place with matrix pointer not equal to null and height equal to zero
BOXING_START_TEST(BOXING_matrixf_init_in_place_test4)
{
    boxing_matrixf *temp_matrix = boxing_matrixf_create(21, 12);

    check_matrixf_width_height(temp_matrix, 21, 12);

    boxing_matrixf_init_in_place(temp_matrix, 5, 0);

    check_matrixf(temp_matrix, 21, 12, DTRUE);

    boxing_matrixf_free(temp_matrix);
}
END_TEST


// Test function boxing_matrixf_init_in_place with matrix pointer not equal to null and width equal to zero
BOXING_START_TEST(BOXING_matrixf_init_in_place_test5)
{
    boxing_matrixf *temp_matrix = boxing_matrixf_create(4, 5);

    check_matrixf_width_height(temp_matrix, 4, 5);

    boxing_matrixf_init_in_place(temp_matrix, 0, 6);

    check_matrixf(temp_matrix, 4, 5, DTRUE);

    boxing_matrixf_free(temp_matrix);
}
END_TEST


// Test function boxing_matrixf_init_in_place with matrix pointer not equal to null and with and height not equal to zero
BOXING_START_TEST(BOXING_matrixf_init_in_place_test6)
{
    boxing_matrixf *temp_matrix = boxing_matrixf_create(11, 10);
    boxing_pointf * data = temp_matrix->data;

    check_matrixf_width_height(temp_matrix, 11, 10);

    boxing_matrixf_init_in_place(temp_matrix, 111, 110);

    check_matrixf(temp_matrix, 111, 110, DTRUE);

    boxing_matrixf_free(temp_matrix);
    free(data);
}
END_TEST


// Test function boxing_matrixf_init_in_place with matrix pointer not equal to null, data pointer of previous matrix equal to null and with and height not equal to zero
BOXING_START_TEST(BOXING_matrixf_init_in_place_test7)
{
    boxing_matrixf *temp_matrix = boxing_matrixf_create(11, 10);
    free(temp_matrix->data);
    temp_matrix->data = NULL;

    check_matrixf_width_height(temp_matrix, 11, 10);

    BOXING_ASSERT(temp_matrix->data == NULL);

    boxing_matrixf_init_in_place(temp_matrix, 111, 110);

    check_matrixf(temp_matrix, 111, 110, DTRUE);

    boxing_matrixf_free(temp_matrix);
}
END_TEST


// Test function boxing_matrixf_init_in_place with matrix pointer not equal to null, data pointer of previous matrix not equal to null and with and height not equal to zero
// is_owning_data == DFALSE
BOXING_START_TEST(BOXING_matrixf_init_in_place_test8)
{
    boxing_matrixf *temp_matrix = boxing_matrixf_create(11, 10);
    temp_matrix->is_owning_data = DFALSE;
    boxing_pointf * data = temp_matrix->data;

    check_matrixf(temp_matrix, 11, 10, DFALSE);

    boxing_matrixf_init_in_place(temp_matrix, 111, 110);

    check_matrixf(temp_matrix, 111, 110, DTRUE);

    boxing_matrixf_free(temp_matrix);
    free(data);
}
END_TEST


// Test function boxing_matrixi_init_in_place with matrix pointer equal to null
BOXING_START_TEST(BOXING_matrixi_init_in_place_test1)
{
    boxing_matrixi *temp_matrix = NULL;

    boxing_matrixi_init_in_place(temp_matrix, 0, 0);

    BOXING_ASSERT(temp_matrix == NULL);
}
END_TEST


// Test function boxing_matrixi_init_in_place with matrix pointer equal to null and width and height not equal to zero
BOXING_START_TEST(BOXING_matrixi_init_in_place_test2)
{
    boxing_matrixi *temp_matrix = NULL;

    boxing_matrixi_init_in_place(temp_matrix, 100, 110);

    BOXING_ASSERT(temp_matrix == NULL);
}
END_TEST


// Test function boxing_matrixi_init_in_place with matrix pointer not equal to null and width and height equal to zero
BOXING_START_TEST(BOXING_matrixi_init_in_place_test3)
{
    boxing_matrixi *temp_matrix = boxing_matrixi_create(20, 10);

    check_matrixi_width_height(temp_matrix, 20, 10);

    boxing_matrixi_init_in_place(temp_matrix, 0, 0);

    check_matrixi(temp_matrix, 20, 10, DTRUE);

    boxing_matrixi_free(temp_matrix);
}
END_TEST


// Test function boxing_matrixi_init_in_place with matrix pointer not equal to null and height equal to zero
BOXING_START_TEST(BOXING_matrixi_init_in_place_test4)
{
    boxing_matrixi *temp_matrix = boxing_matrixi_create(21, 12);

    check_matrixi_width_height(temp_matrix, 21, 12);

    boxing_matrixi_init_in_place(temp_matrix, 5, 0);

    check_matrixi(temp_matrix, 21, 12, DTRUE);

    boxing_matrixi_free(temp_matrix);
}
END_TEST


// Test function boxing_matrixi_init_in_place with matrix pointer not equal to null and width equal to zero
BOXING_START_TEST(BOXING_matrixi_init_in_place_test5)
{
    boxing_matrixi *temp_matrix = boxing_matrixi_create(4, 5);

    check_matrixi_width_height(temp_matrix, 4, 5);

    boxing_matrixi_init_in_place(temp_matrix, 0, 6);

    check_matrixi(temp_matrix, 4, 5, DTRUE);

    boxing_matrixi_free(temp_matrix);
}
END_TEST


// Test function boxing_matrixi_init_in_place with matrix pointer not equal to null and with and height not equal to zero
BOXING_START_TEST(BOXING_matrixi_init_in_place_test6)
{
    boxing_matrixi *temp_matrix = boxing_matrixi_create(11, 10);
    boxing_pointi * data = temp_matrix->data;

    check_matrixi_width_height(temp_matrix, 11, 10);

    boxing_matrixi_init_in_place(temp_matrix, 111, 110);

    check_matrixi(temp_matrix, 111, 110, DTRUE);

    boxing_matrixi_free(temp_matrix);
    free(data);
}
END_TEST


// Test function boxing_matrixi_init_in_place with matrix pointer not equal to null, data pointer of previous matrix equal to null and with and height not equal to zero
BOXING_START_TEST(BOXING_matrixi_init_in_place_test7)
{
    boxing_matrixi *temp_matrix = boxing_matrixi_create(11, 10);
    free(temp_matrix->data);
    temp_matrix->data = NULL;

    check_matrixi_width_height(temp_matrix, 11, 10);

    BOXING_ASSERT(temp_matrix->data == NULL);

    boxing_matrixi_init_in_place(temp_matrix, 111, 110);

    check_matrixi(temp_matrix, 111, 110, DTRUE);

    boxing_matrixi_free(temp_matrix);
}
END_TEST


// Test function boxing_matrixi_init_in_place with matrix pointer not equal to null, data pointer of previous matrix not equal to null and with and height not equal to zero
// is_owning_data == DFALSE
BOXING_START_TEST(BOXING_matrixi_init_in_place_test8)
{
    boxing_matrixi *temp_matrix = boxing_matrixi_create(11, 10);
    temp_matrix->is_owning_data = DFALSE;
    boxing_pointi * data = temp_matrix->data;

    check_matrixi(temp_matrix, 11, 10, DFALSE);

    boxing_matrixi_init_in_place(temp_matrix, 111, 110);

    check_matrixi(temp_matrix, 111, 110, DTRUE);

    boxing_matrixi_free(temp_matrix);
    free(data);
}
END_TEST


// Test function boxing_matrixf_copy source matrix pointer equal to null
BOXING_START_TEST(BOXING_matrixf_copy_test1)
{
    boxing_matrixf *temp_matrix1 = NULL;

    boxing_matrixf *temp_matrix2 = boxing_matrixf_copy(temp_matrix1);

    BOXING_ASSERT(temp_matrix2 == NULL);
}
END_TEST


// Test function boxing_matrixf_copy source matrix pointer not equal to null and data pointer equal to null
BOXING_START_TEST(BOXING_matrixf_copy_test2)
{
    boxing_matrixf *temp_matrix1 = boxing_matrixf_create(11, 10);
    free(temp_matrix1->data);
    temp_matrix1->data = NULL;

    BOXING_ASSERT(temp_matrix1 != NULL);
    BOXING_ASSERT(temp_matrix1->data == NULL);

    boxing_matrixf *temp_matrix2 = boxing_matrixf_copy(temp_matrix1);

    BOXING_ASSERT(temp_matrix2 != NULL);
    BOXING_ASSERT(temp_matrix2->data == NULL);
    BOXING_ASSERT(temp_matrix1 != temp_matrix2);

    boxing_matrixf_free(temp_matrix1);
    boxing_matrixf_free(temp_matrix2);
}
END_TEST


// Test function boxing_matrixf_copy source matrix pointer not equal to null and data pointer not equal to null
BOXING_START_TEST(BOXING_matrixf_copy_test3)
{
    boxing_matrixf *temp_matrix1 = boxing_matrixf_create(11, 10);

    fill_matrixf_random(temp_matrix1);

    BOXING_ASSERT(temp_matrix1 != NULL);
    BOXING_ASSERT(temp_matrix1->data != NULL);

    boxing_matrixf *temp_matrix2 = boxing_matrixf_copy(temp_matrix1);

    check_matrixf(temp_matrix2, 11, 10, DTRUE);

    BOXING_ASSERT(temp_matrix1->data != temp_matrix2->data);

    check_data_equal_matricesf(temp_matrix1, temp_matrix2);

    boxing_matrixf_free(temp_matrix1);
    boxing_matrixf_free(temp_matrix2);
}
END_TEST


// Test function boxing_matrixf_copy source matrix pointer not equal to null and data pointer not equal to null
// is_owning_data = DFALSE
BOXING_START_TEST(BOXING_matrixf_copy_test4)
{
    boxing_matrixf *temp_matrix1 = boxing_matrixf_create(11, 10);
    temp_matrix1->is_owning_data = DFALSE;

    fill_matrixf_random(temp_matrix1);

    BOXING_ASSERT(temp_matrix1 != NULL);
    BOXING_ASSERT(temp_matrix1->data != NULL);

    boxing_matrixf *temp_matrix2 = boxing_matrixf_copy(temp_matrix1);

    check_matrixf(temp_matrix2, 11, 10, DTRUE);

    BOXING_ASSERT(temp_matrix1 != temp_matrix2);
    BOXING_ASSERT(temp_matrix1->data != temp_matrix2->data);

    check_data_equal_matricesf(temp_matrix1, temp_matrix2);

    temp_matrix1->is_owning_data = DTRUE;
    boxing_matrixf_free(temp_matrix1);
    boxing_matrixf_free(temp_matrix2);
}
END_TEST


// Test function boxing_matrixi_copy source matrix pointer equal to null
BOXING_START_TEST(BOXING_matrixi_copy_test1)
{
    boxing_matrixi *temp_matrix1 = NULL;

    boxing_matrixi *temp_matrix2 = boxing_matrixi_copy(temp_matrix1);

    BOXING_ASSERT(temp_matrix2 == NULL);
}
END_TEST


// Test function boxing_matrixi_copy source matrix pointer not equal to null and data pointer equal to null
BOXING_START_TEST(BOXING_matrixi_copy_test2)
{
    boxing_matrixi *temp_matrix1 = boxing_matrixi_create(11, 10);
    free(temp_matrix1->data);
    temp_matrix1->data = NULL;

    BOXING_ASSERT(temp_matrix1 != NULL);
    BOXING_ASSERT(temp_matrix1->data == NULL);

    boxing_matrixi *temp_matrix2 = boxing_matrixi_copy(temp_matrix1);

    BOXING_ASSERT(temp_matrix2 != NULL);
    BOXING_ASSERT(temp_matrix2->data == NULL);
    BOXING_ASSERT(temp_matrix1 != temp_matrix2);

    boxing_matrixi_free(temp_matrix1);
    boxing_matrixi_free(temp_matrix2);
}
END_TEST


// Test function boxing_matrixi_copy source matrix pointer not equal to null and data pointer not equal to null
BOXING_START_TEST(BOXING_matrixi_copy_test3)
{
    boxing_matrixi *temp_matrix1 = boxing_matrixi_create(11, 10);

    fill_matrixi_random(temp_matrix1);

    BOXING_ASSERT(temp_matrix1 != NULL);
    BOXING_ASSERT(temp_matrix1->data != NULL);

    boxing_matrixi *temp_matrix2 = boxing_matrixi_copy(temp_matrix1);

    check_matrixi(temp_matrix2, 11, 10, DTRUE);

    BOXING_ASSERT(temp_matrix1->data != temp_matrix2->data);

    check_data_equal_matricesi(temp_matrix1, temp_matrix2);

    boxing_matrixi_free(temp_matrix1);
    boxing_matrixi_free(temp_matrix2);
}
END_TEST


// Test function boxing_matrixi_copy source matrix pointer not equal to null and data pointer not equal to null
// is_owning_data = DFALSE
BOXING_START_TEST(BOXING_matrixi_copy_test4)
{
    boxing_matrixi *temp_matrix1 = boxing_matrixi_create(11, 10);
    temp_matrix1->is_owning_data = DFALSE;

    fill_matrixi_random(temp_matrix1);

    BOXING_ASSERT(temp_matrix1 != NULL);
    BOXING_ASSERT(temp_matrix1->data != NULL);

    boxing_matrixi *temp_matrix2 = boxing_matrixi_copy(temp_matrix1);

    check_matrixi(temp_matrix2, 11, 10, DTRUE);

    BOXING_ASSERT(temp_matrix1 != temp_matrix2);
    BOXING_ASSERT(temp_matrix1->data != temp_matrix2->data);

    check_data_equal_matricesi(temp_matrix1, temp_matrix2);

    temp_matrix1->is_owning_data = DTRUE;
    boxing_matrixi_free(temp_matrix1);
    boxing_matrixi_free(temp_matrix2);
}
END_TEST


// Test function boxing_matrixf_free with null pointer
BOXING_START_TEST(BOXING_matrixf_free_test1)
{
    boxing_matrixf *temp_matrix = NULL;

    boxing_matrixf_free(temp_matrix);

    BOXING_ASSERT(temp_matrix == NULL);
}
END_TEST


// Test function boxing_matrixf_free with pointer not equal to null
BOXING_START_TEST(BOXING_matrixf_free_test2)
{
    boxing_matrixf *temp_matrix = boxing_matrixf_create(11, 10);

    BOXING_ASSERT(temp_matrix != NULL);
    BOXING_ASSERT(temp_matrix->data != NULL);

    boxing_matrixf_free(temp_matrix);

    //BOXING_ASSERT(temp_matrix == NULL); How to test freeing memory?
}
END_TEST


// Test function boxing_matrixf_free with pointer not equal to null and data pointer equal to null
BOXING_START_TEST(BOXING_matrixf_free_test3)
{
    boxing_matrixf *temp_matrix = boxing_matrixf_create(11, 10);
    free(temp_matrix->data);
    temp_matrix->data = NULL;

    BOXING_ASSERT(temp_matrix != NULL);

    boxing_matrixf_free(temp_matrix);

    //BOXING_ASSERT(temp_matrix == NULL); How to test freeing memory?
}
END_TEST


// Test function boxing_matrixf_free with pointer not equal to null and data pointer not equal to null
// is_owning_data = DFALSE
BOXING_START_TEST(BOXING_matrixf_free_test4)
{
    boxing_matrixf *temp_matrix = boxing_matrixf_create(11, 10);
    temp_matrix->is_owning_data = DFALSE;
    boxing_pointf * datapointer = temp_matrix->data;

    BOXING_ASSERT(temp_matrix != NULL);

    boxing_matrixf_free(temp_matrix);

    //BOXING_ASSERT(temp_matrix == NULL); How to test freeing memory?
    BOXING_ASSERT(datapointer != NULL);

    free(datapointer);
}
END_TEST


// Test function boxing_matrixi_free with null pointer
BOXING_START_TEST(BOXING_matrixi_free_test1)
{
    boxing_matrixi *temp_matrix = NULL;

    boxing_matrixi_free(temp_matrix);

    BOXING_ASSERT(temp_matrix == NULL);
}
END_TEST


// Test function boxing_matrixi_free with pointer not equal to null
BOXING_START_TEST(BOXING_matrixi_free_test2)
{
    boxing_matrixi *temp_matrix = boxing_matrixi_create(11, 10);

    BOXING_ASSERT(temp_matrix != NULL);
    BOXING_ASSERT(temp_matrix->data != NULL);

    boxing_matrixi_free(temp_matrix);

    //BOXING_ASSERT(temp_matrix == NULL); How to test freeing memory?
}
END_TEST


// Test function boxing_matrixi_free with pointer not equal to null and data pointer equal to null
BOXING_START_TEST(BOXING_matrixi_free_test3)
{
    boxing_matrixi *temp_matrix = boxing_matrixi_create(11, 10);
    free(temp_matrix->data);
    temp_matrix->data = NULL;

    BOXING_ASSERT(temp_matrix != NULL);

    boxing_matrixi_free(temp_matrix);

    //BOXING_ASSERT(temp_matrix == NULL); How to test freeing memory?
}
END_TEST


// Test function boxing_matrixi_free with pointer not equal to null and data pointer not equal to null
// is_owning_data = DFALSE
BOXING_START_TEST(BOXING_matrixi_free_test4)
{
    boxing_matrixi *temp_matrix = boxing_matrixi_create(11, 10);
    temp_matrix->is_owning_data = DFALSE;
    boxing_pointi * datapointer = temp_matrix->data;

    BOXING_ASSERT(temp_matrix != NULL);

    boxing_matrixi_free(temp_matrix);

    //BOXING_ASSERT(temp_matrix == NULL); How to test freeing memory?
    BOXING_ASSERT(datapointer != NULL);

    free(datapointer);
}
END_TEST


//  
//  MACROS Matrix Tests
// 

BOXING_START_TEST(MATRIX_ELEMENT_test1)
{
    boxing_matrixf *temp_matrix = boxing_matrixf_create(21, 30);

    fill_matrixf_random(temp_matrix);

    int i = 0;

    for (unsigned int x = 0; x < temp_matrix->height; x++)
    {
        for (unsigned int y = 0; y < temp_matrix->width; y++)
        {
            BOXING_ASSERT(temp_matrix->data[i].x == MATRIX_ELEMENT(temp_matrix, x, y).x);
            BOXING_ASSERT(temp_matrix->data[i].y == MATRIX_ELEMENT(temp_matrix, x, y).y);
            i++;
        }
    }

    boxing_matrixf_free(temp_matrix);
}
END_TEST


BOXING_START_TEST(MATRIX_ELEMENT_test2)
{
    boxing_matrixi *temp_matrix = boxing_matrixi_create(21, 30);

    fill_matrixi_random(temp_matrix);

    int i = 0;

    for (unsigned int x = 0; x < temp_matrix->height; x++)
    {
        for (unsigned int y = 0; y < temp_matrix->width; y++)
        {
            BOXING_ASSERT(temp_matrix->data[i].x == MATRIX_ELEMENT(temp_matrix, x, y).x);
            BOXING_ASSERT(temp_matrix->data[i].y == MATRIX_ELEMENT(temp_matrix, x, y).y);
            i++;
        }
    }

    boxing_matrixi_free(temp_matrix);
}
END_TEST


BOXING_START_TEST(MATRIX_PELEMENT_test1)
{
    boxing_matrixf *temp_matrix = boxing_matrixf_create(21, 30);

    fill_matrixf_random(temp_matrix);

    int i = 0;

    for (unsigned int x = 0; x < temp_matrix->height; x++)
    {
        for (unsigned int y = 0; y < temp_matrix->width; y++)
        {
            BOXING_ASSERT(temp_matrix->data[i].x == MATRIX_PELEMENT(temp_matrix, x, y)->x);
            BOXING_ASSERT(temp_matrix->data[i].y == MATRIX_PELEMENT(temp_matrix, x, y)->y);
            i++;
        }
    }

    boxing_matrixf_free(temp_matrix);
}
END_TEST


BOXING_START_TEST(MATRIX_PELEMENT_test2)
{
    boxing_matrixi *temp_matrix = boxing_matrixi_create(21, 30);

    fill_matrixi_random(temp_matrix);

    int i = 0;

    for (unsigned int x = 0; x < temp_matrix->height; x++)
    {
        for (unsigned int y = 0; y < temp_matrix->width; y++)
        {
            BOXING_ASSERT(temp_matrix->data[i].x == MATRIX_PELEMENT(temp_matrix, x, y)->x);
            BOXING_ASSERT(temp_matrix->data[i].y == MATRIX_PELEMENT(temp_matrix, x, y)->y);
            i++;
        }
    }

    boxing_matrixi_free(temp_matrix);
}
END_TEST


BOXING_START_TEST(MATRIX_ROW_test1)
{
    boxing_matrixf *temp_matrix = boxing_matrixf_create(21, 30);

    fill_matrixf_random(temp_matrix);

    int i = 0;

    for (unsigned int y = 0; y < temp_matrix->height; y++)
    {
        boxing_pointf *prow = MATRIX_ROW(temp_matrix, y);
        for (unsigned int x = 0; x < temp_matrix->width; x++)
        {
            BOXING_ASSERT(temp_matrix->data[i].x == prow[x].x);
            BOXING_ASSERT(temp_matrix->data[i].y == prow[x].y);
            i++;
        }
    }

    boxing_matrixf_free(temp_matrix);
}
END_TEST


BOXING_START_TEST(MATRIX_ROW_test2)
{
    boxing_matrixi *temp_matrix = boxing_matrixi_create(21, 30);

    fill_matrixi_random(temp_matrix);

    int i = 0;

    for (unsigned int y = 0; y < temp_matrix->height; y++)
    {
        boxing_pointi *prow = MATRIX_ROW(temp_matrix, y);
        for (unsigned int x = 0; x < temp_matrix->width; x++)
        {
            BOXING_ASSERT(temp_matrix->data[i].x == prow[x].x);
            BOXING_ASSERT(temp_matrix->data[i].y == prow[x].y);
            i++;
        }
    }

    boxing_matrixi_free(temp_matrix);
}
END_TEST


Suite * matrix_tests(void)
{
    TCase *tc_matrix_functions_tests;
    tc_matrix_functions_tests = tcase_create("functions_matrix_tests");

    // Test function boxing_matrixf_create
    tcase_add_test(tc_matrix_functions_tests, BOXING_matrixf_create_test1);
    tcase_add_test(tc_matrix_functions_tests, BOXING_matrixf_create_test2);
    tcase_add_test(tc_matrix_functions_tests, BOXING_matrixf_create_test3);
    tcase_add_test(tc_matrix_functions_tests, BOXING_matrixf_create_test4);
    tcase_add_test(tc_matrix_functions_tests, BOXING_matrixf_create_test5);

    // Test function boxing_matrixi_create
    tcase_add_test(tc_matrix_functions_tests, BOXING_matrixi_create_test1);
    tcase_add_test(tc_matrix_functions_tests, BOXING_matrixi_create_test2);
    tcase_add_test(tc_matrix_functions_tests, BOXING_matrixi_create_test3);
    tcase_add_test(tc_matrix_functions_tests, BOXING_matrixi_create_test4);
    tcase_add_test(tc_matrix_functions_tests, BOXING_matrixi_create_test5);

    // Test function boxing_matrixf_recreate
    tcase_add_test(tc_matrix_functions_tests, BOXING_matrixf_recreate_test1);
    tcase_add_test(tc_matrix_functions_tests, BOXING_matrixf_recreate_test2);
    tcase_add_test(tc_matrix_functions_tests, BOXING_matrixf_recreate_test3);
    tcase_add_test(tc_matrix_functions_tests, BOXING_matrixf_recreate_test4);
    tcase_add_test(tc_matrix_functions_tests, BOXING_matrixf_recreate_test5);
    tcase_add_test(tc_matrix_functions_tests, BOXING_matrixf_recreate_test6);
    tcase_add_test(tc_matrix_functions_tests, BOXING_matrixf_recreate_test7);
    tcase_add_test(tc_matrix_functions_tests, BOXING_matrixf_recreate_test8);

    // Test function boxing_matrixi_recreate
    tcase_add_test(tc_matrix_functions_tests, BOXING_matrixi_recreate_test1);
    tcase_add_test(tc_matrix_functions_tests, BOXING_matrixi_recreate_test2);
    tcase_add_test(tc_matrix_functions_tests, BOXING_matrixi_recreate_test3);
    tcase_add_test(tc_matrix_functions_tests, BOXING_matrixi_recreate_test4);
    tcase_add_test(tc_matrix_functions_tests, BOXING_matrixi_recreate_test5);
    tcase_add_test(tc_matrix_functions_tests, BOXING_matrixi_recreate_test6);
    tcase_add_test(tc_matrix_functions_tests, BOXING_matrixi_recreate_test7);
    tcase_add_test(tc_matrix_functions_tests, BOXING_matrixi_recreate_test8);

    // Test function boxing_matrixf_init_in_place
    tcase_add_test(tc_matrix_functions_tests, BOXING_matrixf_init_in_place_test1);
    tcase_add_test(tc_matrix_functions_tests, BOXING_matrixf_init_in_place_test2);
    tcase_add_test(tc_matrix_functions_tests, BOXING_matrixf_init_in_place_test3);
    tcase_add_test(tc_matrix_functions_tests, BOXING_matrixf_init_in_place_test4);
    tcase_add_test(tc_matrix_functions_tests, BOXING_matrixf_init_in_place_test5);
    tcase_add_test(tc_matrix_functions_tests, BOXING_matrixf_init_in_place_test6);
    tcase_add_test(tc_matrix_functions_tests, BOXING_matrixf_init_in_place_test7);
    tcase_add_test(tc_matrix_functions_tests, BOXING_matrixf_init_in_place_test8);

    // Test function boxing_matrixi_init_in_place
    tcase_add_test(tc_matrix_functions_tests, BOXING_matrixi_init_in_place_test1);
    tcase_add_test(tc_matrix_functions_tests, BOXING_matrixi_init_in_place_test2);
    tcase_add_test(tc_matrix_functions_tests, BOXING_matrixi_init_in_place_test3);
    tcase_add_test(tc_matrix_functions_tests, BOXING_matrixi_init_in_place_test4);
    tcase_add_test(tc_matrix_functions_tests, BOXING_matrixi_init_in_place_test5);
    tcase_add_test(tc_matrix_functions_tests, BOXING_matrixi_init_in_place_test6);
    tcase_add_test(tc_matrix_functions_tests, BOXING_matrixi_init_in_place_test7);
    tcase_add_test(tc_matrix_functions_tests, BOXING_matrixi_init_in_place_test8);

    // Test function boxing_matrixf_copy
    tcase_add_test(tc_matrix_functions_tests, BOXING_matrixf_copy_test1);
    tcase_add_test(tc_matrix_functions_tests, BOXING_matrixf_copy_test2);
    tcase_add_test(tc_matrix_functions_tests, BOXING_matrixf_copy_test3);
    tcase_add_test(tc_matrix_functions_tests, BOXING_matrixf_copy_test4);

    // Test function boxing_matrixi_copy
    tcase_add_test(tc_matrix_functions_tests, BOXING_matrixi_copy_test1);
    tcase_add_test(tc_matrix_functions_tests, BOXING_matrixi_copy_test2);
    tcase_add_test(tc_matrix_functions_tests, BOXING_matrixi_copy_test3);
    tcase_add_test(tc_matrix_functions_tests, BOXING_matrixi_copy_test4);

    // Test function boxing_matrixf_free
    tcase_add_test(tc_matrix_functions_tests, BOXING_matrixf_free_test1);
    tcase_add_test(tc_matrix_functions_tests, BOXING_matrixf_free_test2);
    tcase_add_test(tc_matrix_functions_tests, BOXING_matrixf_free_test3);
    tcase_add_test(tc_matrix_functions_tests, BOXING_matrixf_free_test4);

    // Test function boxing_matrixi_free
    tcase_add_test(tc_matrix_functions_tests, BOXING_matrixi_free_test1);
    tcase_add_test(tc_matrix_functions_tests, BOXING_matrixi_free_test2);
    tcase_add_test(tc_matrix_functions_tests, BOXING_matrixi_free_test3);
    tcase_add_test(tc_matrix_functions_tests, BOXING_matrixi_free_test4);

    TCase *tc_matrix_macros_tests;
    tc_matrix_macros_tests = tcase_create("macros_tests");

    // Test macros MATRIX_ELEMENT
    tcase_add_test(tc_matrix_macros_tests, MATRIX_ELEMENT_test1);
    tcase_add_test(tc_matrix_macros_tests, MATRIX_ELEMENT_test2);

    // Test macros MATRIX_PELEMENT
    tcase_add_test(tc_matrix_macros_tests, MATRIX_PELEMENT_test1);
    tcase_add_test(tc_matrix_macros_tests, MATRIX_PELEMENT_test2);

    // Test macros MATRIX_ROW
    tcase_add_test(tc_matrix_macros_tests, MATRIX_ROW_test1);
    tcase_add_test(tc_matrix_macros_tests, MATRIX_ROW_test2);

    Suite *s;
    s = suite_create("matrix_test_util");
    suite_add_tcase(s, tc_matrix_functions_tests);
    suite_add_tcase(s, tc_matrix_macros_tests);

    return s;
}
