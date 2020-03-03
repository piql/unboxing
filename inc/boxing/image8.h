#ifndef BOXING_IMAGE8_H
#define BOXING_IMAGE8_H

/*****************************************************************************
**
**  Definition of the image8 C interface
**
**  Creation date:  2014/12/16
**  Created by:     Piql AS
**
**
**  Copyright (c) 2014 Piql AS. All rights reserved.
**
**  This file is part of the boxing library
**
*****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

//  PROJECT INCLUDES
//
#include "boxing/bool.h"
#include "boxing/math/math.h"

//  DEFINE
//
#define BOXING_PIXEL_MIN     0
#define BOXING_PIXEL_MAX     255
#define BOXING_PIXEL_LEVELS  256

//  TYPES
//
typedef unsigned char boxing_image8_pixel;

typedef struct boxing_image8_s
{
    unsigned int width;
    unsigned int height;
    DBOOL is_owning_data;
    boxing_image8_pixel * data;
} boxing_image8;

//  FUNCTIONS
//

boxing_image8 * boxing_image8_create(unsigned int width, unsigned int height);
boxing_image8 * boxing_image8_create2(const boxing_image8_pixel * buffer, unsigned int width, unsigned int height);
boxing_image8 * boxing_image8_recreate(boxing_image8 * image, unsigned int width, unsigned int height);
void            boxing_image8_init_in_place(boxing_image8 * image, unsigned int width, unsigned int height);
void            boxing_image8_reinit_in_place(boxing_image8 * image, unsigned int width, unsigned int height);
void            boxing_image8_free(boxing_image8 * image);
void            boxing_image8_free_in_place(boxing_image8 * image);
boxing_image8 * boxing_image8_copy(const boxing_image8 * image);
boxing_image8 * boxing_image8_copy_use_buffer(const boxing_image8 * image);
DBOOL           boxing_image8_is_null(const boxing_image8 * image);
boxing_image8 * boxing_image8_crop(const boxing_image8 * image, int x_offset, int y_offset, int width, int height);
boxing_image8 * boxing_image8_rotate(const boxing_image8 * image, int rotation);

#define IMAGE8_PIXEL(image, x, y) ((image)->data[(x) + (y) * (image)->width])
#define IMAGE8_PPIXEL(image, x, y) ((image)->data + (x) + (y) * (image)->width)

#define IMAGE8_SCANLINE(image, y) ((image)->data + (y) * (image)->width)

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
