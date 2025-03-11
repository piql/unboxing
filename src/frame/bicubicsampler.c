/*****************************************************************************
**
**  Implementation of the bicubic image interpolation interface
**
**  Creation date:  2017/03/31
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

// Definition
static boxing_image8 * sample(boxing_sampler * sampler, const boxing_image8 * image);


/*! 
  * \addtogroup frame
  * \{
  */


// PUBLIC BICUBIC SAMPLER FUNCTIONS
//

//----------------------------------------------------------------------------
/*!
 *  \brief Create a boxing bicubic sempler instance.
 *
 *  Allocate memory for the boxing_sampler type
 *  and initializes resulting structure with specified values.
 *  Return instance of allocated boxing_sampler structure.
 *
 *  \param[in]  width   Width of the area.
 *  \param[in]  height  Height of the area.
 *  \return instance of allocated boxing_sampler structure.
 */

boxing_sampler * boxing_bicubicsampler_create(int width, int height)
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


// PRIVATE BICUBIC SAMPLER FUNCTIONS
//

static boxing_image8_pixel get_pixel_clamped_bicubic(const boxing_image8 * image, int x, int y)
{
    x = BOXING_MATH_CLAMP(0, (int)image->width - 1, x);
    y = BOXING_MATH_CLAMP(0, (int)image->height - 1, y);
    return image->data[image->width * y + x];
}

// t is a value that goes from 0 to 1 to interpolate in a C1 continuous way across uniformly sampled data points.
// when t is 0, this will return B.  When t is 1, this will return C.  Inbetween values will return an interpolation
// between B and C.  A and B are used to calculate slopes at the edges.
static float cubic_hermite(float A, float B, float C, float D, float t)
{
    float a = -A / 2.0f + (3.0f*B) / 2.0f - (3.0f*C) / 2.0f + D / 2.0f;
    float b = A - (5.0f*B) / 2.0f + 2.0f*C - D / 2.0f;
    float c = -A / 2.0f + C / 2.0f;
    float d = B;

    return a*t*t*t + b*t*t + c*t + d;
}

static boxing_image8_pixel sample_bicubic(const boxing_image8 * image, float u, float v)
{
    // calculate coordinates -> also need to offset by half a pixel to keep image from shifting down and left half a pixel
    float x = u * (image->width - 1);
    int xint = (int)x;
    float xfract = x - (float)floor(x);

    float y = v * (image->height - 1);
    int yint = (int)y;
    float yfract = y - (float)floor(y);

    // 1st row
    boxing_image8_pixel p00 = get_pixel_clamped_bicubic(image, xint - 1, yint - 1);
    boxing_image8_pixel p10 = get_pixel_clamped_bicubic(image, xint + 0, yint - 1);
    boxing_image8_pixel p20 = get_pixel_clamped_bicubic(image, xint + 1, yint - 1);
    boxing_image8_pixel p30 = get_pixel_clamped_bicubic(image, xint + 2, yint - 1);

    // 2nd row
    boxing_image8_pixel p01 = get_pixel_clamped_bicubic(image, xint - 1, yint + 0);
    boxing_image8_pixel p11 = get_pixel_clamped_bicubic(image, xint + 0, yint + 0);
    boxing_image8_pixel p21 = get_pixel_clamped_bicubic(image, xint + 1, yint + 0);
    boxing_image8_pixel p31 = get_pixel_clamped_bicubic(image, xint + 2, yint + 0);

    // 3rd row
    boxing_image8_pixel p02 = get_pixel_clamped_bicubic(image, xint - 1, yint + 1);
    boxing_image8_pixel p12 = get_pixel_clamped_bicubic(image, xint + 0, yint + 1);
    boxing_image8_pixel p22 = get_pixel_clamped_bicubic(image, xint + 1, yint + 1);
    boxing_image8_pixel p32 = get_pixel_clamped_bicubic(image, xint + 2, yint + 1);

    // 4th row
    boxing_image8_pixel p03 = get_pixel_clamped_bicubic(image, xint - 1, yint + 2);
    boxing_image8_pixel p13 = get_pixel_clamped_bicubic(image, xint + 0, yint + 2);
    boxing_image8_pixel p23 = get_pixel_clamped_bicubic(image, xint + 1, yint + 2);
    boxing_image8_pixel p33 = get_pixel_clamped_bicubic(image, xint + 2, yint + 2);

    // interpolate bi-cubically!
    // Clamp the values since the curve can put the value below 0 or above 255
    boxing_image8_pixel ret;

    float col0 = cubic_hermite(p00, p10, p20, p30, xfract);
    float col1 = cubic_hermite(p01, p11, p21, p31, xfract);
    float col2 = cubic_hermite(p02, p12, p22, p32, xfract);
    float col3 = cubic_hermite(p03, p13, p23, p33, xfract);
    float value = cubic_hermite(col0, col1, col2, col3, yfract);
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
            *pixel++ = sample_bicubic(image, u, v);
        }
    }

    return out_image;
}
