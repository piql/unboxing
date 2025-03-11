/*****************************************************************************
**
**  Implementation of the image interface
**
**  Creation date:  2016/06/28
**  Created by:     Piql
**
**
**  Copyright (c) 2016 Piql AS. All rights reserved.
**
**  This file is part of the boxing library
**
*****************************************************************************/

//  PROJECT INCLUDES
//
#include "boxing/image8.h"
#include "boxing/log.h"


//----------------------------------------------------------------------------
/*!
 *  \defgroup   image  Images
 *  \brief      Two dimensional image with grayscale 8bit image data.
 *  \ingroup    unbox
 *
 *  Structures and functions to work with a 2 dimensional image with grayscale 8bit image data,
 *  meaning one byte is used to store light intensity from 0 to 255, where 0 is black
 *  (no light) and 255 is white (maximum light).
 */


/*! 
  * \addtogroup image
  * \{
  */


//----------------------------------------------------------------------------
/*!
 *  \def BOXING_PIXEL_MIN() image8.h
 *  \brief A macro for value of minimum light intensity.
 *
 *  A macro for value of minimum light intensity.
 *  Corresponds to a value "0" of integer type.
 */


//----------------------------------------------------------------------------
/*!
 *  \def BOXING_PIXEL_MAX() image8.h
 *  \brief A macro for value of maximum light intensity.
 *
 *  A macro for value of maximum light intensity.
 *  Corresponds to a value "255" of integer type.
 */


//----------------------------------------------------------------------------
/*!
 *  \def BOXING_PIXEL_LEVELS() image8.h
 *  \brief A macro for value of count levels for the light intensity.
 *
 *  A macro for value of count levels for the light intensity.
 *  Corresponds to a value "256" of integer type.
 */


//----------------------------------------------------------------------------
/*!
 *  \def IMAGE8_PIXEL(image, x, y) image8.h
 *  \brief Obtaining state for the point of the image.
 *
 *  A macro for obtaining state for the point of the image specified by its coordinates.
 *
 *  \param[in]  image  Image.
 *  \param[in]  x      X-coordinate.
 *  \param[in]  y      Y-coordinate.
 */


//----------------------------------------------------------------------------
/*!
 *  \def IMAGE8_PPIXEL(image, x, y) image8.h
 *  \brief Obtaining a pointer for the point of the image.
 *
 *  A macro for obtaining a pointer for the point of the image specified by its coordinates.
 *
 *  \param[in]  image  Image.
 *  \param[in]  x      X-coordinate.
 *  \param[in]  y      Y-coordinate.
 */


//----------------------------------------------------------------------------
/*!
 *  \def IMAGE8_PPIXEL(image, y) image8.h
 *  \brief Obtaining a pointer for the line of the image.
 *
 *  A macro for obtaining a pointer for the line of the image specified by Y-coordinate.
 *
 *  \param[in]  image  Image.
 *  \param[in]  y      Y-coordinate.
 */


/*! 
 *  \var typedef unsigned char boxing_image8_pixel
 *  \brief Type definitions for specifying a grayscale for an image pixel.
 *   
 *  Type definitions for specifying a grayscale variable for an image pixel.
 *  This type is used to store light intensity from 0 to 255, where 0 is black (no light) and 255 is white (maximum light).
 */


//----------------------------------------------------------------------------
/*!
 *  \struct     boxing_image8_s  image8.h
 *  \brief      Image data storage.
 *
 *  \param width           Width of the image.
 *  \param height          Height of the image.
 *  \param is_owning_data  If true the structure is managing the data buffer.
 *  \param data            Pointer to the array data of image
 *
 *  The image struct represents a 2 dimensional image with grayscale 8bit image data,
 *  meaning one byte is used to store light intensity from 0 to 255, where 0 is black 
 *  (no light) and 255 is white (maximum light).
 *  If is_owning_data is set to true the structure is responsible for managing the buffer
 *  with image data and the data will be freed when calling boxing_image8_free, otherwize
 *  the caller is responsible for the buffer management.
 */

// PUBLIC IMAGE8 FUNCTIONS
//

//----------------------------------------------------------------------------
/*!
 *  \brief Create an image with specified sizes.
 *
 *  Create an empty image with specified width and height,
 *  with is_owning_data set to true.
 *  If width or height equal to zero, image does not create and function return NULL.
 *  If memory allocation fails, the function returns NULL.
 *
 *  \param[in]  width     The width of the image that is created.
 *  \param[in]  height    The height of the image that is created.
 *  \return created image with specified sizes.
 */

boxing_image8 *boxing_image8_create(unsigned int width, unsigned int height)
{
    if (width == 0 || height == 0)
    {
        DLOG_ERROR("(boxing_image8_create) Failed to create image with null size");
        return NULL;
    }

    boxing_image8 *image = malloc(sizeof(boxing_image8));
    if (image == NULL)
    {
        DLOG_ERROR( "(boxing_image8_create) Failed to allocate image" );
        return NULL;
    }

    image->width = width;
    image->height = height;
    image->data = malloc(width * height);
    image->is_owning_data = DTRUE;
    if (image->data == NULL && width * height != 0)
    {
        free(image);
        DLOG_ERROR("(boxing_image8_create) Failed to allocate image buffer");
        return NULL;
    }
    return image;
}


//----------------------------------------------------------------------------
/*!
 *  \brief Create an image with specified sizes and with existing data.
 *
 *  Create an image with specified width and height,
 *  with is_owning_data set to false.
 *  If width or height equal to zero, image does not create and function return NULL.
 *  If memory allocation fails, the function returns NULL.
 *
 *  \param[in]  buffer    The pointer to image data from other image instance.
 *  \param[in]  width     The width of the image that is created.
 *  \param[in]  height    The height of the image that is created.
 *  \return created image with specified sizes.
 */

boxing_image8 *boxing_image8_create2(const boxing_image8_pixel * buffer, unsigned int width, unsigned int height)
{
    if (width == 0 || height == 0)
    {
        DLOG_ERROR("(boxing_image8_create2) Failed to create image with null size");
        return NULL;
    }

    boxing_image8 *image = malloc(sizeof(boxing_image8));
    if (image == NULL)
    {
        DLOG_ERROR( "(boxing_image8_create2) Failed to allocate image" );
        return NULL;
    }

    image->width = width;
    image->height = height;
    image->data = malloc(width * height);
    image->is_owning_data = DTRUE;
    if (image->data == NULL)
    {
        free(image);
        DLOG_ERROR("(boxing_image8_create2) Failed to allocate image buffer");
        return NULL;
    }
    else
    {
	memcpy(image->data, buffer, width * height);
    }
    return image;
}


//----------------------------------------------------------------------------
/*!
 *  \brief Initialize an existing image with new width and height.
 *
 *  Initialize an existing image with new specified width and height,
 *  with is_owning_data set to true.
 *  If width or height equal to zero, image does not recreate and function return NULL, 
 *  all memory for image data are free.
 *  If input image is NULL, creates the new instance of the image.
 *  If pointer to image data is NULL, allocates a new memory for image data.
 *
 *  \param[in]  image     The pointer to image instance.
 *  \param[in]  width     New width of the image.
 *  \param[in]  height    New height of the image.
 *  \return image with new sizes.
 */

boxing_image8 * boxing_image8_recreate(boxing_image8 * image, unsigned int width, unsigned int height)
{
    if (width == 0 || height == 0)
    {
        if (image != NULL)
        {
            boxing_image8_free(image);
        }

        return NULL;
    }

    if (image == NULL)
    {
        return boxing_image8_create(width, height);
    }
    else
    {
        if (image->width == width && image->height == height && image->data != NULL && image->is_owning_data)
        {
            return image;
        }
        else
        {
            if (image->is_owning_data && image->data != NULL)
            {
                free(image->data);
                image->data = NULL;
            }

            boxing_image8_reinit_in_place(image, width, height);
            return image;
        }
    }
}


//----------------------------------------------------------------------------
/*!
 *  \brief Initialize image data in the existing image with new width and height.
 *
 *  Initialize image data in the existing image with new specified width and height,
 *  with is_owning_data set to true.
 *  If width or height equal to zero, image data does not initialize and function 
 *  return image with image data pointer equal to NULL, all memory for image data are free.
 *  If input image is NULL, image data does not initialize and function return NULL.
 *  If pointer to image data is NULL, allocates a new memory for image data.
 *
 *  \param[in]  image     The pointer to image instance.
 *  \param[in]  width     New width of the image.
 *  \param[in]  height    New height of the image.
 *  \return image with new sizes.
 */

void boxing_image8_reinit_in_place(boxing_image8 * image, unsigned int width, unsigned int height)
{
    if (image == NULL)
    {
        return;
    }

    image->is_owning_data = DTRUE;
    image->width = width;
    image->height = height;
    if (width == 0 || height == 0)
    {
        free(image->data);
        image->data = NULL;
    }
    else
    {
        image->data = malloc(width * height);
    }
    
    if (image->data == NULL && width * height != 0)
    {
        boxing_throw("Failed to init image in place!");
    }
}


//----------------------------------------------------------------------------
/*!
 *  \brief Initialize image data in the new image with new width and height.
 *
 *  Initialize image data in the new image with uninitialized data,
 *  with specified width and height, after initialization is_owning_data set to true.
 *  If width or height equal to zero, image data does not initialize and function
 *  return image with image data pointer equal to NULL, all memory for image data are free.
 *  If input image is NULL, image data does not initialize and function return NULL.
 *
 *  \param[in]  image     The pointer to image instance.
 *  \param[in]  width     Width of the image.
 *  \param[in]  height    Height of the image.
 *  \return image with specified sizes.
 */

void boxing_image8_init_in_place(boxing_image8 * image, unsigned int width, unsigned int height)
{
    if (image == NULL)
    {
        return;
    }

    image->data = NULL;
    boxing_image8_reinit_in_place(image, width, height);
}


//----------------------------------------------------------------------------
/*!
 *  \brief Frees occupied memory of image
 *
 *  Frees occupied memory of image data and image instance.
 *
 *  \param[in]  image     The pointer to image instance.
 */

void boxing_image8_free(boxing_image8 * image)
{
    boxing_image8_free_in_place(image);
    free(image);
}


//----------------------------------------------------------------------------
/*!
 *  \brief Frees occupied memory of image data
 *
 *  Frees occupied memory of image data.
 *
 *  \param[in]  image     The pointer to image instance.
 */

void boxing_image8_free_in_place( boxing_image8 * image )
{
    if (image != NULL)
    {
        if (image->is_owning_data)
        {
            free(image->data);
            image->data = NULL;
        }
    }
}


//----------------------------------------------------------------------------
/*!
 *  \brief Creates a copy of input image.
 *
 *  Creates a copy of input image.
 *  If input image is NULL, image will not be copied and function return NULL.
 *  If pointer to input image data is NULL, creates an image with image data pointer equal to NULL.
 *  If image data does not owning input image, 
 *  after copying the image data will belong to the created copy of image.
 *
 *  \param[in]  image     The pointer to image instance.
 *  \return copy of input image.
 */

boxing_image8 *boxing_image8_copy(const boxing_image8 *image)
{
    if (image == NULL)
    {
        return NULL;
    }

    if (image != NULL && image->data == NULL)
    {
        boxing_image8 *emptycopy = boxing_image8_create(1, 1);
        boxing_image8_free_in_place(emptycopy);
        return emptycopy;
    }

    boxing_image8 *copy = boxing_image8_create(image->width, image->height);
    memcpy(copy->data, image->data, image->width * image->height);
    return copy;
}


//----------------------------------------------------------------------------
/*!
 *  \brief Creates a copy of input image with data belongs input image.
 *
 *  Creates a copy of input image with data belongs input image.
 *  If input image is NULL, image will not be copied and function return NULL.
 *  After copying the image data will not belong to the created copy of image.
 *
 *  \param[in]  image     The pointer to image instance.
 *  \return copy of input image.
 */

boxing_image8 * boxing_image8_copy_use_buffer(const boxing_image8 * image)
{
    if (image == NULL)
    {
        return NULL;
    }

    boxing_image8 * copy = malloc(sizeof(boxing_image8));
    memcpy(copy, image, sizeof(boxing_image8));
    copy->is_owning_data = DFALSE;
    return copy;
}


//----------------------------------------------------------------------------
/*!
 *  \brief Determine whether the image is empty.
 *
 *  If pointer to the image is NULL or
 *  image width or height equal to zero or 
 *  pointer to image data is NULL, then we return true.
 *  Else we return false.
 *
 *  \param[in]  image     The pointer to image instance.
 *  \return indication whether the image is empty.
 */

DBOOL boxing_image8_is_null(const boxing_image8 * image)
{
    return image == NULL || image->width == 0 || image->height == 0 || image->data == NULL;
}


//----------------------------------------------------------------------------
/*!
 *  \brief Copy the data from the specified area of the input image.
 *
 *  Copy the data from the specified area of the input image. 
 *  Area specified by using coordinates of the upper left corner and the width and height
 *  of the rectangular area.
 *  Else we return false.
 *
 *  \param[in]  image     The pointer to image instance.
 *  \param[in]  x_offset  The X coordinate of the upper left corner of the copied area.
 *  \param[in]  y_offset  The Y coordinate of the upper left corner of the copied area.
 *  \param[in]  width     The width of the copied area.
 *  \param[in]  height    The height of the copied area.
 *  \return image instance with copied data.
 */

boxing_image8 * boxing_image8_crop(const boxing_image8 * image, int x_offset, int y_offset, int width, int height)
{
    if (image == NULL)
    {
        return NULL;
    }

    boxing_image8 * crop;
    if (x_offset < 0)
    {
        width += x_offset;
        x_offset = 0;
    }
    if (y_offset < 0)
    {
        height += y_offset;
        y_offset = 0;
    }
    if (x_offset + width > (int)image->width)
    {
        width = image->width - x_offset;
    }
    if (y_offset + height > (int)image->height)
    {
        height = image->height - y_offset;
    }
    if (width <= 0 || height <= 0)
    {
        return NULL;
    }
    crop = boxing_image8_create(width, height);
    for(unsigned int y = 0; y < crop->height; y++)
    {
        const boxing_image8_pixel *src = image->data + (y + y_offset) * image->width + x_offset;
        boxing_image8_pixel *dst = crop->data + y * width;
        memcpy(dst, src, width);
    }

    return crop;
}

/** Crop image data in place. Returns true if a crop was performed. */
DBOOL boxing_image8_crop_in_place(
    /** Image instance to operate on */
    boxing_image8 *image,
    /** X coordinate of the top left corner to start cropping from */
    int x_offset,
    /** Y coordinate of the top left corner to start cropping from */
    int y_offset,
    /** Width of the cropped (kept) area */
    int width,
    /** Height of the cropped (kept) area */
    int height
) {
    if (image == NULL) return DFALSE;
    if (x_offset < 0) {
        width += x_offset;
        x_offset = 0;
    }
    if (y_offset < 0) {
        height += y_offset;
        y_offset = 0;
    }

    if (x_offset + width > (int)image->width) width = image->width - x_offset;
    if (y_offset + height > (int)image->height) height = image->height - y_offset;
    if (width <= 0 || height <= 0) return DFALSE;
    if ((unsigned)width * (unsigned)height > image->width * image->height) {
        void *new_data = realloc(image->data, width * height);
        if (new_data == NULL) return DFALSE;
        image->data = new_data;
    }
    if ((unsigned)width > image->width) {
        for (unsigned int y = height; y-- > 0;) {
            const boxing_image8_pixel *src = image->data + (y + y_offset) * image->width + x_offset;
            boxing_image8_pixel *dst = image->data + y * width;
            memmove(dst, src, width);
        }
    } else {
        for (unsigned int y = 0; y < height; y++) {
            const boxing_image8_pixel *src = image->data + (y + y_offset) * image->width + x_offset;
            boxing_image8_pixel *dst = image->data + y * width;
            memmove(dst, src, width);
        }
    }
    if ((unsigned)width * (unsigned)height < image->width * image->height) {
        void *new_data = realloc(image->data, width * height);
        if (new_data != NULL) image->data = new_data;
    }
    image->width = (unsigned)width;
    image->height = (unsigned)height;
    return DTRUE;
}


//----------------------------------------------------------------------------
/*!
 *  \brief Rotate the input image.
 *
 *  Rotate the input image at an angle multiple of 90 degrees an return the new instance of rotated image.
 *  The image is rotated only for the following values of the input variable rotation:
 *  rotation = 90 - Rotate image counterclockwise;
 *  rotation = 180 - Just flips the image;
 *  rotation = 270 - Rotate image clockwise.
 *  Else we just return copy of input image.
 *
 *  \param[in]  image     The pointer to image instance.
 *  \param[in]  rotation  The angle at which must be rotate the image.
 *  \return image instance with rotated image data.
 */

boxing_image8 * boxing_image8_rotate(const boxing_image8 * image, int rotation)
{
    if (NULL == image)
    {
        return NULL;
    }

    boxing_image8 * result_image = NULL;

    switch(rotation)
    {
        case 90:
        {
            result_image = boxing_image8_create(image->height, image->width);

            //Rotate image counterclockwise
            for (unsigned int i = 0; i < image->height; ++i)
            {
                for (unsigned int j = 0; j < image->width; ++j)
                {
                    result_image->data[(image->width - 1 - j) * image->height + i] =
                            image->data[i * image->width + j];
                }
            }
        }
        break;
        case 180:
        {
            result_image = boxing_image8_create(image->width, image->height);

            //Just invert an image
            const unsigned int image_size = image->height * image->width;
            for (unsigned int i = 0; i < image_size; ++i)
            {
                result_image->data[i] = image->data[image_size - 1 - i];
            }
        }
        break;
        case 270:
        {
            result_image = boxing_image8_create(image->height, image->width);

            //Rotate image clockwise
            for (unsigned int i = 0; i < image->height; ++i)
            {
                for (unsigned int j = 0; j < image->width; ++j)
                {
                    result_image->data[j * image->height + (image->height - 1 - i)] =
                            image->data[i * image->width + j];
                }
            }
        }
        break;
        default:
        {
            result_image = boxing_image8_copy(image);
        }
        break;
    }

    return result_image;
}


//----------------------------------------------------------------------------
/*!
  * \} end of image group
  */
