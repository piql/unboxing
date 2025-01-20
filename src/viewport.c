/*****************************************************************************
**
**  Implementation of the viewport interface
**
**  Creation date:  2018/11/20
**  Created by:     Piql
**
**
**  Copyright (c) 2018 Piql AS. All rights reserved.
**
**  This file is part of the boxing library
**
*****************************************************************************/

//  PROJECT INCLUDES
//
#include "boxing/viewport.h"
#include "boxing/platform/memory.h"
#include "boxing/math/math.h"


typedef struct viewport_buffer_s
{
    unsigned int width;
    unsigned int height;
    unsigned int element_size;
    unsigned int scanline_size;
    void *data;
} viewport_buffer;

struct boxing_viewport_s
{
    unsigned int width;
    unsigned int height;
    unsigned int x_offset;
    unsigned int y_offset;
    viewport_buffer buf;
};

//----------------------------------------------------------------------------
/*!
 *  \defgroup   viewport  ViewPort
 *  \brief      Two dimensional view for accessing any type of two dimensional data.
 *  \ingroup    unbox
 *
 *  The viewport is a tool for viewing any arbitrary sections of data
 *  within a two dimensional buffer. The buffer it self is organized as 1 dimensional array
 *  of elements listed in linear memory space.
 *
 */


/*! 
  * \addtogroup viewport
  * \{
  */

//----------------------------------------------------------------------------
/*!
 *  \struct     boxing_viewport_s  viewport.h
 *  \brief      An object that contains the viewports internal structure.
 *
 */

// PUBLIC VIEWPORT FUNCTIONS
//

//----------------------------------------------------------------------------
/*!
 *  \brief Create a new viewport object.
 *
 *  Create a viewport object for a pre allocated buffer. The buffer must be at least
 *  width*height*element_size bytes long 
 * 
 *  If width, height, scanline_size or element_size is equal to zero, then no
 *  viewport is created and the function returns NULL.
 *  If memory allocation fails, the function returns NULL.
 *
 *  \param[in]  buffer          A pointer to a one dimensional array of elements.
 *  \param[in]  width           Width of the buffer.
 *  \param[in]  height          Height of the buffer.
 *  \param[in]  scanline_size   Number of elements per scanline.
 *  \param[in]  element_size    Size of an element.
 *  \return created image with specified sizes.
 */

boxing_viewport * boxing_viewport_create(void *buffer, int width, int height, int scanline_size, int element_size)
{
    if(!buffer || (width < 1) || (height < 1) || (scanline_size < 1) || (element_size < 1))
    {
        return NULL;
    }

    boxing_viewport * vp = boxing_memory_allocate( sizeof(boxing_viewport) );
    if(vp)
    {
        vp->x_offset = 0;
        vp->y_offset = 0;
        vp->width = width;
        vp->height = height;
        vp->buf.data = buffer;
        vp->buf.width = width;
        vp->buf.height = height;
        vp->buf.element_size = element_size;
        vp->buf.scanline_size = scanline_size;
    }
    return vp;
}


//----------------------------------------------------------------------------
/*!
 *  \brief Create a copy of an existing viewport, but referencing the same data buffer.
 *
 *  \param[in]  buffer          A pointer to an existing viewport.
 *  \return a view port copy or NULL in case of errors.
 */

boxing_viewport * boxing_viewport_clone(const boxing_viewport *vp)
{
    boxing_viewport * new_vp = boxing_memory_allocate( sizeof(boxing_viewport) );
    if(new_vp)
    {
        boxing_memory_copy(new_vp, vp, sizeof(boxing_viewport));
    }

    return new_vp;
}


//----------------------------------------------------------------------------
/*!
 *  \brief Resores the viewport to default view.
 *
 *  This function restores the viewport settings to default, that is full width
 *  and height and zero offset.
 *
 *  \param[in]  buffer          A pointer to an existing viewport.
 */

void boxing_viewport_reset(boxing_viewport *vp)
{
    vp->width = vp->buf.width;
    vp->height = vp->buf.height;
    vp->x_offset = 0;
    vp->y_offset = 0;
}


//----------------------------------------------------------------------------
/*!
 *  \brief returns a pointer to the first element for a given scanline
 *  within the current view
 *
 *  \param[in]  vp          A pointer to a viewport.
 *  \param[in]  width       Width of view.
 *  \param[in]  height      Height of view.
 *  \param[in]  x_offset    Horizontal offset of the new view relative to the current view.
 *  \param[in]  y_offset    Vertical offset of the new view relative to the current view.
 */

void boxing_viewport_set_view(boxing_viewport *vp, int width, int height, int x_offset, int y_offset)
{

    if (width < 0)
    {
        width = vp->buf.width - vp->x_offset;
    }

    if (height < 0)
    {
        height = vp->buf.height - vp->y_offset;
    }

    vp->x_offset = BOXING_MATH_CLAMP(0, (int)vp->buf.width,  (int)vp->x_offset + x_offset);
    vp->y_offset = BOXING_MATH_CLAMP(0, (int)vp->buf.height, (int)vp->y_offset + y_offset);

    vp->width    = BOXING_MATH_CLAMP(0, (int)vp->buf.width  - (int)vp->x_offset, width);
    vp->height   = BOXING_MATH_CLAMP(0, (int)vp->buf.height - (int)vp->y_offset, height);
}


//----------------------------------------------------------------------------
/*!
 *  \brief returns a pointer to the first element for a given scanline
 *  within the current view
 *
 *  \param[in]  vp          A pointer to a viewport.
 *  \return pointer to the first element.
 */

void * boxing_viewport_scanline(const boxing_viewport *vp, int height)
{
    return ((char *)vp->buf.data) + (vp->buf.element_size * (vp->buf.scanline_size * (height + vp->y_offset) + vp->x_offset) );
}


//----------------------------------------------------------------------------
/*!
 *  \brief returns the size of an scanline.
 *
 *  \param[in]  vp          A pointer to a viewport.
 *  \return scanline size.
 */

int boxing_viewport_scanline_size(const boxing_viewport *vp)
{
    return vp->buf.scanline_size;
}


//----------------------------------------------------------------------------
/*!
 *  \brief returns the size of an element.
 *
 *  \param[in]  vp          A pointer to a viewport.
 *  \return element size.
 */

int boxing_viewport_element_size(const boxing_viewport *vp)
{
    return vp->buf.element_size;
}


//----------------------------------------------------------------------------
/*!
 *  \brief Free all resourses the viewport has allocated.
 *
 *  \param[in]  vp          A pointer to a viewport.
 */

void boxing_viewport_free(boxing_viewport *vp)
{
    boxing_memory_free(vp);
}


//----------------------------------------------------------------------------
/*!
 *  \brief returns the width of the current view.
 *
 *  \param[in]  vp          A pointer to a viewport.
 *  \return width of the current view.
 */

int boxing_viewport_width(const boxing_viewport *vp)
{
    return vp->width;
}


//----------------------------------------------------------------------------
/*!
 *  \brief returns the height of the current view.
 *
 *  \param[in]  vp          A pointer to a viewport.
 *  \return height of the current view.
 */

int boxing_viewport_height(const boxing_viewport *vp)
{
    return vp->height; 
}


//----------------------------------------------------------------------------
/*!
 *  \brief returns the horisontal offset of the current view.
 *
 *  \param[in]  vp          A pointer to a viewport.
 *  \return horizontal offset.
 */

int boxing_viewport_offset_x(const boxing_viewport *vp)
{
    return vp->x_offset;
}


//----------------------------------------------------------------------------
/*!
 *  \brief returns the vertical offset of the current view.
 *
 *  \param[in]  vp          A pointer to a viewport.
 *  \return vertical offset.
 */

int boxing_viewport_offset_y(const boxing_viewport *vp)
{
    return vp->y_offset;
}


//----------------------------------------------------------------------------
/*!
 *  \brief returns the buffer width.
 *
 *  \param[in]  vp          A pointer to a viewport.
 *  \return buffer width.
 */

int boxing_viewport_buffer_width(const boxing_viewport *vp)
{
    return vp->buf.width;
}


//----------------------------------------------------------------------------
/*!
 *  \brief returns the buffer height.
 *
 *  \param[in]  vp          A pointer to a viewport.
 *  \return buffer width.
 */

int boxing_viewport_buffer_height(const boxing_viewport *vp)
{
    return vp->buf.height;
}

//----------------------------------------------------------------------------
/*!
  * \} end of viewport group
  */
