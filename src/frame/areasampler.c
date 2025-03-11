/*****************************************************************************
**
**  Implementation of the area sampling interfacing
**
**  Creation date:  2014/01/06
**  Created by:     Haakon Larsson
**
**
**  Copyright (c) 2014 Piql AS. All rights reserved.
**
**  This file is part of the boxing library
**
*****************************************************************************/

//  PROJECT INCLUDES
//
#include "areasampler.h"
#include "boxing/platform/platform.h"

// PRIVATE INTERFACE
//
static boxing_image8 * sample(boxing_sampler * sampler, const boxing_image8 * image);


/*! 
  * \addtogroup frame
  * \{
  */


//----------------------------------------------------------------------------
/*!
 *  \struct     boxing_areasampler_s  areasampler.h
 *  \brief      Area sempler data storage.
 *
 *  \param base    boxing_sampler structure.
 *  \param radius  Radius of the area.
 *
 *  Structure for storing area sampler data.
 */


// PUBLIC AREA SAMPLER FUNCTIONS
//

//----------------------------------------------------------------------------
/*!
 *  \brief Create a boxing area sempler instance.
 *
 *  Allocate memory for the boxing_areasampler type, converts structure to boxing_sampler
 *  and initializes resulting structure data with specified values.
 *  Return instance of allocated boxing_areasampler structure which is converted into boxing_sampler structure.
 *
 *  \param[in]  width   Width of the area.
 *  \param[in]  height  Height of the area.
 *  \return instance of allocated boxing_areasampler structure which is converted into boxing_sampler structure.
 */

boxing_sampler * boxing_areasampler_create(int width, int height)
{
    boxing_sampler * sampler = malloc(sizeof(boxing_areasampler));
    boxing_sampler_init(sampler, width, height);
    sampler->sample = sample;
    return sampler;
}


//----------------------------------------------------------------------------
/*!
 *  \brief Sets the radius of the area in structure boxing_areasampler.
 *
 *  Sets the radius of the area in structure boxing_areasampler.
 *
 *  \param[in]  sampler  Pointer to the boxing_areasampler structure.
 *  \param[in]  radius   New radius.
 */

void boxing_areasampler_set_radius(boxing_areasampler * sampler, int radius)
{
    sampler->radius = radius;
}


//----------------------------------------------------------------------------
/*!
  * \} end of frame group
  */


// PRIVATE AREA SAMPLER FUNCTIONS
//

static boxing_image8 * sample(boxing_sampler * sampler, const boxing_image8 * image)
{

    if ( boxing_image8_is_null(image) )
    {
        return NULL;
    }

    boxing_image8 * out_image = boxing_image8_create(sampler->location_matrix.width, sampler->location_matrix.height);
    boxing_image8_pixel * pixel = out_image->data;
    int radius = ((boxing_areasampler*)sampler)->radius;

    for(unsigned int i = 0; i < sampler->location_matrix.height; i++)
    {
        boxing_pointf *scan_line = MATRIX_ROW(&sampler->location_matrix, i);
        const boxing_pointf *scan_line_end = scan_line + sampler->location_matrix.width;
        for (; scan_line != scan_line_end; scan_line++)
        {
            boxing_float x = scan_line->x;
            boxing_float y = scan_line->y;
            boxing_float res = 0.0f;
            int x_start = (int)(x - radius);
            int x_end   = x_start + radius*2 + 1;
            int y_start = (int)(y - radius);
            int y_end   = y_start + radius*2 + 1;

            for(int yi = y_start; yi < y_end; yi++)
            {
                boxing_image8_pixel * ptr = IMAGE8_SCANLINE(image, yi);
                for(int xi = x_start; xi < x_end; xi++)
                {
                    res += ptr[xi];
                }
            }
            res /= (radius*2+1)*(radius*2+1);
            if(res < 0.0f)
                *pixel++ = BOXING_PIXEL_MIN;
            else if(res > 255.0f)
                *pixel++ = BOXING_PIXEL_MAX;
            *pixel++ = (boxing_image8_pixel)res;
        }
    }

    return out_image;
}
