/*****************************************************************************
**
**  Implementation of the painter interface
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
#include    "boxing/graphics/painter.h"

//  BASE INCLUDES
//
#include    "boxing/log.h"

//  PRIVATE INTERFACE
//

static void set_callbacks(boxing_painter *painter);


/*! 
  * \addtogroup graphics
  * \{
  */


//----------------------------------------------------------------------------
/*!
 *  \struct  boxing_painter_s  painter.h
 *  \brief   Painter data storage structure.
 *  
 *  \param draw_line   Function to draw line. void (*draw_line)(struct boxing_painter_s * painter, int x1, int y1, int x2, int y2, const int color)
 *  \param draw_rect   Function to draw rectangle. void (*draw_rect)(struct boxing_painter_s * painter, int x, int y, int width, int height, const int color)
 *  \param fill_rect   Function to fill rectangle. void (*fill_rect)(struct boxing_painter_s * painter, int x, int y, int width, int height, const int color)
 *  \param draw_image  Function to draw image. void (*draw_image)(struct boxing_painter_s *painter, int x, int y, const char * image, int scan_line_width, int sx, int sy, int sw, int sh)
 *  \param width       Function to get image width. int (*width)(struct boxing_painter_s * painter)
 *  \param height      Function to get image height. int (*height)(struct boxing_painter_s * painter)
 *  \param region      Region.
 *  \param device      Paint device.
 *
 *  Painter data storage structure description.
 */


// PUBLIC PAINTER FUNCTIONS
//

//----------------------------------------------------------------------------
/*!
 *  \brief Create a painter instance.
 *
 *  Allocate memory for the boxing_painter type.
 *  Return instance of allocated structure.
 *
 *  \return instance of allocated boxing_painter structure.
 */

boxing_painter * boxing_painter_create(void)
{
    boxing_painter *painter = malloc(sizeof(boxing_painter));
    return painter;
}


//----------------------------------------------------------------------------
/*!
 *  \brief Frees occupied memory of boxing_painter structure.
 *
 *  Frees occupied memory of boxing_painter structure.
 *
 *  \param[in]  painter  Pointer to the boxing_painter structure.
 */

void boxing_painter_free(boxing_painter * painter)
{
    free(painter);
}


//----------------------------------------------------------------------------
/*!
 *  \brief Initializes all boxing_painter structure data.
 *
 *  Initializes all boxing_painter structure data according to the input paint device data.
 *
 *  \param[in] painter  Pointer to the boxing_painter structure.
 *  \param[in] device   Pointer to the boxing_paintdevice structure.
 */

void boxing_painter_init(boxing_painter * painter, boxing_paintdevice * device)
{
    painter->region.x = 0;
    painter->region.y = 0;
    painter->region.width = device->width(device);
    painter->region.height = device->height(device);
    painter->device = device;
    set_callbacks(painter);
}


//----------------------------------------------------------------------------
/*!
 *  \brief Copy rectangle region.
 *
 *  Copy rectangle region from source painter to the destination painter.
 *
 *  \param[in] copy_to    Pointer to the destination painter.
 *  \param[in] copy_from  Pointer to the source painter.
 *  \param[in] region     Pointer to the region.
 */

void boxing_painter_clip(boxing_painter * copy_to, const boxing_painter * copy_from, const boxing_recti * region)
{
    boxing_recti tmp = *region;
    copy_to->region = copy_from->region;
    tmp.x += copy_to->region.x;
    tmp.y += copy_to->region.y;
    boxing_math_recti_intersect(&copy_to->region, &tmp);
    copy_to->device = copy_from->device;

    set_callbacks(copy_to);
}


//----------------------------------------------------------------------------
/*!
  * \} end of graphics group
  */


// PRIVATE PAINTER FUNCTIONS
//

static void draw_line(boxing_painter *painter, int x1, int y1, int x2, int y2, const int color)
{
    painter->device->draw_line(painter->device, &painter->region, x1+painter->region.x, y1+painter->region.y, x2+painter->region.x, y2+painter->region.y, color);
}

static void draw_rect(boxing_painter *painter, int x, int y, int width, int height, const int color)
{
    painter->device->draw_rect(painter->device, &painter->region, x+painter->region.x, y+painter->region.y, width, height, color);
}

static void fill_rect(boxing_painter *painter, int x, int y, int width, int height, const int color)
{
    painter->device->fill_rect(painter->device, &painter->region, x+painter->region.x, y+painter->region.y, width, height, color);
}

static void draw_image (boxing_painter *painter,  int x, int y, const char * image, int scanline_width, int sx, int sy, int sw, int sh)
{
    painter->device->draw_image(painter->device, &painter->region, x+painter->region.x, y+painter->region.y, image, scanline_width, sx, sy, sw, sh);
}

static int width(boxing_painter *painter)
{
    return painter->region.width;
}

static int height(boxing_painter *painter)
{
    return painter->region.height;
}

static void set_callbacks(boxing_painter *painter)
{
    painter->draw_line = draw_line;
    painter->draw_rect = draw_rect;
    painter->fill_rect = fill_rect;
    painter->draw_image = draw_image;
    painter->width = width;
    painter->height = height;
}
