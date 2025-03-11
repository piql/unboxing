/*****************************************************************************
**
**  Implementation of the border interface
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
#include    "boxing/graphics/border.h"
#include    "boxing/utils.h"

//  DEFINES
//

#define SMEMBER(member) (((boxing_border*)border)->member)

//  PRIVATE INTERFACE
//

static void set_size(boxing_component *border, int width, int height);
static void add(boxing_component *border, boxing_component * child);
static void render(boxing_component *border, boxing_painter * painter);


/*! 
  * \addtogroup graphics
  * \{
  */


//----------------------------------------------------------------------------
/*!
 *  \struct  boxing_border_s  border.h
 *  \brief   Structure for storing border data.
 *
 *  \param base         boxing_component structure instance.
 *  \param container    Pointer to the boxing_component structure.
 *  \param border_size  Size of the border.
 *  \param gap_size     Size of the gap.
 *
 *  Structure for storing border data.
 */


// PUBLIC BORDER FUNCTIONS
//

//----------------------------------------------------------------------------
/*!
 *  \brief Initializes structure variables with specified input values.
 *
 *  Initializes structure variables with specified input value,
 *  initialize boxing container and sets function pointers.
 *
 *  \param[in] border       Pointer to the boxing_border structure.
 *  \param[in] border_size  Size of the border.
 *  \param[in] gap_size     Size of the gap.
 */

void boxing_border_init(boxing_border * border, int border_size, int gap_size)
{
    boxing_component_init((boxing_component*)border, NULL);
    border->border_size = border_size;
    border->gap_size = gap_size;
    border->container = malloc(sizeof(boxing_component));
    boxing_component_init((boxing_component*)border->container, NULL);

    border->container->pos.x = border->border_size+border->gap_size;
    border->container->pos.y = border->border_size+border->gap_size;

    boxing_component_add((boxing_component*)border, border->container);

    border->base.add = add;
    border->base.free = boxing_border_free;
    border->base.render = render;
    border->base.set_size = set_size;
}


//----------------------------------------------------------------------------
/*!
 *  \brief Frees occupied memory of boxing_component structure and all internal components.
 *
 *  Frees occupied memory of boxing_component structure and all internal components.
 *
 *  \param[in]  border  Pointer to the boxing_component structure.
 */

void boxing_border_free(boxing_component * border)
{
    boxing_component_free(SMEMBER(container));
    boxing_component_free(border);
    free(SMEMBER(container));
}


//----------------------------------------------------------------------------
/*!
  * \} end of graphics group
  */


// PRIVATE BORDER FUNCTIONS
//

static void set_size(boxing_component *border, int width, int height) 
{ 
    boxing_component_set_size(border, width, height);

    width -= 2*(SMEMBER(border_size)+SMEMBER(gap_size));
    if(width < 0)
        width = 0;

    height -= 2*(SMEMBER(border_size)+SMEMBER(gap_size));
    if(height < 0)
        height = 0;

    ((boxing_border*)border)->container->set_size(((boxing_border*)border)->container, width, height);
}

static void add(boxing_component *border, boxing_component * child)
{
    ((boxing_border*)border)->container->add(((boxing_border*)border)->container, child);
}

static void render(boxing_component *border, boxing_painter * painter)
{
    // reset the whole area
    if(SMEMBER(gap_size))
        for(int i = SMEMBER(border_size); i < SMEMBER(gap_size) + SMEMBER(border_size); i++)
            painter->draw_rect(painter, i, i, border->size.x-(i*2), border->size.y-(i*2), boxing_component_get_background_color(border));

    if(SMEMBER(border_size))
        for(int i = 0; i < SMEMBER(border_size); i++)
            painter->draw_rect(painter, i, i, border->size.x-(i*2), border->size.y-(i*2), boxing_component_get_foreground_color(border));

    boxing_component_render(border, painter);
}
