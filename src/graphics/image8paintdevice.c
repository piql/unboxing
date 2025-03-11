/*****************************************************************************
**
**  Implementation of the boxing_image8paintdevice interface
**
**  Creation date:  2014/12/16
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
#include    "boxing/graphics/image8paintdevice.h"
#include    "boxing/platform/platform.h"
#include    "boxing/utils.h"

//  DEFINES
//

#define DEVICE_IMAGE (((boxing_image8paintdevice*)device)->image)

//  PRIVATE INTERFACE
//

static void draw_line(boxing_paintdevice *device, const boxing_recti * clip_region, int x1, int y1, int x2, int y2, const int color);
static void draw_rectangle(boxing_paintdevice *device, const boxing_recti * clip_region, int x, int y, int width, int height, const int color);
static void draw_image(boxing_paintdevice *device, const boxing_recti * clip_region, int x, int y, const char * image, int scanline_width, int sx, int sy, int sw, int sh);
static void fill_rectangle(boxing_paintdevice *device, const boxing_recti * clip_region, int x, int y, int width, int height, const int color);
static int  width(boxing_paintdevice *device);
static int  height(boxing_paintdevice *device);

static void image8paintdevice_free(boxing_paintdevice *device);


/*! 
  * \addtogroup graphics
  * \{
  */


//----------------------------------------------------------------------------
/*!
 *  \struct     boxing_image8paintdevice_s  image8paintdevice.h
 *  \brief      Image paing device structure.
 *  
 *  \param base   Base boxing_paintdevice structure instance.
 *  \param image  boxing_image8 structure instance.
 *
 * Image paing device structure description.
 */


// PUBLIC IMAGE8 PAINT DEVICE FUNCTIONS
//

//----------------------------------------------------------------------------
/*!
 *  \brief Create a boxing_paintdevice instance.
 *
 *  Allocate memory for the boxing_image8paintdevice type.
 *  Return instance of allocated structure.
 *
 *  \param[in]  image  Image.
 *  \return instance of allocated boxing_paintdevice structure.
 */

boxing_paintdevice * boxing_image8paintdevice_create(boxing_image8 * image)
{
    boxing_image8paintdevice * device = malloc(sizeof(boxing_image8paintdevice));
    device->image = *image;
    device->image.is_owning_data = DFALSE;

    device->base.draw_image = draw_image;
    device->base.draw_line = draw_line;
    device->base.draw_rect = draw_rectangle;
    device->base.fill_rect = fill_rectangle;
    device->base.free = image8paintdevice_free;
    device->base.width = width;
    device->base.height = height;

    return (boxing_paintdevice *)device;
}


//----------------------------------------------------------------------------
/*!
  * \} end of graphics group
  */


// PRIVATE IMAGE8 PAINT DEVICE FUNCTIONS
//

static void image8paintdevice_free(boxing_paintdevice *device)
{
    free(device);
}


static void draw_line(boxing_paintdevice *device, const boxing_recti * clip_region, int x1, int y1, int x2, int y2, const int color)
{
    BOXING_UNUSED_PARAMETER( clip_region );

    const DBOOL steep = (abs(y2 - y1) > abs(x2 - x1));
    if(steep)
    {
        int tmp = x1;
        x1 = y1;
        y1 = tmp;

        tmp = x2;
        x2 = y2;
        y2 = tmp;
    }

    if(x1 > x2)
    {
        int tmp = x1;
        x1 = x2;
        x2 = tmp;

        tmp = y1;
        y1 = y2;
        y2 = tmp;
    }

    const int dx = x2 - x1;
    const int dy = abs(y2 - y1);

    boxing_float error = dx / 2.0f;
    const int ystep = (y1 < y2) ? 1 : -1;
    int y = y1;

    const int max_x = x2 + 1;

    if(steep)
    {
        for(int x = x1; x < max_x; x++)
        {
            IMAGE8_PIXEL(&DEVICE_IMAGE, y, x) = (boxing_image8_pixel)color;
            error -= dy;
            if(error < 0)
            {
                y += ystep;
                error += dx;
            }
        }
    }
    else
    {
        for(int x  = x1; x < max_x; x++)
        {
            IMAGE8_PIXEL(&DEVICE_IMAGE, x, y) = (boxing_image8_pixel)color;
            error -= dy;
            if(error < 0)
            {
                y += ystep;
                error += dx;
            }
        }
    }

}

static void draw_rectangle(boxing_paintdevice *device, const boxing_recti * clip_region, int x, int y, int width, int height, const int color)
{
    // top horizontal line
    draw_line(device, clip_region, x, y, x+width-1, y, color);
    // bottom horizontal line
    draw_line(device, clip_region, x, y+height-1, x+width-1, y+height-1, color);
    // left vertical line
    draw_line(device, clip_region, x, y, x, y+height-1, color);
    // right vertical line
    draw_line(device, clip_region, x+width-1, y, x+width-1, y+height-1, color);
}

static void fill_rectangle(boxing_paintdevice *device, const boxing_recti * clip_region, int x, int y, int width, int height, const int color)
{
    boxing_recti region = {x,y,width,height};
    boxing_math_recti_intersect(&region, clip_region);

    if((region.width == 0) || (region.height == 0))
        return;

    // populate one line
    boxing_image8_pixel *pixel_base = IMAGE8_SCANLINE(&DEVICE_IMAGE, region.y) + region.x;
    boxing_image8_pixel *pixel_pointer  = pixel_base;

    for(int i = 0; i < region.width; i++, pixel_pointer++)
    {
        *pixel_pointer = 0xFF & color;
    }

    pixel_pointer = pixel_base + DEVICE_IMAGE.width;

    for(int i = 1; i < region.height; i++, pixel_pointer += DEVICE_IMAGE.width)
    {
        memcpy(pixel_pointer, pixel_base, region.width);
    }
}

static void draw_image(boxing_paintdevice *device, const boxing_recti * clip_region, int x, int y, const char * image, int scanline_width, int sx, int sy, int sw, int sh)
{
    boxing_recti region = {x,y,sw,sh};
    boxing_math_recti_intersect(&region, clip_region);
    
    if(region.width * region.height == 0)
        return;

    boxing_image8_pixel *pixel_destination = IMAGE8_SCANLINE(&DEVICE_IMAGE, region.y) + region.x;
    const char          *pixel_source = image + (scanline_width*sy  + sx);
    const int            width    = DEVICE_IMAGE.width;

    for(int i = 0; i < region.height; i++)
    {
        memcpy(pixel_destination, pixel_source, region.width);
        pixel_source += scanline_width;
        pixel_destination += width;    
    }
}

static int width(boxing_paintdevice *device)
{
    return DEVICE_IMAGE.width;
}

static int height(boxing_paintdevice *device)
{
    return DEVICE_IMAGE.height;
}
