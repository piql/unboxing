/*****************************************************************************
**
**  Implementation of the generic frame interface
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
#include "boxing/graphics/genericframe.h"
#include "boxing/utils.h"
#include "boxing/string.h"
#include "boxing/platform/platform.h"

//  PRIVATE INTERFACE
//

static boxing_pointi get_tile_size(struct boxing_generic_container_s * container);
static boxing_pointi get_tile_size_enum(struct boxing_generic_container_s * container, enum boxing_content_type type);


/*! 
  * \addtogroup graphics
  * \{
  */


//----------------------------------------------------------------------------
/*!
 *  \enum       boxing_content_type genericframe.h
 *  \brief      The enumeration for specifying the content type.
 *  
 *  \param BOXING_CONTENT_TYPE_DIGITAL  (0) Digital content type.
 *  \param BOXING_CONTENT_TYPE_ANALOG   (1) Analog content type.
 *
 *  The enumeration for specifying the content type.
 */


//----------------------------------------------------------------------------
/*!
 *  \struct     boxing_generic_container_s  genericframe.h
 *  \brief      Generic container data storage.
 *  
 *  \param capasity            Function to get capacity value. (int (*capasity)(struct boxing_generic_container_s * container))
 *  \param dimension           Function to get dimension values. (boxing_pointi (*dimension)(struct boxing_generic_container_s * container))
 *  \param location            Function to get location point. (boxing_pointi (*location)(struct boxing_generic_container_s * container))
 *  \param set_data            Function to set data. (void (*set_data)(struct boxing_generic_container_s * container, const char * data, int size))
 *  \param set_tile_size       Function to set tile size. (void (*set_tile_size)(struct boxing_generic_container_s * container, const boxing_pointi size))
 *  \param get_tile_size       Function to get tile size. (boxing_pointi (*get_tile_size)(struct boxing_generic_container_s * container))
 *  \param get_tile_size_enum  Function to get tile size with the given type. (boxing_pointi (*get_tile_size_enum)(struct boxing_generic_container_s * container, enum boxing_content_type))
 *  \param tile_size           Tile size.
 *
 *  Generic container data storage structure description.
 */


//----------------------------------------------------------------------------
/*!
 *  \struct     boxing_frame_s  genericframe.h
 *  \brief      Frame data storage structure.
 *  
 *  \param free                       Function to free memory. Does not free the actual pointer. (void (*free)(struct boxing_frame_s * frame))
 *  \param render                     Function to render frame. (void (*render)(struct boxing_frame_s * frame, boxing_painter * painter))
 *  \param size                       Function to get frame size. (*size)(const struct boxing_frame_s * frame)
 *  \param container                  Function to frame conteiner. (boxing_generic_container  *(*container)(struct boxing_frame_s * frame))
 *  \param metadata_container         Function to get metadata conteiner. (boxing_generic_container *(*metadata_container)(struct boxing_frame_s * frame))
 *  \param max_levels_per_symbol      Function to get max level per symbol. (int (*max_levels_per_symbol)(struct boxing_frame_s * frame))
 *  \param levels_per_symbol          Function to get levels per symbol. (int (*levels_per_symbol)(struct boxing_frame_s * frame))
 *  \param set_levels_per_symbol      Function to set level per symbol. (void (*set_levels_per_symbol)(struct boxing_frame_s * frame, int levels, DBOOL force))
 *  \param set_system_human_readable  Function to set system human readable string. (void (*set_system_human_readable)(struct boxing_frame_s * frame, const char * str))
 *  \param set_user_human_readable    Function to set user human readable string. (void (*set_user_human_readable)(struct boxing_frame_s * frame, const char * str))
 *  \param create_frame_tracker       Function to create frame tracker. (boxing_tracker_s *(*create_frame_tracker)(struct boxing_frame_s * frame))
 *  \param format                     Format.
 *  \param name                       Name.
 *  \param short_description          Short description.
 *  \param description                Description.
 *
 *  Frame data storage structure description.
 */


// PUBLIC GENERIC FRAME FUNCTIONS
//

//----------------------------------------------------------------------------
/*!
 *  \brief Initializes all structure pointers and variables with defult values.
 *
 *  Initializes all input structure pointers and variables with default values.
 *
 *  \param[in] container  Pointer to the boxing_generic_container structure.
 */

void boxing_generic_frame_container_init(boxing_generic_container * container)
{
    container->tile_size.x = container->tile_size.y = 1;
    container->set_tile_size = boxing_generic_frame_container_set_tile_size;
    container->get_tile_size_enum = get_tile_size_enum;
    container->get_tile_size = get_tile_size;
    container->capasity = NULL;
    container->dimension = NULL;
    container->location = NULL;
    container->set_data = NULL;
}


//----------------------------------------------------------------------------
/*!
 *  \brief Set new tile size.
 *
 *  Set new tile size in the input boxing_generic_container structure.
 *
 *  \param[in] container  Pointer to the boxing_generic_container structure.
 *  \param[in] size       New tile size.
 */

void boxing_generic_frame_container_set_tile_size(boxing_generic_container * container, const boxing_pointi size)
{
    container->tile_size = size;
}

// generic frame stuff


//----------------------------------------------------------------------------
/*!
 *  \brief Initialize generic frame instance.
 *
 *  Initialize generic frame instance with default values.
 *
 *  \param[in] frame  Pointer to the boxing_frame_s structure.
 */

void boxing_generic_frame_init(struct boxing_frame_s * frame)
{
    memset(frame, 0, sizeof(boxing_frame));
    frame->free = boxing_generic_frame_free;
}


//----------------------------------------------------------------------------
/*!
 *  \brief Frees occupied memory of boxing_frame_s structure.
 *
 *  Frees occupied memory of all internal structure pointers. Does not free the actual pointer.
 *
 *  \ingroup toc
 *  \param[in]  frame  Pointer to the boxing_frame_s structure.
 */

void boxing_generic_frame_free(struct boxing_frame_s * frame)
{
    free(frame->format);
    free(frame->name);
    free(frame->short_description);
    free(frame->description);
}


//----------------------------------------------------------------------------
/*!
 *  \brief Get the frame format.
 *
 *  Get the frame format from the boxing_frame instance.
 *
 *  \param[in]  frame  Pointer to the boxing_frame structure.
 *  \return Frame format.
 */

const char * boxing_generic_frame_get_format(const boxing_frame * frame)
{
    return frame->format;
}


//----------------------------------------------------------------------------
/*!
 *  \brief Get the frame name.
 *
 *  Get the frame name from the boxing_frame instance.
 *
 *  \param[in]  frame  Pointer to the boxing_frame structure.
 *  \return Frame name.
 */

const char * boxing_generic_frame_get_name(const struct boxing_frame_s * frame)
{
    return frame->name;
}


//----------------------------------------------------------------------------
/*!
 *  \brief Get the frame short description.
 *
 *  Get the frame short description from the boxing_frame instance.
 *
 *  \param[in]  frame  Pointer to the boxing_frame structure.
 *  \return Frame short description.
 */

const char * boxing_generic_frame_get_short_description(const struct boxing_frame_s * frame)
{
    return frame->short_description;
}


//----------------------------------------------------------------------------
/*!
 *  \brief Get the frame description.
 *
 *  Get the frame description from the boxing_frame instance.
 *
 *  \param[in]  frame  Pointer to the boxing_frame structure.
 *  \return Frame description.
 */

const char * boxing_generic_frame_get_description(const struct boxing_frame_s * frame)
{
    return frame->description;
}


//----------------------------------------------------------------------------
/*!
 *  \brief Set the frame format.
 *
 *  Set the new value of the frame format.
 *
 *  \param[in]  frame   Pointer to the boxing_frame structure.
 *  \param[in]  format  New frame format string.
 */

void  boxing_generic_frame_set_format(boxing_frame * frame, const char * format)
{
    free(frame->format);
    frame->format = boxing_string_clone(format);
}


//----------------------------------------------------------------------------
/*!
 *  \brief Set the frame name.
 *
 *  Set the new value of the frame name.
 *
 *  \param[in]  frame  Pointer to the boxing_frame structure.
 *  \param[in]  name   New frame name string.
 */

void boxing_generic_frame_set_name(struct boxing_frame_s * frame, const char * name)
{
    free(frame->name);
    frame->name = boxing_string_clone(name);
}


//----------------------------------------------------------------------------
/*!
 *  \brief Set the frame short description.
 *
 *  Set the new value of the frame short description.
 *
 *  \param[in]  frame  Pointer to the boxing_frame structure.
 *  \param[in]  name   New frame short description string.
 */

void boxing_generic_frame_set_short_description(struct boxing_frame_s * frame, const char * name)
{
    free(frame->short_description);
    frame->short_description = boxing_string_clone(name);
}


//----------------------------------------------------------------------------
/*!
 *  \brief Set the frame description.
 *
 *  Set the new value of the frame description.
 *
 *  \param[in]  frame  Pointer to the boxing_frame structure.
 *  \param[in]  name   New frame description string.
 */

void boxing_generic_frame_set_description(struct boxing_frame_s * frame, const char * name)
{
    free(frame->description);
    frame->description = boxing_string_clone(name);
}


//----------------------------------------------------------------------------
/*!
  * \} end of graphics group
  */


// PRIVATE GENERIC FRAME FUNCTIONS
//

static boxing_pointi get_tile_size(struct boxing_generic_container_s * container)
{
    return container->tile_size;
}

static boxing_pointi get_tile_size_enum(struct boxing_generic_container_s * container, enum boxing_content_type type)
{
    BOXING_UNUSED_PARAMETER(type);
    return container->tile_size;
}
