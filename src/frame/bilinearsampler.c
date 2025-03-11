/*****************************************************************************
**
**  Implementation of the bicubic image interpolation interface
**
**  Creation date:  2017/03/29
**  Created by:     Alexander Silokhin
**
**
**  Copyright (c) 2017 Piql AS. All rights reserved.
**
**  This file is part of the boxing library
**
*****************************************************************************/

//  PROJECT INCLUDES
//
#include "bicubicsampler.h"
#include "boxing/platform/platform.h"

//  PRIVATE INTERFACE
//

// Declaration
static boxing_image8 * sample(boxing_sampler * sampler, const boxing_image8 * image);


/*! 
  * \addtogroup frame
  * \{
  */


// PUBLIC BILINEAR SAMPLER FUNCTIONS
//

//----------------------------------------------------------------------------
/*!
 *  \brief Create a boxing bilinear sempler instance.
 *
 *  Allocate memory for the boxing_sampler type
 *  and initializes resulting structure with specified values.
 *  Return instance of allocated boxing_sampler structure.
 *
 *  \param[in]  width   Width of the area.
 *  \param[in]  height  Height of the area.
 *  \return instance of allocated boxing_sampler structure.
 */

boxing_sampler * boxing_bilinearsampler_create(int width, int height)
{
    boxing_sampler * sampler = malloc(sizeof(boxing_sampler));
    boxing_sampler_init(sampler, width, height);
    sampler->sample = sample;
    return sampler;
}


//----------------------------------------------------------------------------
/*!
  * \} end of frame group
  */


// PRIVATE BILINEAR SAMPLER FUNCTIONS
//

static boxing_image8_pixel get_pixel_clamped(const boxing_image8 * image, int x, int y)
{
    x = BOXING_MATH_CLAMP(0, (int)image->width - 1, x);
    y = BOXING_MATH_CLAMP(0, (int)image->height - 1, y);
    return image->data[image->width * y + x];
}

static float lerp(float s, float e, float t)
{
    return s+(e-s)*t;
}

static float blerp(float c00, float c10, float c01, float c11, float tx, float ty)
{
    return lerp(lerp(c00, c10, tx), lerp(c01, c11, tx), ty);
}

static boxing_image8_pixel sample_bilinear(const boxing_image8 * image, float u, float v)
{
    float xfract = u * (float)(image->width - 1);
    int xint = (int)xfract;

    float yfract = v * (float)(image->height - 1);
    int yint = (int)yfract;

    boxing_image8_pixel c00 = get_pixel_clamped(image, xint, yint);
    boxing_image8_pixel c10 = get_pixel_clamped(image, xint + 1, yint);
    boxing_image8_pixel c01 = get_pixel_clamped(image, xint, yint + 1);
    boxing_image8_pixel c11 = get_pixel_clamped(image, xint + 1, yint + 1);

    // interpolate bi-linearly!
    boxing_image8_pixel ret;

    float value = blerp(c00, c10, c01, c11, xfract - xint, yfract - yint);
    value = BOXING_MATH_CLAMP(0.0f, 255.0f, value);
    ret = (boxing_image8_pixel)value;

    return ret;
}

// Definition
static boxing_image8 * sample(boxing_sampler * sampler, const boxing_image8 * image)
{
    if ( boxing_image8_is_null(image) )
    {
        return NULL;
    }

    boxing_image8 * out_image = boxing_image8_create(sampler->location_matrix.width, sampler->location_matrix.height);
    boxing_image8_pixel * pixel = out_image->data;
    for(unsigned int i = 0; i < sampler->location_matrix.height; i++)
    {
        const boxing_pointf * scan_line = MATRIX_ROW(&sampler->location_matrix, i);
        const boxing_pointf * scan_line_end = scan_line + sampler->location_matrix.width;
        float v = (float)i / (float)(out_image->height);
        for (unsigned int j = 0; scan_line != scan_line_end; ++scan_line, ++j)
        {
            float u = (float)j / (float)(out_image->width);
            *pixel++ = sample_bilinear(image, u, v);
        }
    }

    return out_image;
}
