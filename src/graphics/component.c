/*****************************************************************************
**
**  Implementation of the component interface
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
#include    "boxing/graphics/component.h"


/*! 
  * \addtogroup graphics
  * \{
  */


//----------------------------------------------------------------------------
/*!
 *  \enum       boxing_align component.h
 *  \brief      The enumeration for specifying the align type.
 *  
 *  \param BOXING_ALIGN_HORIZONTAL  (0) Align horizontal.
 *  \param BOXING_ALIGN_VERTICAL    (1) Align vertical.
 *
 *  The enumeration for specifying the align type.
 */


//----------------------------------------------------------------------------
/*!
 *  \struct     boxing_component_s  component.h
 *  \brief      Structure for storing boxing component data.
 *  
 *  \param add               Pointer to a function to add a new boxing_component_s child pointer. (void (*add)(struct boxing_component_s *component, struct boxing_component_s * child))
 *  \param render            Pointer to a function to render component (void (*render)(struct boxing_component_s *component, boxing_painter * painter)).
 *  \param set_size          Pointer to a function to set width and height values. (void (*set_size)(struct boxing_component_s *component, int width, int height))
 *  \param free              Pointer to a function to free memory, not frees the pointer passed. (void (*free)(struct boxing_component_s *component))
 *  \param children          Pointers array of boxing_component_* children instances.
 *  \param pos               Poisition.
 *  \param size              Size.
 *  \param background_color  Backgroung color.
 *  \param foreground_color  Foregroung color.
 *  \param parent            Parent boxing_component_s structure pointer.
 *  \param peer              Peer boxing_component_s structure pointer.
 *
 *  Structure for storing boxing component data.
 */


// PUBLIC COMPONENT FUNCTIONS
//

//----------------------------------------------------------------------------
/*!
 *  \brief Initializes boxing_component structure.
 *
 *  This function initialize parent boxing_component pointer by the given pointer (Parent is null by default),
 *  and initialize other data by default values.
 *
 *  \param[in]  component  Pointer to the boxing_component structure. 
 *  \param[in]  parent     Pointer to the parent boxing_component structure.
 */

void boxing_component_init(boxing_component * component, boxing_component * parent)
{
    component->parent = parent;
    component->peer = NULL;
    component->background_color = 0;
    component->foreground_color = 1;
    component->size.x = component->size.y = 0;
    component->pos.x = component->pos.y = 0;
    component->add = boxing_component_add;
    component->render = boxing_component_render;
    component->free = boxing_component_free;
    component->set_size = boxing_component_set_size;

    gvector_create_inplace(&component->children, sizeof(boxing_component *), 0);
}


//----------------------------------------------------------------------------
/*!
 *  \brief The function gets absolute location of the given boxing_component.
 *
 *  The function gets absolute location of the given boxing_component.
 *
 *  \param[in]  component  Pointer to the boxing_component structure.
 *  \return Absolute location of the given boxing_component.
 */

boxing_pointi boxing_component_absolute_location(const boxing_component * component)
{
    boxing_pointi return_value;

    if(component->peer)
    {
        return_value = boxing_component_absolute_location(component->peer);
        return_value.x += component->pos.x;
        return_value.y += component->pos.y;
        return return_value;
    }
    else
    {
        return component->pos;
    }
}


//----------------------------------------------------------------------------
/*!
 *  \brief Frees occupied memory of children boxing_component pointers array.
 *
 *  Frees occupied memory of children boxing_component pointers array.
 *  Does not free component pointer itself.
 *
 *  \param[in]  component  Pointer to the boxing_component structure.
 */

void boxing_component_free(boxing_component * component)
{
    free(component->children.buffer);
}


//----------------------------------------------------------------------------
/*!
 *  \brief The function gets background color from the given boxing_component instance.
 *
 *  The function gets background color from the given boxing_component instance.
 *
 *  \param[in]  component  Pointer to the boxing_component structure.
 *  \return Background color.
 */

int boxing_component_get_background_color(const struct boxing_component_s *component)
{
    while (component->parent)
    {
        component = component->parent;
    }
    return component->background_color;
}


//----------------------------------------------------------------------------
/*!
 *  \brief The function gets foreground color from the given boxing_component instance.
 *
 *  The function gets foreground color from the given boxing_component instance.
 *
 *  \param[in]  component  Pointer to the boxing_component structure.
 *  \return Foreground color.
 */

int boxing_component_get_foreground_color(const struct boxing_component_s *component)
{
    while (component->parent)
    {
        component = component->parent;
    }
    return component->foreground_color;
}


//----------------------------------------------------------------------------
/*!
 *  \brief The function sets width and height in the given boxing_component instance.
 *
 *  The function sets width and height in the given boxing_component instance.
 *
 *  \param[in]  component  Pointer to the boxing_component structure.
 *  \param[in]  width      Width.
 *  \param[in]  height     Height.
 */

void boxing_component_set_size(struct boxing_component_s *component, int width, int height)
{
    component->size.x = width;
    component->size.y = height;
}


//----------------------------------------------------------------------------
/*!
 *  \brief The function render the given boxing_component instance.
 *
 *  The function render the given boxing_component instance.
 *
 *  \param[in]  component  Pointer to the boxing_component structure.
 *  \param[in]  painter    Pointer to the boxing_painter structure.
 */

void boxing_component_render(struct boxing_component_s *component, boxing_painter * painter)
{
    boxing_component ** it;
    boxing_component ** it_end = (((boxing_component**)(component->children.buffer)) + component->children.size);
    for (it = component->children.buffer; it != it_end; it++)
    {
        boxing_component * child = *it;
        boxing_painter child_painter;
        boxing_recti recti = { child->pos.x, child->pos.y, child->size.x, child->size.y };
        boxing_painter_clip(&child_painter, painter, &recti);
        child->render(child, &child_painter);
    }
}


//----------------------------------------------------------------------------
/*!
 *  \brief The function adds the new child boxing_component_s pointer to the boxing_component_s structure.
 *
 *  The function adds the new child boxing_component_s pointer to the boxing_component_s structure.
 *
 *  \param[in]  component  Pointer to the boxing_component structure.
 *  \param[in]  child      Pointer to the child boxing_component structure.
 */

void boxing_component_add(struct boxing_component_s *component, struct boxing_component_s * child)
{
    child->peer = component;
    gvector_append_data(&component->children, 1, &child);
}


//----------------------------------------------------------------------------
/*!
  * \} end of graphics group
  */
