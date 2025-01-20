#pragma GCC diagnostic ignored "-Wunused-variable"

#include "unittests.h"
#include "boxing/platform/memory.h"
#include "boxing/image8.h"
#include "boxing/utils.h"


static void check_crop_image(boxing_image8 *temp_image1, boxing_image8 *temp_image2, int shift_x, int shift_y)
{
    int size2 = temp_image2->width * temp_image2->height;

    for (int i = 0; i < size2; i++)
    {
        int x2 = i % temp_image2->width;
        int y2 = i / temp_image2->width;

        int shift1 = (y2 + shift_y) * temp_image1->width + x2 + shift_x;
        BOXING_ASSERT(temp_image2->data[i] == temp_image1->data[shift1]);
    }
}


static void fill_image_random(boxing_image8 *temp_image)
{
    int size1 = temp_image->width * temp_image->height;
    for (int i = 0; i < size1; i++)
    {
        temp_image->data[i] = (boxing_image8_pixel)rand() % 256;
    }
}


static void check_image(boxing_image8 *temp_image, unsigned int width, unsigned int height, DBOOL is_owning_data)
{
    BOXING_ASSERT(temp_image != NULL);
    BOXING_ASSERT(temp_image->width == width);
    BOXING_ASSERT(temp_image->height == height);
    BOXING_ASSERT(temp_image->is_owning_data == is_owning_data);
    BOXING_ASSERT(temp_image->data != NULL);
}


static void check_image_width_height(boxing_image8 *temp_image, unsigned int width, unsigned int height)
{
    BOXING_ASSERT(temp_image != NULL);
    BOXING_ASSERT(temp_image->width == width);
    BOXING_ASSERT(temp_image->height == height);
}


// Tests for file boxing/image8.h

//  
//  FUNCTIONS Image Tests
// 

// Test to creating a new boxing_image8 object with width and height equal to zero
BOXING_START_TEST(BOXING_image8_create_test1)
{
    boxing_image8 *temp_image = boxing_image8_create(0, 0);

    BOXING_ASSERT(temp_image == NULL);
}
END_TEST


// Test to creating a new boxing_image8 object with width equal to zero
BOXING_START_TEST(BOXING_image8_create_test2)
{
    boxing_image8 *temp_image = boxing_image8_create(0, 100);

    BOXING_ASSERT(temp_image == NULL);
}
END_TEST


// Test to creating a new boxing_image8 object with height equal to zero
BOXING_START_TEST(BOXING_image8_create_test3)
{
    boxing_image8 *temp_image = boxing_image8_create(100, 0);

    BOXING_ASSERT(temp_image == NULL);
}
END_TEST


// Test to creating a new boxing_image8 object with width and height not equal to zero
BOXING_START_TEST(BOXING_image8_create_test4)
{
    boxing_image8 *temp_image = boxing_image8_create(101, 102);

    check_image(temp_image, 101, 102, DTRUE);

    boxing_image8_free(temp_image);
}
END_TEST


// Test to creating a new boxing_image8 object with width and height equal to max unsigned int value
BOXING_START_TEST(BOXING_image8_create_test5)
{
    boxing_image8 *temp_image = boxing_image8_create(4294967295, 4294967295);

    check_image(temp_image, 4294967295, 4294967295, DTRUE);

    boxing_image8_free(temp_image);
}
END_TEST


// Test to recreating a new boxing_image8 object with width and height equal to zero
BOXING_START_TEST(BOXING_image8_recreate_test1)
{
    boxing_image8 *temp_image1 = boxing_image8_create(101, 102);

    BOXING_ASSERT(temp_image1 != NULL);

    boxing_image8 *temp_image2 = boxing_image8_recreate(temp_image1, 0, 0);

    BOXING_ASSERT(temp_image2 == NULL);
}
END_TEST


// Test to recreating a new boxing_image8 object with width equal to zero
BOXING_START_TEST(BOXING_image8_recreate_test2)
{
    boxing_image8 *temp_image1 = boxing_image8_create(101, 102);

    BOXING_ASSERT(temp_image1 != NULL);

    boxing_image8 *temp_image2 = boxing_image8_recreate(temp_image1, 0, 50);

    BOXING_ASSERT(temp_image2 == NULL);
}
END_TEST


// Test to recreating a new boxing_image8 object with height equal to zero
BOXING_START_TEST(BOXING_image8_recreate_test3)
{
    boxing_image8 *temp_image1 = boxing_image8_create(101, 102);

    BOXING_ASSERT(temp_image1 != NULL);

    boxing_image8 *temp_image2 = boxing_image8_recreate(temp_image1, 50, 0);

    BOXING_ASSERT(temp_image2 == NULL);
}
END_TEST


// Test to recreating a new boxing_image8 object when the previous image equal to NULL and width and height are equal to zero
BOXING_START_TEST(BOXING_image8_recreate_test4)
{
    boxing_image8 *temp_image1 = boxing_image8_create(0, 0);

    BOXING_ASSERT(temp_image1 == NULL);

    boxing_image8 *temp_image2 = boxing_image8_recreate(temp_image1, 0, 0);

    BOXING_ASSERT(temp_image2 == NULL);
}
END_TEST


// Test to recreating a new boxing_image8 object when the previous image equal to NULL and width and height are not equal to zero
BOXING_START_TEST(BOXING_image8_recreate_test5)
{
    boxing_image8 *temp_image1 = boxing_image8_create(0, 0);

    BOXING_ASSERT(temp_image1 == NULL);

    boxing_image8 *temp_image2 = boxing_image8_recreate(temp_image1, 50, 40);

    check_image(temp_image2, 50, 40, DTRUE);

    boxing_image8_free(temp_image2);
}
END_TEST


// Test to recreating a new boxing_image8 object when the previous image not equal to NULL and width and height not equal to zero
BOXING_START_TEST(BOXING_image8_recreate_test6)
{
    boxing_image8 *temp_image1 = boxing_image8_create(20, 10);

    check_image_width_height(temp_image1, 20, 10);

    boxing_image8 *temp_image2 = boxing_image8_recreate(temp_image1, 22, 33);

    check_image(temp_image2, 22, 33, DTRUE);

    BOXING_ASSERT(temp_image1 == temp_image2);

    boxing_image8_free(temp_image2);
}
END_TEST


// Test to recreating a new boxing_image8 object when the previous image not equal to NULL, data pointer of previous image equal to null and width and height not equal to zero
BOXING_START_TEST(BOXING_image8_recreate_test7)
{
    boxing_image8 *temp_image1 = boxing_image8_create(20, 10);
    boxing_image8_free_in_place(temp_image1);

    check_image_width_height(temp_image1, 20, 10);

    BOXING_ASSERT(temp_image1->data == NULL);

    boxing_image8 *temp_image2 = boxing_image8_recreate(temp_image1, 24, 35);

    check_image(temp_image2, 24, 35, DTRUE);

    BOXING_ASSERT(temp_image1 == temp_image2);

    boxing_image8_free(temp_image2);
}
END_TEST


// Test to recreating a new boxing_image8 object when the previous image not equal to NULL, data pointer of previous image not equal to null and width and height not equal to zero
// is_owning_data == DFALSE
BOXING_START_TEST(BOXING_image8_recreate_test8)
{
    boxing_image8 *temp_image1 = boxing_image8_create(20, 10);
    boxing_image8_pixel * temp_data = temp_image1->data;
    temp_image1->is_owning_data = DFALSE;

    check_image(temp_image1, 20, 10, DFALSE);

    boxing_image8 *temp_image2 = boxing_image8_recreate(temp_image1, 24, 35);

    check_image(temp_image2, 24, 35, DTRUE);

    BOXING_ASSERT(temp_image1 == temp_image2);

    boxing_image8_free(temp_image2);
    boxing_memory_free(temp_data);
}
END_TEST


// Test function boxing_image8_init_in_place with image pointer equal to null
BOXING_START_TEST(BOXING_image8_init_in_place_test1)
{
    boxing_image8 *temp_image = NULL;

    boxing_image8_init_in_place(temp_image, 0, 0);

    BOXING_ASSERT(temp_image == NULL);
}
END_TEST


// Test function boxing_image8_init_in_place with image pointer equal to null and width and height not equal to zero
BOXING_START_TEST(BOXING_image8_init_in_place_test2)
{
    boxing_image8 *temp_image = NULL;

    boxing_image8_init_in_place(temp_image, 100, 110);

    BOXING_ASSERT(temp_image == NULL);
}
END_TEST


// Test function boxing_image8_init_in_place with image pointer not equal to null and width and height equal to zero
BOXING_START_TEST(BOXING_image8_init_in_place_test3)
{
    boxing_image8 *temp_image = boxing_image8_create(20, 10);

    check_image_width_height(temp_image, 20, 10);

    boxing_image8_reinit_in_place(temp_image, 0, 0);

    check_image_width_height(temp_image, 0, 0);
    BOXING_ASSERT(temp_image->data == NULL);

    boxing_image8_free(temp_image);
}
END_TEST


// Test function boxing_image8_init_in_place with image pointer not equal to null and height equal to zero
BOXING_START_TEST(BOXING_image8_init_in_place_test4)
{
    boxing_image8 *temp_image = boxing_image8_create(21, 12);

    check_image_width_height(temp_image, 21, 12);

    boxing_image8_reinit_in_place(temp_image, 5, 0);

    check_image_width_height(temp_image, 5, 0);
    BOXING_ASSERT(temp_image->data == NULL);

    boxing_image8_free(temp_image);
}
END_TEST


// Test function boxing_image8_init_in_place with image pointer not equal to null and width equal to zero
BOXING_START_TEST(BOXING_image8_init_in_place_test5)
{
    boxing_image8 *temp_image = boxing_image8_create(4, 5);

    check_image_width_height(temp_image, 4, 5);

    boxing_image8_reinit_in_place(temp_image, 0, 6);

    check_image_width_height(temp_image, 0, 6);
    BOXING_ASSERT(temp_image->data == NULL);

    boxing_image8_free(temp_image);
}
END_TEST


// Test function boxing_image8_init_in_place with image pointer not equal to null and with and height not equal to zero
BOXING_START_TEST(BOXING_image8_init_in_place_test6)
{
    boxing_image8 *temp_image = boxing_image8_create(11, 10);
    boxing_image8_pixel * image_data = temp_image->data;

    check_image_width_height(temp_image, 11, 10);

    boxing_image8_reinit_in_place(temp_image, 111, 110);

    check_image(temp_image, 111, 110, DTRUE);

    boxing_image8_free(temp_image);
    boxing_memory_free(image_data);
}
END_TEST


// Test function boxing_image8_init_in_place with image pointer not equal to null, data pointer of previous image equal to null and with and height not equal to zero
BOXING_START_TEST(BOXING_image8_init_in_place_test7)
{
    boxing_image8 *temp_image = boxing_image8_create(11, 10);
    boxing_image8_free_in_place(temp_image);

    check_image_width_height(temp_image, 11, 10);

    BOXING_ASSERT(temp_image->data == NULL);

    boxing_image8_reinit_in_place(temp_image, 111, 110);

    check_image(temp_image, 111, 110, DTRUE);

    boxing_image8_free(temp_image);
}
END_TEST


// Test function boxing_image8_init_in_place with image pointer not equal to null, data pointer of previous image not equal to null and with and height not equal to zero
// is_owning_data == DFALSE
BOXING_START_TEST(BOXING_image8_init_in_place_test8)
{
    boxing_image8 *temp_image = boxing_image8_create(11, 10);
    boxing_image8_pixel * image_data = temp_image->data;
    temp_image->is_owning_data = DFALSE;

    check_image(temp_image, 11, 10, DFALSE);

    boxing_image8_reinit_in_place(temp_image, 111, 110);

    check_image(temp_image, 111, 110, DTRUE);

    boxing_image8_free(temp_image);
    boxing_memory_free(image_data);
}
END_TEST


// Test function boxing_image8_free with null pointer
BOXING_START_TEST(BOXING_image8_free_test1)
{
    boxing_image8 *temp_image = NULL;

    boxing_image8_free(temp_image);

    BOXING_ASSERT(temp_image == NULL);
}
END_TEST


// Test function boxing_image8_free with pointer not equal to null
BOXING_START_TEST(BOXING_image8_free_test2)
{
    boxing_image8 *temp_image = boxing_image8_create(11, 10);

    BOXING_ASSERT(temp_image != NULL);
    BOXING_ASSERT(temp_image->data != NULL);

    boxing_image8_free(temp_image);

    //BOXING_ASSERT(temp_image == NULL); How to test freeing memory?
}
END_TEST


// Test function boxing_image8_free with pointer not equal to null and data pointer equal to null
BOXING_START_TEST(BOXING_image8_free_test3)
{
    boxing_image8 *temp_image = boxing_image8_create(11, 10);
    boxing_image8_free_in_place(temp_image);

    BOXING_ASSERT(temp_image != NULL);

    boxing_image8_free(temp_image);

    //BOXING_ASSERT(temp_image == NULL); How to test freeing memory?
}
END_TEST


// Test function boxing_image8_free with pointer not equal to null and data pointer not equal to null
// is_owning_data = DFALSE
BOXING_START_TEST(BOXING_image8_free_test4)
{
    boxing_image8 *temp_image = boxing_image8_create(11, 10);
    temp_image->is_owning_data = DFALSE;
    boxing_image8_pixel * datapointer = temp_image->data;

    BOXING_ASSERT(temp_image != NULL);

    boxing_image8_free(temp_image);

    //BOXING_ASSERT(temp_image == NULL); How to test freeing memory?
    BOXING_ASSERT(datapointer != NULL);
    boxing_memory_free(datapointer);
}
END_TEST


// Test function boxing_image8_free_in_place with null pointer
BOXING_START_TEST(BOXING_image8_free_in_place_test1)
{
    boxing_image8 *temp_image = NULL;

    boxing_image8_free_in_place(temp_image);

    BOXING_ASSERT(temp_image == NULL);
}
END_TEST


// Test function boxing_image8_free_in_place with pointer not equal to null
BOXING_START_TEST(BOXING_image8_free_in_place_test2)
{
    boxing_image8 *temp_image = boxing_image8_create(11, 10);

    BOXING_ASSERT(temp_image != NULL);
    BOXING_ASSERT(temp_image->data != NULL);

    boxing_image8_free_in_place(temp_image);

    BOXING_ASSERT(temp_image != NULL);
    BOXING_ASSERT(temp_image->data == NULL);

    boxing_image8_free(temp_image);
}
END_TEST


// Test function boxing_image8_free_in_place with pointer not equal to null and data pointer equal to null
BOXING_START_TEST(BOXING_image8_free_in_place_test3)
{
    boxing_image8 *temp_image = boxing_image8_create(11, 10);
    boxing_image8_free_in_place(temp_image);

    BOXING_ASSERT(temp_image != NULL);
    BOXING_ASSERT(temp_image->data == NULL);

    boxing_image8_free_in_place(temp_image);

    BOXING_ASSERT(temp_image != NULL);
    BOXING_ASSERT(temp_image->data == NULL);

    boxing_image8_free(temp_image);
}
END_TEST


// Test function boxing_image8_free_in_place with pointer not equal to null and data pointer not equal to null
// is_owning_data = DFALSE
BOXING_START_TEST(BOXING_image8_free_in_place_test4)
{
    boxing_image8 *temp_image = boxing_image8_create(11, 10);
    temp_image->is_owning_data = DFALSE;

    BOXING_ASSERT(temp_image != NULL);
    BOXING_ASSERT(temp_image->data != NULL);

    boxing_image8_free_in_place(temp_image);

    BOXING_ASSERT(temp_image != NULL);
    BOXING_ASSERT(temp_image->data != NULL);

    temp_image->is_owning_data = DTRUE;
    boxing_image8_free(temp_image);
}
END_TEST


// Test function boxing_image8_copy source image pointer equal to null
BOXING_START_TEST(BOXING_image8_copy_test1)
{
    boxing_image8 *temp_image1 = NULL;

    boxing_image8 *temp_image2 = boxing_image8_copy(temp_image1);

    BOXING_ASSERT(temp_image2 == NULL);
}
END_TEST


// Test function boxing_image8_copy source image pointer not equal to null and data pointer equal to null
BOXING_START_TEST(BOXING_image8_copy_test2)
{
    boxing_image8 *temp_image1 = boxing_image8_create(11, 10);

    boxing_image8_free_in_place(temp_image1);

    BOXING_ASSERT(temp_image1 != NULL);
    BOXING_ASSERT(temp_image1->data == NULL);

    boxing_image8 *temp_image2 = boxing_image8_copy(temp_image1);

    BOXING_ASSERT(temp_image2 != NULL);
    BOXING_ASSERT(temp_image2->data == NULL);
    BOXING_ASSERT(temp_image1 != temp_image2);

    boxing_image8_free(temp_image1);
    boxing_image8_free(temp_image2);
}
END_TEST


// Test function boxing_image8_copy source image pointer not equal to null and data pointer not equal to null
BOXING_START_TEST(BOXING_image8_copy_test3)
{
    boxing_image8 *temp_image1 = boxing_image8_create(11, 10);

    fill_image_random(temp_image1);

    BOXING_ASSERT(temp_image1 != NULL);
    BOXING_ASSERT(temp_image1->data != NULL);

    boxing_image8 *temp_image2 = boxing_image8_copy(temp_image1);

    check_image(temp_image2, 11, 10, DTRUE);

    BOXING_ASSERT(temp_image1->data != temp_image2->data);

    check_crop_image(temp_image1, temp_image2, 0, 0);

    boxing_image8_free(temp_image1);
    boxing_image8_free(temp_image2);
}
END_TEST


// Test function boxing_image8_copy source image pointer not equal to null and data pointer not equal to null
// is_owning_data = DFALSE
BOXING_START_TEST(BOXING_image8_copy_test4)
{
    boxing_image8 *temp_image1 = boxing_image8_create(11, 10);
    temp_image1->is_owning_data = DFALSE;

    fill_image_random(temp_image1);

    BOXING_ASSERT(temp_image1 != NULL);
    BOXING_ASSERT(temp_image1->data != NULL);

    boxing_image8 *temp_image2 = boxing_image8_copy(temp_image1);

    check_image(temp_image2, 11, 10, DTRUE);

    BOXING_ASSERT(temp_image1 != temp_image2);
    BOXING_ASSERT(temp_image1->data != temp_image2->data);

    check_crop_image(temp_image1, temp_image2, 0, 0);

    temp_image1->is_owning_data = DTRUE;
    boxing_image8_free(temp_image1);
    boxing_image8_free(temp_image2);
}
END_TEST


// Test function boxing_image8_copy_use_buffer source image pointer equal to null
BOXING_START_TEST(BOXING_image8_copy_use_buffer_test1)
{
    boxing_image8 *temp_image1 = NULL;

    boxing_image8 *temp_image2 = boxing_image8_copy_use_buffer(temp_image1);

    BOXING_ASSERT(temp_image2 == NULL);
}
END_TEST


// Test function boxing_image8_copy_use_buffer source image pointer not equal to null and data pointer equal to null
BOXING_START_TEST(BOXING_image8_copy_use_buffer_test2)
{
    boxing_image8 *temp_image1 = boxing_image8_create(11, 10);

    boxing_image8_free_in_place(temp_image1);

    BOXING_ASSERT(temp_image1 != NULL);
    BOXING_ASSERT(temp_image1->data == NULL);

    boxing_image8 *temp_image2 = boxing_image8_copy_use_buffer(temp_image1);

    BOXING_ASSERT(temp_image2 != NULL);
    BOXING_ASSERT(temp_image2->data == NULL);
    BOXING_ASSERT(temp_image1 != temp_image2);

    boxing_image8_free(temp_image1);
    boxing_image8_free(temp_image2);
}
END_TEST


// Test function boxing_image8_copy_use_buffer source image pointer not equal to null and data pointer not equal to null
BOXING_START_TEST(BOXING_image8_copy_use_buffer_test3)
{
    boxing_image8 *temp_image1 = boxing_image8_create(11, 10);

    fill_image_random(temp_image1);

    BOXING_ASSERT(temp_image1 != NULL);
    BOXING_ASSERT(temp_image1->data != NULL);

    boxing_image8 *temp_image2 = boxing_image8_copy_use_buffer(temp_image1);

    check_image(temp_image2, 11, 10, DFALSE);

    BOXING_ASSERT(temp_image1 != temp_image2);
    BOXING_ASSERT(temp_image1->data == temp_image2->data);

    check_crop_image(temp_image1, temp_image2, 0, 0);

    boxing_image8_free(temp_image1);
    boxing_image8_free(temp_image2);
}
END_TEST


// Test function boxing_image8_copy_use_buffer source image pointer not equal to null and data pointer not equal to null
// is_owning_data = DFALSE
BOXING_START_TEST(BOXING_image8_copy_use_buffer_test4)
{
    boxing_image8 *temp_image1 = boxing_image8_create(11, 10);
    temp_image1->is_owning_data = DFALSE;

    fill_image_random(temp_image1);

    BOXING_ASSERT(temp_image1 != NULL);
    BOXING_ASSERT(temp_image1->data != NULL);

    boxing_image8 *temp_image2 = boxing_image8_copy_use_buffer(temp_image1);

    check_image(temp_image2, 11, 10, DFALSE);

    BOXING_ASSERT(temp_image1 != temp_image2);
    BOXING_ASSERT(temp_image1->data == temp_image2->data);

    check_crop_image(temp_image1, temp_image2, 0, 0);

    temp_image1->is_owning_data = DTRUE;
    boxing_image8_free(temp_image1);
    boxing_image8_free(temp_image2);
}
END_TEST


// Test function boxing_image8_is_null with image pointer is NULL
BOXING_START_TEST(BOXING_image8_is_null_test1)
{
    boxing_image8 *temp_image1 = NULL;

    DBOOL state = boxing_image8_is_null(temp_image1);

    BOXING_ASSERT(state == DTRUE);
}
END_TEST


// Test function boxing_image8_is_null with image pointer is not NULL, data pointer is NULL
BOXING_START_TEST(BOXING_image8_is_null_test2)
{
    boxing_image8 *temp_image1 = boxing_image8_create(11, 10);
    boxing_image8_free_in_place(temp_image1);

    DBOOL state = boxing_image8_is_null(temp_image1);

    BOXING_ASSERT(state == DTRUE);

    boxing_image8_free(temp_image1);
}
END_TEST


// Test function boxing_image8_is_null with image pointer is not NULL, data pointer is not NULL
BOXING_START_TEST(BOXING_image8_is_null_test3)
{
    boxing_image8 *temp_image1 = boxing_image8_create(9, 10);

    DBOOL state = boxing_image8_is_null(temp_image1);

    BOXING_ASSERT(state == DFALSE);

    boxing_image8_free(temp_image1);
}
END_TEST


// Test function boxing_image8_crop with image pointer is NULL
BOXING_START_TEST(BOXING_image8_crop_test1)
{
    boxing_image8 *temp_image1 = NULL;

    boxing_image8 *temp_image2 = boxing_image8_crop(temp_image1, 0, 0, 1, 1);

    BOXING_ASSERT(temp_image2 == NULL);
}
END_TEST


// Test function boxing_image8_crop with all crop data equal to zero
BOXING_START_TEST(BOXING_image8_crop_test2)
{
    boxing_image8 *temp_image1 = boxing_image8_create(10, 25);

    boxing_image8 *temp_image2 = boxing_image8_crop(temp_image1, 0, 0, 0, 0);

    BOXING_ASSERT(temp_image2 == NULL);

    boxing_image8_free(temp_image1);
}
END_TEST


// Test function boxing_image8_crop with all crop data is negative
BOXING_START_TEST(BOXING_image8_crop_test3)
{
    boxing_image8 *temp_image1 = boxing_image8_create(10, 25);

    boxing_image8 *temp_image2 = boxing_image8_crop(temp_image1, -10, -20, -30, -40);

    BOXING_ASSERT(temp_image2 == NULL);

    boxing_image8_free(temp_image1);
}
END_TEST


// Test function boxing_image8_crop with offset crop data is positive and size crop data is negative
BOXING_START_TEST(BOXING_image8_crop_test4)
{
    boxing_image8 *temp_image1 = boxing_image8_create(10, 25);

    boxing_image8 *temp_image2 = boxing_image8_crop(temp_image1, 5, 6, -30, -40);

    BOXING_ASSERT(temp_image2 == NULL);

    boxing_image8_free(temp_image1);
}
END_TEST


// Test function boxing_image8_crop with data out of range in the lower left corner
BOXING_START_TEST(BOXING_image8_crop_test5)
{
    boxing_image8 *temp_image1 = boxing_image8_create(10, 25);

    boxing_image8 *temp_image2 = boxing_image8_crop(temp_image1, -10, -10, 5, 5);

    BOXING_ASSERT(temp_image2 == NULL);

    boxing_image8_free(temp_image1);
}
END_TEST


// Test function boxing_image8_crop with data out of range in the upper left corner
BOXING_START_TEST(BOXING_image8_crop_test6)
{
    boxing_image8 *temp_image1 = boxing_image8_create(10, 25);

    boxing_image8 *temp_image2 = boxing_image8_crop(temp_image1, -10, 30, 5, 5);

    BOXING_ASSERT(temp_image2 == NULL);

    boxing_image8_free(temp_image1);
}
END_TEST


// Test function boxing_image8_crop with data out of range in the upper right corner
BOXING_START_TEST(BOXING_image8_crop_test7)
{
    boxing_image8 *temp_image1 = boxing_image8_create(10, 25);

    boxing_image8 *temp_image2 = boxing_image8_crop(temp_image1, 15, 30, 5, 5);

    BOXING_ASSERT(temp_image2 == NULL);

    boxing_image8_free(temp_image1);
}
END_TEST


// Test function boxing_image8_crop with data out of range in the lower right corner
BOXING_START_TEST(BOXING_image8_crop_test8)
{
    boxing_image8 *temp_image1 = boxing_image8_create(10, 25);

    boxing_image8 *temp_image2 = boxing_image8_crop(temp_image1, 15, -10, 5, 5);

    BOXING_ASSERT(temp_image2 == NULL);

    boxing_image8_free(temp_image1);
}
END_TEST


// Test function boxing_image8_crop with field intersect in the lower left corner
BOXING_START_TEST(BOXING_image8_crop_test9)
{
    boxing_image8 *temp_image1 = boxing_image8_create(10, 25);

    fill_image_random(temp_image1);

    boxing_image8 *temp_image2 = boxing_image8_crop(temp_image1, -10, -10, 15, 15);

    check_image_width_height(temp_image2, 5, 5);

    check_crop_image(temp_image1, temp_image2, 0, 0);

    boxing_image8_free(temp_image1);
    boxing_image8_free(temp_image2);
}
END_TEST


// Test function boxing_image8_crop with field intersect in the upper left corner
BOXING_START_TEST(BOXING_image8_crop_test10)
{
    boxing_image8 *temp_image1 = boxing_image8_create(10, 25);

    fill_image_random(temp_image1);

    boxing_image8 *temp_image2 = boxing_image8_crop(temp_image1, -10, 20, 15, 15);

    check_image_width_height(temp_image2, 5, 5);

    check_crop_image(temp_image1, temp_image2, 0, 20);

    boxing_image8_free(temp_image1);
    boxing_image8_free(temp_image2);
}
END_TEST


// Test function boxing_image8_crop with field intersect in the upper right corner
BOXING_START_TEST(BOXING_image8_crop_test11)
{
    boxing_image8 *temp_image1 = boxing_image8_create(10, 25);

    fill_image_random(temp_image1);

    boxing_image8 *temp_image2 = boxing_image8_crop(temp_image1, 5, 20, 15, 15);

    check_image_width_height(temp_image2, 5, 5);

    check_crop_image(temp_image1, temp_image2, 5, 20);

    boxing_image8_free(temp_image1);
    boxing_image8_free(temp_image2);
}
END_TEST


// Test function boxing_image8_crop with field intersect in the lower right corner
BOXING_START_TEST(BOXING_image8_crop_test12)
{
    boxing_image8 *temp_image1 = boxing_image8_create(10, 25);

    fill_image_random(temp_image1);

    boxing_image8 *temp_image2 = boxing_image8_crop(temp_image1, 5, -10, 15, 15);

    check_image_width_height(temp_image2, 5, 5);

    check_crop_image(temp_image1, temp_image2, 5, 0);

    boxing_image8_free(temp_image1);
    boxing_image8_free(temp_image2);
}
END_TEST


// Test function boxing_image8_crop with field intersect inside the lower edge
BOXING_START_TEST(BOXING_image8_crop_test13)
{
    boxing_image8 *temp_image1 = boxing_image8_create(15, 15);

    fill_image_random(temp_image1);

    boxing_image8 *temp_image2 = boxing_image8_crop(temp_image1, 5, -10, 5, 15);

    check_image_width_height(temp_image2, 5, 5);

    check_crop_image(temp_image1, temp_image2, 5, 0);

    boxing_image8_free(temp_image1);
    boxing_image8_free(temp_image2);
}
END_TEST


// Test function boxing_image8_crop with field intersect inside the left edge
BOXING_START_TEST(BOXING_image8_crop_test14)
{
    boxing_image8 *temp_image1 = boxing_image8_create(15, 15);

    fill_image_random(temp_image1);

    boxing_image8 *temp_image2 = boxing_image8_crop(temp_image1, -10, 5, 15, 5);

    check_image_width_height(temp_image2, 5, 5);

    check_crop_image(temp_image1, temp_image2, 0, 5);

    boxing_image8_free(temp_image1);
    boxing_image8_free(temp_image2);
}
END_TEST


// Test function boxing_image8_crop with field intersect inside the upper edge
BOXING_START_TEST(BOXING_image8_crop_test15)
{
    boxing_image8 *temp_image1 = boxing_image8_create(15, 15);

    fill_image_random(temp_image1);

    boxing_image8 *temp_image2 = boxing_image8_crop(temp_image1, 5, 10, 5, 10);

    check_image_width_height(temp_image2, 5, 5);

    check_crop_image(temp_image1, temp_image2, 5, 10);

    boxing_image8_free(temp_image1);
    boxing_image8_free(temp_image2);
}
END_TEST


// Test function boxing_image8_crop with field intersect inside the right edge
BOXING_START_TEST(BOXING_image8_crop_test16)
{
    boxing_image8 *temp_image1 = boxing_image8_create(15, 15);

    fill_image_random(temp_image1);

    boxing_image8 *temp_image2 = boxing_image8_crop(temp_image1, 10, 5, 10, 5);

    check_image_width_height(temp_image2, 5, 5);

    check_crop_image(temp_image1, temp_image2, 10, 5);

    boxing_image8_free(temp_image1);
    boxing_image8_free(temp_image2);
}
END_TEST


// Test function boxing_image8_crop with area completely within
BOXING_START_TEST(BOXING_image8_crop_test17)
{
    boxing_image8 *temp_image1 = boxing_image8_create(15, 15);

    fill_image_random(temp_image1);

    boxing_image8 *temp_image2 = boxing_image8_crop(temp_image1, 5, 5, 5, 5);

    check_image_width_height(temp_image2, 5, 5);

    check_crop_image(temp_image1, temp_image2, 5, 5);

    boxing_image8_free(temp_image1);
    boxing_image8_free(temp_image2);
}
END_TEST


// Test function boxing_image8_crop with completely overlapping areas
BOXING_START_TEST(BOXING_image8_crop_test18)
{
    boxing_image8 *temp_image1 = boxing_image8_create(15, 15);

    fill_image_random(temp_image1);

    boxing_image8 *temp_image2 = boxing_image8_crop(temp_image1, 0, 0, 15, 15);

    check_image_width_height(temp_image2, 15, 15);

    check_crop_image(temp_image1, temp_image2, 0, 0);

    boxing_image8_free(temp_image1);
    boxing_image8_free(temp_image2);
}
END_TEST


// Test function boxing_image8_crop with overlapping area of the lower and left edge
BOXING_START_TEST(BOXING_image8_crop_test19)
{
    boxing_image8 *temp_image1 = boxing_image8_create(15, 15);

    fill_image_random(temp_image1);

    boxing_image8 *temp_image2 = boxing_image8_crop(temp_image1, 0, 0, 10, 10);

    check_image_width_height(temp_image2, 10, 10);

    check_crop_image(temp_image1, temp_image2, 0, 0);

    boxing_image8_free(temp_image1);
    boxing_image8_free(temp_image2);
}
END_TEST


// Test function boxing_image8_crop with overlapping area of the upper and left edge
BOXING_START_TEST(BOXING_image8_crop_test20)
{
    boxing_image8 *temp_image1 = boxing_image8_create(15, 15);

    fill_image_random(temp_image1);

    boxing_image8 *temp_image2 = boxing_image8_crop(temp_image1, 0, 5, 10, 10);

    check_image_width_height(temp_image2, 10, 10);

    check_crop_image(temp_image1, temp_image2, 0, 5);

    boxing_image8_free(temp_image1);
    boxing_image8_free(temp_image2);
}
END_TEST


// Test function boxing_image8_crop with overlapping area of the upper and right edge
BOXING_START_TEST(BOXING_image8_crop_test21)
{
    boxing_image8 *temp_image1 = boxing_image8_create(15, 15);

    fill_image_random(temp_image1);

    boxing_image8 *temp_image2 = boxing_image8_crop(temp_image1, 5, 5, 10, 10);

    check_image_width_height(temp_image2, 10, 10);

    check_crop_image(temp_image1, temp_image2, 5, 5);

    boxing_image8_free(temp_image1);
    boxing_image8_free(temp_image2);
}
END_TEST


// Test function boxing_image8_crop with overlapping area of the lower and right edge
BOXING_START_TEST(BOXING_image8_crop_test22)
{
    boxing_image8 *temp_image1 = boxing_image8_create(15, 15);

    fill_image_random(temp_image1);

    boxing_image8 *temp_image2 = boxing_image8_crop(temp_image1, 5, 0, 10, 10);

    check_image_width_height(temp_image2, 10, 10);

    check_crop_image(temp_image1, temp_image2, 5, 0);

    boxing_image8_free(temp_image1);
    boxing_image8_free(temp_image2);
}
END_TEST


// Test function boxing_image8_crop with totally more area
BOXING_START_TEST(BOXING_image8_crop_test23)
{
    boxing_image8 *temp_image1 = boxing_image8_create(15, 15);

    fill_image_random(temp_image1);

    boxing_image8 *temp_image2 = boxing_image8_crop(temp_image1, -5, -5, 25, 25);

    check_image_width_height(temp_image2, 15, 15);

    check_crop_image(temp_image1, temp_image2, 0, 0);

    boxing_image8_free(temp_image1);
    boxing_image8_free(temp_image2);
}
END_TEST


// Test function boxing_image8_crop with overlay the lower half of area
BOXING_START_TEST(BOXING_image8_crop_test24)
{
    boxing_image8 *temp_image1 = boxing_image8_create(10, 10);

    fill_image_random(temp_image1);

    boxing_image8 *temp_image2 = boxing_image8_crop(temp_image1, -5, -5, 20, 10);

    check_image_width_height(temp_image2, 10, 5);

    check_crop_image(temp_image1, temp_image2, 0, 0);

    boxing_image8_free(temp_image1);
    boxing_image8_free(temp_image2);
}
END_TEST


// Test function boxing_image8_crop with overlay the left half of area
BOXING_START_TEST(BOXING_image8_crop_test25)
{
    boxing_image8 *temp_image1 = boxing_image8_create(10, 10);

    fill_image_random(temp_image1);

    boxing_image8 *temp_image2 = boxing_image8_crop(temp_image1, -5, -5, 10, 20);

    check_image_width_height(temp_image2, 5, 10);

    check_crop_image(temp_image1, temp_image2, 0, 0);

    boxing_image8_free(temp_image1);
    boxing_image8_free(temp_image2);
}
END_TEST


// Test function boxing_image8_crop with overlay the upper half of area
BOXING_START_TEST(BOXING_image8_crop_test26)
{
    boxing_image8 *temp_image1 = boxing_image8_create(10, 10);

    fill_image_random(temp_image1);

    boxing_image8 *temp_image2 = boxing_image8_crop(temp_image1, -5, 5, 20, 10);

    check_image_width_height(temp_image2, 10, 5);

    check_crop_image(temp_image1, temp_image2, 0, 5);

    boxing_image8_free(temp_image1);
    boxing_image8_free(temp_image2);
}
END_TEST


// Test function boxing_image8_crop with overlay the right half of area
BOXING_START_TEST(BOXING_image8_crop_test27)
{
    boxing_image8 *temp_image1 = boxing_image8_create(10, 10);

    fill_image_random(temp_image1);

    boxing_image8 *temp_image2 = boxing_image8_crop(temp_image1, 5, -5, 10, 20);

    check_image_width_height(temp_image2, 5, 10);

    check_crop_image(temp_image1, temp_image2, 5, 0);

    boxing_image8_free(temp_image1);
    boxing_image8_free(temp_image2);
}
END_TEST


//  
//  MACROS Image Tests
// 

BOXING_START_TEST(IMAGE8_PIXEL_test1)
{
    boxing_image8 *temp_image1 = boxing_image8_create(11, 10);

    fill_image_random(temp_image1);

    int i = 0;

    for (boxing_image8_pixel y = 0; y < temp_image1->height; y++)
    {
        for (boxing_image8_pixel x = 0; x < temp_image1->width; x++)
        {
            BOXING_ASSERT(temp_image1->data[i] == (boxing_image8_pixel)IMAGE8_PIXEL(temp_image1, x, y));
            i++;
        }
    }

    boxing_image8_free(temp_image1);
}
END_TEST


BOXING_START_TEST(IMAGE8_PPIXEL_test1)
{
    boxing_image8 *temp_image1 = boxing_image8_create(11, 10);

    fill_image_random(temp_image1);

    int i = 0;

    for (boxing_image8_pixel y = 0; y < temp_image1->height; y++)
    {
        for (boxing_image8_pixel x = 0; x < temp_image1->width; x++)
        {
            BOXING_ASSERT(temp_image1->data[i] == (boxing_image8_pixel)*IMAGE8_PPIXEL(temp_image1, x, y));
            i++;
        }
    }

    boxing_image8_free(temp_image1);
}
END_TEST


BOXING_START_TEST(IMAGE8_SCANLINE_test1)
{
    boxing_image8 *temp_image1 = boxing_image8_create(11, 10);

    fill_image_random(temp_image1);

    int i = 0;

    for (boxing_image8_pixel y = 0; y < temp_image1->height; y++)
    {
        boxing_image8_pixel *pline = IMAGE8_SCANLINE(temp_image1, y);
        for (boxing_image8_pixel x = 0; x < temp_image1->width; x++)
        {
            BOXING_ASSERT(temp_image1->data[i] == pline[x]);
            i++;
        }
    }

    boxing_image8_free(temp_image1);
}
END_TEST


Suite * image8_tests(void)
{
    TCase *tc_image8_functions_image_tests;
    tc_image8_functions_image_tests = tcase_create("functions_image_tests");

    // Test function boxing_image8_create
    tcase_add_test(tc_image8_functions_image_tests, BOXING_image8_create_test1);
    tcase_add_test(tc_image8_functions_image_tests, BOXING_image8_create_test2);
    tcase_add_test(tc_image8_functions_image_tests, BOXING_image8_create_test3);
    tcase_add_test(tc_image8_functions_image_tests, BOXING_image8_create_test4);
    tcase_add_test(tc_image8_functions_image_tests, BOXING_image8_create_test5);

    // Test function boxing_image8_recreate
    tcase_add_test(tc_image8_functions_image_tests, BOXING_image8_recreate_test1);
    tcase_add_test(tc_image8_functions_image_tests, BOXING_image8_recreate_test2);
    tcase_add_test(tc_image8_functions_image_tests, BOXING_image8_recreate_test3);
    tcase_add_test(tc_image8_functions_image_tests, BOXING_image8_recreate_test4);
    tcase_add_test(tc_image8_functions_image_tests, BOXING_image8_recreate_test5);
    tcase_add_test(tc_image8_functions_image_tests, BOXING_image8_recreate_test6);
    tcase_add_test(tc_image8_functions_image_tests, BOXING_image8_recreate_test7);
    tcase_add_test(tc_image8_functions_image_tests, BOXING_image8_recreate_test8);

    // Test function boxing_image8_init_in_place
    tcase_add_test(tc_image8_functions_image_tests, BOXING_image8_init_in_place_test1);
    tcase_add_test(tc_image8_functions_image_tests, BOXING_image8_init_in_place_test2);
    tcase_add_test(tc_image8_functions_image_tests, BOXING_image8_init_in_place_test3);
    tcase_add_test(tc_image8_functions_image_tests, BOXING_image8_init_in_place_test4);
    tcase_add_test(tc_image8_functions_image_tests, BOXING_image8_init_in_place_test5);
    tcase_add_test(tc_image8_functions_image_tests, BOXING_image8_init_in_place_test6);
    tcase_add_test(tc_image8_functions_image_tests, BOXING_image8_init_in_place_test7);
    tcase_add_test(tc_image8_functions_image_tests, BOXING_image8_init_in_place_test8);

    // Test function boxing_image8_free
    tcase_add_test(tc_image8_functions_image_tests, BOXING_image8_free_test1);
    tcase_add_test(tc_image8_functions_image_tests, BOXING_image8_free_test2);
    tcase_add_test(tc_image8_functions_image_tests, BOXING_image8_free_test3);
    tcase_add_test(tc_image8_functions_image_tests, BOXING_image8_free_test4);

    // Test function boxing_image8_free_in_place
    tcase_add_test(tc_image8_functions_image_tests, BOXING_image8_free_in_place_test1);
    tcase_add_test(tc_image8_functions_image_tests, BOXING_image8_free_in_place_test2);
    tcase_add_test(tc_image8_functions_image_tests, BOXING_image8_free_in_place_test3);
    tcase_add_test(tc_image8_functions_image_tests, BOXING_image8_free_in_place_test4);

    // Test function boxing_image8_copy
    tcase_add_test(tc_image8_functions_image_tests, BOXING_image8_copy_test1);
    tcase_add_test(tc_image8_functions_image_tests, BOXING_image8_copy_test2);
    tcase_add_test(tc_image8_functions_image_tests, BOXING_image8_copy_test3);
    tcase_add_test(tc_image8_functions_image_tests, BOXING_image8_copy_test4);

    // Test function boxing_image8_copy_use_buffer
    tcase_add_test(tc_image8_functions_image_tests, BOXING_image8_copy_use_buffer_test1);
    tcase_add_test(tc_image8_functions_image_tests, BOXING_image8_copy_use_buffer_test2);
    tcase_add_test(tc_image8_functions_image_tests, BOXING_image8_copy_use_buffer_test3);
    tcase_add_test(tc_image8_functions_image_tests, BOXING_image8_copy_use_buffer_test4);

    // Test function boxing_image8_is_null
    tcase_add_test(tc_image8_functions_image_tests, BOXING_image8_is_null_test1);
    tcase_add_test(tc_image8_functions_image_tests, BOXING_image8_is_null_test2);
    tcase_add_test(tc_image8_functions_image_tests, BOXING_image8_is_null_test3);

    // Test function boxing_image8_crop
    tcase_add_test(tc_image8_functions_image_tests, BOXING_image8_crop_test1);
    tcase_add_test(tc_image8_functions_image_tests, BOXING_image8_crop_test2);
    tcase_add_test(tc_image8_functions_image_tests, BOXING_image8_crop_test3);
    tcase_add_test(tc_image8_functions_image_tests, BOXING_image8_crop_test4);
    tcase_add_test(tc_image8_functions_image_tests, BOXING_image8_crop_test5);
    tcase_add_test(tc_image8_functions_image_tests, BOXING_image8_crop_test6);
    tcase_add_test(tc_image8_functions_image_tests, BOXING_image8_crop_test7);
    tcase_add_test(tc_image8_functions_image_tests, BOXING_image8_crop_test8);
    tcase_add_test(tc_image8_functions_image_tests, BOXING_image8_crop_test9);
    tcase_add_test(tc_image8_functions_image_tests, BOXING_image8_crop_test10);
    tcase_add_test(tc_image8_functions_image_tests, BOXING_image8_crop_test11);
    tcase_add_test(tc_image8_functions_image_tests, BOXING_image8_crop_test12);
    tcase_add_test(tc_image8_functions_image_tests, BOXING_image8_crop_test13);
    tcase_add_test(tc_image8_functions_image_tests, BOXING_image8_crop_test14);
    tcase_add_test(tc_image8_functions_image_tests, BOXING_image8_crop_test15);
    tcase_add_test(tc_image8_functions_image_tests, BOXING_image8_crop_test16);
    tcase_add_test(tc_image8_functions_image_tests, BOXING_image8_crop_test17);
    tcase_add_test(tc_image8_functions_image_tests, BOXING_image8_crop_test18);
    tcase_add_test(tc_image8_functions_image_tests, BOXING_image8_crop_test19);
    tcase_add_test(tc_image8_functions_image_tests, BOXING_image8_crop_test20);
    tcase_add_test(tc_image8_functions_image_tests, BOXING_image8_crop_test21);
    tcase_add_test(tc_image8_functions_image_tests, BOXING_image8_crop_test22);
    tcase_add_test(tc_image8_functions_image_tests, BOXING_image8_crop_test23);
    tcase_add_test(tc_image8_functions_image_tests, BOXING_image8_crop_test24);
    tcase_add_test(tc_image8_functions_image_tests, BOXING_image8_crop_test25);
    tcase_add_test(tc_image8_functions_image_tests, BOXING_image8_crop_test26);
    tcase_add_test(tc_image8_functions_image_tests, BOXING_image8_crop_test27);

    TCase *tc_image8_macros_tests;
    tc_image8_macros_tests = tcase_create("macros_tests");

    // Test macros IMAGE8_PIXEL
    tcase_add_test(tc_image8_macros_tests, IMAGE8_PIXEL_test1);

    // Test macros IMAGE8_PPIXEL
    tcase_add_test(tc_image8_macros_tests, IMAGE8_PPIXEL_test1);

    // Test macros IMAGE8_SCANLINE
    tcase_add_test(tc_image8_macros_tests, IMAGE8_SCANLINE_test1);

    Suite *s;
    s = suite_create("image8_test_util");
    suite_add_tcase(s, tc_image8_functions_image_tests);
    suite_add_tcase(s, tc_image8_macros_tests);

    return s;
}
