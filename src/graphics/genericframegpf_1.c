/*****************************************************************************
**
**  Implementation of the dgenericframegpf_1 interface
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
#include    "boxing/graphics/genericframegpf_1.h"
#include    "boxing/graphics/referencepoint.h"
#include    "boxing/graphics/referencebar.h"
#include    "boxing/graphics/calibrationbar.h"
#include    "boxing/graphics/contentcontainer.h"
#include    "boxing/graphics/metadatabar.h"
#include    "boxing/graphics/label.h"
#include    "boxing/frame/trackergpf_1.h"

//  BASE INCLUDES
//
#include    "boxing/log.h"

//  DEFINES
//

#define SMEMBER(member) (((boxing_frame_gpf_1*)frame)->member)

//  PRIVATE INTERFACE
//

static void          add(boxing_frame_gpf_1 * frame, boxing_component * child);
static void          add_components(boxing_frame_gpf_1 * frame);
static boxing_pointi size(const boxing_frame * frame);
static int           frame_max_levels_per_symbol(boxing_frame * frame);
static int           levels_per_symbol(boxing_frame * frame);
static void          set_levels_per_symbol(boxing_frame * frame, int levels, DBOOL force /*  = false */);
static void          set_system_human_readable(boxing_frame * frame, const char * str);
static void          set_user_human_readable(boxing_frame * frame, const char * str);
static void          render(boxing_frame * frame, boxing_painter * painter);

static int           impl_capasity(boxing_generic_container * container);
static boxing_pointi impl_dimension(boxing_generic_container * container);
static boxing_pointi impl_location(boxing_generic_container * container);
static boxing_pointi impl_tile_size(boxing_generic_container * container, enum boxing_content_type type);
static void          impl_set_data(boxing_generic_container * container, const char * data, int size);
static void          impl_set_tile_size(boxing_generic_container * container, const boxing_pointi size);

static boxing_pointi imeta_tile_size(boxing_generic_container * container);
static void          imeta_set_data(boxing_generic_container * container, const char * data, int size);
static boxing_pointi imeta_location(boxing_generic_container * container);
static boxing_pointi imeta_dimension(boxing_generic_container * container);
static int           imeta_capasity(boxing_generic_container * container);

static boxing_generic_container *  frame_gpf_1_container(boxing_frame * frame);
static boxing_generic_container *  frame_gpf_1_metadata_container(boxing_frame * frame);
static struct boxing_tracker_s *   frame_gpf_1_create_tracker(boxing_frame * frame);

static void frame_gpf_1_free(boxing_frame * frame);


/*! 
  * \addtogroup graphics
  * \{
  */


//----------------------------------------------------------------------------
/*!
 *  \struct     boxing_container_impl_1_s  genericframegpf_1.h
 *  \brief      Generic container.
 *  
 *  \param base   Base boxing_generic_container structure instance.
 *  \param frame  Pointer to the boxing_frame_gpf_1_s structure instance.
 *
 *  Generic container structure description.
 */


//----------------------------------------------------------------------------
/*!
 *  \struct     boxing_container_imeta_1_s  genericframegpf_1.h
 *  \brief      Generic metadata container.
 *  
 *  \param base   Base boxing_generic_container structure instance.
 *  \param frame  Pointer to the boxing_frame_gpf_1_s structure instance.
 *
 *  Generic metadata container structure description.
 */


//----------------------------------------------------------------------------
/*!
 *  \struct     boxing_frame_gpf_1_s  genericframegpf_1.h
 *  \brief      Boxing frame gpf 1.
 *  
 *  \param base                          Base boxing_frame structure instance.
 *  \param base_border                   Base boxing_border structure instance.
 *  \param top_left_reference_point      Top left reference point.
 *  \param top_right_reference_point     Top right reference point.
 *  \param bottom_left_reference_point   Bottom left reference point.
 *  \param bottom_right_reference_point  Bottom right reference point.
 *  \param top_reference_bar             Top reference bar.
 *  \param bottom_reference_bar          Bottom reference bar.
 *  \param left_reference_bar            Left reference bar.
 *  \param right_reference_bar           Right reference bar.
 *  \param calibration_bar               Calibration bar.
 *  \param content_container             Content container.
 *  \param metadata_bar                  Metadata bar.
 *  \param label_system_define           Label system define.
 *  \param label_user_define             Label user define.
 *  \param max_levels_per_symbol         Max levels per symbol.
 *  \param levels_per_symbol             Levels per symbol.
 *  \param corner_mark_size              Corner mark size.
 *  \param corner_mark_gap               Corner mark gap.
 *  \param tiles_per_column              Tiles per column.
 *  \param generic_container             Generic container.
 *  \param generic_metadata_container    Generic metadata container.
 *  \param sync_point_h_distance         Sync point horizontal distance.
 *  \param sync_point_v_distance         Sync point vertical distance
 *  \param sync_point_h_offset           Sync point horizontal offset.
 *  \param sync_point_v_offset           Sync point vertical offset.
 *  \param sync_point_radius             Sync point radius.
 *  \param reference_bar_freq_divider    Reference bar frequency divider.
 *  \param analog_content_symbol_size    Analog content symbol syze.
 *  \param digital_content_symbol_size   Digital content symbol syze.
 *  \param reference_bar_sync_distance   Reference bar sync distance.
 *  \param reference_bar_sync_offset     Reference bar sync offset.
 *
 *  Boxing frame gpf 1 structure description.
 */


// PUBLIC GENERIC FRAME GPF 1 FUNCTIONS
//

//----------------------------------------------------------------------------
/*!
 *  \brief Initializes boxing_container_impl_1 structure.
 *
 *  Initializes frame pointer according to the input value and
 *  initializes all other structure pointers and variables with default values.
 *
 *  \param[in] container  Pointer to the boxing_container_impl_1 structure.
 *  \param[in] frame      Pointer to the boxing_frame_gpf_1 structure.
 */

void boxing_container_impl_1_init(boxing_container_impl_1 * container, boxing_frame_gpf_1 * frame)
{
    boxing_generic_frame_container_init(&container->base);
    container->base.capasity = impl_capasity;
    container->base.dimension = impl_dimension;
    container->base.location = impl_location;
    container->base.get_tile_size_enum = impl_tile_size;
    container->base.set_data = impl_set_data;
    container->base.set_tile_size = impl_set_tile_size;
    container->frame = frame;
}


//----------------------------------------------------------------------------
/*!
 *  \brief Initializes boxing_container_imeta_1 structure.
 *
 *  Initializes frame pointer according to the input value and
 *  initializes all other structure pointers and variables with default values.
 *
 *  \param[in] container  Pointer to the boxing_container_imeta_1 structure.
 *  \param[in] frame      Pointer to the boxing_frame_gpf_1 structure.
 */

void boxing_container_imeta_1_init(boxing_container_imeta_1 * container, boxing_frame_gpf_1 * frame)
{
    boxing_generic_frame_container_init(&container->base);
    container->frame = frame;
    container->base.get_tile_size = imeta_tile_size;
    container->base.set_data = imeta_set_data;
    container->base.location = imeta_location;
    container->base.dimension = imeta_dimension;
    container->base.capasity = imeta_capasity;
}


//----------------------------------------------------------------------------
/*!
 *  \brief Initializes boxing_frame_gpf_1 structure.
 *
 *  Initializes boxing_frame_gpf_1 structure data according to the input data.
 *
 *  \param[in] frame                        Pointer to the boxing_frame_gpf_1 structure.
 *  \param[in] width                        Frame width.
 *  \param[in] height                       Frame height.
 *  \param[in] border                       Border size.
 *  \param[in] border_gap                   Border gap.
 *  \param[in] corner_mark_size             Corner mark size.
 *  \param[in] corner_mark_gap              Corner mark gap.
 *  \param[in] tiles_per_column             Tiles per column.
 *  \param[in] max_levels_per_symbol        Max level per symbol.
 *  \param[in] reference_bar_freq_divider   Reference bar frequency divider.
 *  \param[in] analog_content_symbol_size   Analog content symbol size.
 *  \param[in] digital_content_symbol_size  Digital content symbol size.
 *  \param[in] reference_bar_sync_distance  Reference bar sync distance.
 *  \param[in] reference_bar_sync_offset    Reference bar sync offset.
 */

void boxing_generic_frame_gpf_1_init(boxing_frame_gpf_1 * frame, int width, int height, int border, int border_gap,
                    int corner_mark_size, int corner_mark_gap, int tiles_per_column, int max_levels_per_symbol, int reference_bar_freq_divider,
                    int analog_content_symbol_size, int digital_content_symbol_size, int reference_bar_sync_distance, int reference_bar_sync_offset)
{
    boxing_generic_frame_init(&frame->base);
    
    frame->base.size = size;
    frame->base.max_levels_per_symbol = frame_max_levels_per_symbol;
    frame->base.levels_per_symbol = levels_per_symbol;
    frame->base.set_levels_per_symbol = set_levels_per_symbol;
    frame->base.set_system_human_readable = set_system_human_readable;
    frame->base.set_user_human_readable = set_user_human_readable;
    frame->base.render = render;
    frame->base.free = frame_gpf_1_free;
    frame->base.container = frame_gpf_1_container;
    frame->base.metadata_container = frame_gpf_1_metadata_container;
    frame->base.create_frame_tracker = frame_gpf_1_create_tracker;
    

    frame->max_levels_per_symbol = max_levels_per_symbol;
    frame->levels_per_symbol = max_levels_per_symbol;
    frame->corner_mark_size = corner_mark_size;
    frame->corner_mark_gap = corner_mark_gap;
    frame->tiles_per_column = tiles_per_column;
    frame->reference_bar_freq_divider = reference_bar_freq_divider;
    frame->analog_content_symbol_size = analog_content_symbol_size;
    frame->digital_content_symbol_size = digital_content_symbol_size;
    frame->reference_bar_sync_distance = reference_bar_sync_distance;
    frame->reference_bar_sync_offset = reference_bar_sync_offset;

    boxing_container_impl_1_init(&frame->generic_container, frame);
    boxing_container_imeta_1_init(&frame->generic_metadata_container, frame);

    boxing_border_init(&frame->base_border, border, border_gap);
    // populate component
    add_components(frame);

    // update all subcomponents with new dimensions
    boxing_generic_frame_gpf_1_set_size(frame, width, height);
    frame->base_border.base.foreground_color = max_levels_per_symbol - 1;
    frame->base_border.base.background_color = 0;
    frame->base_border.base.pos.x = frame->base_border.base.pos.y = 0;
}


//----------------------------------------------------------------------------
/*!
 *  \brief Set frame sizes.
 *
 *  Set frame sizes according to the input width and height.
 *
 *  \param[in] frame   Pointer to the boxing_frame_gpf_1 structure.
 *  \param[in] width   New frame width.
 *  \param[in] height  New frame height.
 */

void boxing_generic_frame_gpf_1_set_size(boxing_frame_gpf_1 * frame, int width, int height)
{
    frame->base_border.base.set_size((boxing_component*)&frame->base_border, width, height);

    const boxing_pointi viewport_size          = frame->base_border.container->size;
    const int frame_width                      = viewport_size.x;
    const int frame_height                     = viewport_size.y;
    const int calibration_band_width           = frame_width - 2 * frame->corner_mark_size;
    const int calibration_band_height          = frame->corner_mark_size / 2;
    const int metadata_band_width              = frame_width - 2 * frame->corner_mark_size;
    const int metadata_band_height             = frame->corner_mark_size / 2;
    const int user_content_width               = frame->corner_mark_size / 2;
    const int user_content_height              = frame_height - 2 * frame->corner_mark_size;
    const int content_width                    = frame_width  - 2 * frame->corner_mark_size;
    const int content_height                   = frame_height - 2 * frame->corner_mark_size;
    const int reference_band_width             = frame->corner_mark_size / 2;
    const int horizontal_reference_band_length = frame_width  - 2 * frame->corner_mark_size;
    const int vertical_reference_band_length   = frame_height - 2 * frame->corner_mark_size;


    // top left corner mark;
    frame->top_left_reference_point->base.set_size((boxing_component*)frame->top_left_reference_point,
            frame->corner_mark_size, frame->corner_mark_size);
    frame->top_left_reference_point->base.pos.x = frame->top_left_reference_point->base.pos.y = 0;

    // top right corner mark;
    frame->top_right_reference_point->base.set_size((boxing_component*)frame->top_right_reference_point,
            frame->corner_mark_size, frame->corner_mark_size);
    frame->top_right_reference_point->base.pos.x = frame_width - frame->corner_mark_size;
    frame->top_right_reference_point->base.pos.y = 0;

    // bottom left corner mark;
    frame->bottom_left_reference_point->base.set_size((boxing_component*)frame->bottom_left_reference_point,
            frame->corner_mark_size, frame->corner_mark_size);
    frame->bottom_left_reference_point->base.pos.x = 0;
    frame->bottom_left_reference_point->base.pos.y = frame_height - frame->corner_mark_size;

    // bottom right corner mark;
    frame->bottom_right_reference_point->base.set_size((boxing_component*)frame->bottom_right_reference_point,
            frame->corner_mark_size, frame->corner_mark_size);
    frame->bottom_right_reference_point->base.pos.x = frame_width - frame->corner_mark_size;
    frame->bottom_right_reference_point->base.pos.y = frame_height - frame->corner_mark_size;

    // top reference bar
    frame->top_reference_bar->base.set_size((boxing_component*)frame->top_reference_bar, horizontal_reference_band_length, reference_band_width);
    frame->top_reference_bar->base.pos.x = frame->corner_mark_size;
    frame->top_reference_bar->base.pos.y = frame->corner_mark_size/2;
    frame->top_reference_bar->quiet_zone_size = frame->base_border.gap_size;

    // bottom reference bar
    frame->bottom_reference_bar->base.set_size((boxing_component*)frame->bottom_reference_bar,
            horizontal_reference_band_length, reference_band_width);
    frame->bottom_reference_bar->base.pos.x = frame->corner_mark_size;
    frame->bottom_reference_bar->base.pos.y = frame_height - frame->corner_mark_size;
    frame->bottom_reference_bar->quiet_zone_size = frame->base_border.gap_size;

    // left reference bar
    frame->left_reference_bar->base.set_size((boxing_component*)frame->left_reference_bar, reference_band_width, vertical_reference_band_length);
    frame->left_reference_bar->base.pos.x = frame->corner_mark_size/2;
    frame->left_reference_bar->base.pos.y = frame->corner_mark_size;
    frame->left_reference_bar->quiet_zone_size = frame->base_border.gap_size;

    // right reference bar
    frame->right_reference_bar->base.set_size((boxing_component*)frame->right_reference_bar, reference_band_width, vertical_reference_band_length);
    frame->right_reference_bar->base.pos.x = frame_width - frame->corner_mark_size;
    frame->right_reference_bar->base.pos.y = frame->corner_mark_size;
    frame->right_reference_bar->quiet_zone_size = frame->base_border.gap_size;

    // calabration bar
    frame->calibration_bar->base.set_size((boxing_component*)frame->calibration_bar, calibration_band_width, calibration_band_height);
    frame->calibration_bar->base.pos.x = frame->corner_mark_size;
    frame->calibration_bar->base.pos.y = 0;

    // content container
    frame->content_container->base.set_size((boxing_component*)frame->content_container, content_width, content_height);
    frame->content_container->base.pos.x = frame->corner_mark_size;
    frame->content_container->base.pos.y = frame->corner_mark_size;

    // metadata bar
    frame->metadata_bar->base.set_size((boxing_component*)frame->metadata_bar, metadata_band_width, metadata_band_height);
    boxing_metadata_bar_set_tile_size(frame->metadata_bar, metadata_band_height/frame->tiles_per_column);
    frame->metadata_bar->base.pos.x = frame->corner_mark_size;
    frame->metadata_bar->base.pos.y = frame_height - frame->corner_mark_size/2;

    // system define human readable
    frame->label_system_define->base.set_size((boxing_component*)frame->label_system_define, user_content_width, user_content_height);
    frame->label_system_define->base.pos.x = 0;
    frame->label_system_define->base.pos.y = frame->corner_mark_size;

    // user define human readable
    frame->label_user_define->base.set_size((boxing_component*)frame->label_user_define, user_content_width, user_content_height);
    frame->label_user_define->base.pos.x = frame_width - user_content_width -1;
    frame->label_user_define->base.pos.y = frame->corner_mark_size;
}


//----------------------------------------------------------------------------
/*!
 *  \brief Get corner mark dimension.
 *
 *  Get corner mark dimension from the boxing_frame_gpf_1 instance.
 *
 *  \param[in]  frame  Pointer to the boxing_frame_gpf_1 structure.
 *  \return Corner mark dimension.
 */

boxing_pointi boxing_generic_frame_gpf_1_corner_mark_dimension(boxing_frame_gpf_1 * frame)
{
    return frame->top_left_reference_point->base.size;
}


//----------------------------------------------------------------------------
/*!
 *  \brief Get corner mark gap size.
 *
 *  Get corner mark gap size from the boxing_frame_gpf_1 instance.
 *
 *  \param[in]  frame  Pointer to the boxing_frame_gpf_1 structure.
 *  \return Corner mark gap size.
 */

int boxing_generic_frame_gpf_1_corner_mark_gap_size(boxing_frame_gpf_1 * frame)
{
    return frame->top_left_reference_point->gap_size;
}


//----------------------------------------------------------------------------
/*!
 *  \brief Set tracker options.
 *
 *  Set tracker options according to the input data.
 *
 *  \param[in] frame                  Pointer to the boxing_frame_gpf_1 structure.
 *  \param[in] sync_point_h_distance  Sync point horizontal distance.
 *  \param[in] sync_point_v_distance  Sync point vertical distance.
 *  \param[in] sync_point_h_offset    Sync point horizontal offset.
 *  \param[in] sync_point_v_offset    Sync point vertical offset.
 *  \param[in] sync_point_radius      Sync point radius.
 */

void boxing_generic_frame_gpf_1_set_tracker_options(boxing_frame_gpf_1 * frame, int sync_point_h_distance, int sync_point_v_distance,
                    int sync_point_h_offset, int sync_point_v_offset, int sync_point_radius)
{
    frame->sync_point_h_distance = sync_point_h_distance;
    frame->sync_point_v_distance = sync_point_v_distance;
    frame->sync_point_h_offset = sync_point_h_offset;
    frame->sync_point_v_offset = sync_point_v_offset;
    frame->sync_point_radius   = sync_point_radius;
}


//----------------------------------------------------------------------------
/*!
  * \} end of graphics group
  */


// PRIVATE GENERIC FRAME GPF 1 FUNCTIONS
//

static int imeta_capasity(boxing_generic_container * container)
{
    return ((boxing_container_imeta_1*)container)->frame->metadata_bar->data_capasity;
}

static boxing_pointi imeta_dimension(boxing_generic_container * container)
{
    return ((boxing_container_imeta_1*)container)->frame->metadata_bar->base.size;
}

static boxing_pointi imeta_location(boxing_generic_container * container)
{
    return boxing_component_absolute_location((boxing_component*)((boxing_container_imeta_1*)container)->frame->metadata_bar);
}

static void imeta_set_data(boxing_generic_container * container, const char * data, int size)
{
    boxing_metadata_bar_set_data(((boxing_container_imeta_1*)container)->frame->metadata_bar, data, size);
}

static boxing_pointi imeta_tile_size(boxing_generic_container * container)
{
    int tile_size = ((boxing_container_imeta_1*)container)->frame->metadata_bar->tile_size;
    boxing_pointi return_value = { tile_size, tile_size };
    return return_value;
}

static int impl_capasity(boxing_generic_container * container)
{
    return ((boxing_container_impl_1*)container)->frame->content_container->data_capasity;
}

static boxing_pointi impl_dimension(boxing_generic_container * container)
{
    return ((boxing_container_impl_1*)container)->frame->content_container->base.size;
}

static boxing_pointi impl_location(boxing_generic_container * container)
{
    return boxing_component_absolute_location((boxing_component *)((boxing_container_impl_1*)container)->frame->content_container);
}

static void impl_set_data(boxing_generic_container * container, const char * data, int size)
{
    boxing_content_container_set_data(((boxing_container_impl_1*)container)->frame->content_container, data, size);
}

static boxing_pointi impl_tile_size(boxing_generic_container * container, enum boxing_content_type type)
{
    boxing_pointi return_value;
    switch (type)
    {
    case BOXING_CONTENT_TYPE_DIGITAL:
        return_value.x = ((boxing_container_impl_1*)container)->frame->digital_content_symbol_size;
        return_value.y = ((boxing_container_impl_1*)container)->frame->digital_content_symbol_size;
        return return_value;
    case BOXING_CONTENT_TYPE_ANALOG:
        return_value.x = ((boxing_container_impl_1*)container)->frame->analog_content_symbol_size;
        return_value.y = ((boxing_container_impl_1*)container)->frame->analog_content_symbol_size;
        return return_value;
    default:
        return_value.x = return_value.y = 1;
        return return_value;
    }
}

static void impl_set_tile_size(boxing_generic_container * container, const boxing_pointi size)
{
    boxing_generic_frame_container_set_tile_size(container, size);
    boxing_content_container_set_symbol_size(((boxing_container_impl_1*)container)->frame->content_container, size.x);
}

static struct boxing_tracker_s * frame_gpf_1_create_tracker(boxing_frame * frame) // move me!
{
    return (struct boxing_tracker_s *)boxing_tracker_gpf_1_create(((boxing_frame_gpf_1*)frame));
}


static boxing_generic_container *  frame_gpf_1_metadata_container(boxing_frame * frame)
{
    return (boxing_generic_container*)&SMEMBER(generic_metadata_container);
}

static boxing_generic_container * frame_gpf_1_container(boxing_frame * frame)
{
    return (boxing_generic_container*)&SMEMBER(generic_container);
}

static void add(boxing_frame_gpf_1 * frame, boxing_component * child)
{
    if (!child)
        return;

    child->parent = (boxing_component *)&frame->base_border;
    frame->base_border.base.add((boxing_component *)&frame->base_border, child);
}

static void add_components(boxing_frame_gpf_1 * frame)
{
    // top left corner mark;
    frame->top_left_reference_point = malloc(sizeof(boxing_reference_point));
    boxing_reference_point_init(frame->top_left_reference_point, frame->corner_mark_gap);
    add(frame, (boxing_component *)frame->top_left_reference_point);

    // top right corner mark;
    frame->top_right_reference_point = malloc(sizeof(boxing_reference_point));
    boxing_reference_point_init(frame->top_right_reference_point, frame->corner_mark_gap);
    add(frame, (boxing_component *)frame->top_right_reference_point);

    // bottom left corner mark;
    frame->bottom_left_reference_point = malloc(sizeof(boxing_reference_point));
    boxing_reference_point_init(frame->bottom_left_reference_point, frame->corner_mark_gap);
    add(frame, (boxing_component *)frame->bottom_left_reference_point);

    // bottom right corner mark;
    frame->bottom_right_reference_point = malloc(sizeof(boxing_reference_point));
    boxing_reference_point_init(frame->bottom_right_reference_point, frame->corner_mark_gap);
    add(frame, (boxing_component *)frame->bottom_right_reference_point);

    // top reference bar
    frame->top_reference_bar = malloc(sizeof(boxing_reference_bar));
    boxing_reference_bar_init(frame->top_reference_bar, BOXING_ALIGN_HORIZONTAL, frame->reference_bar_freq_divider, frame->reference_bar_sync_distance, frame->reference_bar_sync_offset);
    add(frame, (boxing_component *)frame->top_reference_bar);

    // bottom reference bar
    frame->bottom_reference_bar = malloc(sizeof(boxing_reference_bar));
    boxing_reference_bar_init(frame->bottom_reference_bar, BOXING_ALIGN_HORIZONTAL, frame->reference_bar_freq_divider, frame->reference_bar_sync_distance, frame->reference_bar_sync_offset);
    add(frame, (boxing_component *)frame->bottom_reference_bar);

    // left reference bar
    frame->left_reference_bar = malloc(sizeof(boxing_reference_bar));
    boxing_reference_bar_init(frame->left_reference_bar, BOXING_ALIGN_VERTICAL, frame->reference_bar_freq_divider, frame->reference_bar_sync_distance, frame->reference_bar_sync_offset);
    add(frame, (boxing_component *)frame->left_reference_bar);

    // right reference bar
    frame->right_reference_bar = malloc(sizeof(boxing_reference_bar));
    boxing_reference_bar_init(frame->right_reference_bar, BOXING_ALIGN_VERTICAL, frame->reference_bar_freq_divider, frame->reference_bar_sync_distance, frame->reference_bar_sync_offset);
    add(frame, (boxing_component *)frame->right_reference_bar);

    // calabration bar
    frame->calibration_bar = malloc(sizeof(boxing_calibration_bar));
    boxing_calibration_bar_init(frame->calibration_bar, frame->levels_per_symbol);
    add(frame, (boxing_component *)frame->calibration_bar);

    // content container
    frame->content_container = malloc(sizeof(boxing_content_container));
    boxing_content_container_init(frame->content_container);
    add(frame, (boxing_component *)frame->content_container);

    // metadata bar
    frame->metadata_bar = malloc(sizeof(boxing_metadata_bar));
    boxing_metadata_bar_init(frame->metadata_bar, frame->tiles_per_column);
    add(frame, (boxing_component *)frame->metadata_bar);

    // system define human readable
    frame->label_system_define = malloc(sizeof(boxing_label));
    boxing_label_init(frame->label_system_define, "", BOXING_ALIGN_VERTICAL);
    add(frame, (boxing_component *)frame->label_system_define);

    // user define human readable
    frame->label_user_define = malloc(sizeof(boxing_label));
    boxing_label_init(frame->label_user_define, "", BOXING_ALIGN_VERTICAL);
    add(frame, (boxing_component *)frame->label_user_define);

    frame->generic_container.base.set_tile_size(&frame->generic_container.base, frame->generic_container.base.get_tile_size_enum(&frame->generic_container.base, BOXING_CONTENT_TYPE_DIGITAL));
}


static void frame_gpf_1_free(boxing_frame * frame)
{
    SMEMBER(top_left_reference_point)->base.free((boxing_component*)SMEMBER(top_left_reference_point));
    SMEMBER(top_right_reference_point)->base.free((boxing_component*)SMEMBER(top_right_reference_point));
    SMEMBER(bottom_left_reference_point)->base.free((boxing_component*)SMEMBER(bottom_left_reference_point));
    SMEMBER(bottom_right_reference_point)->base.free((boxing_component*)SMEMBER(bottom_right_reference_point));
    SMEMBER(top_reference_bar)->base.free((boxing_component*)SMEMBER(top_reference_bar));
    SMEMBER(bottom_reference_bar)->base.free((boxing_component*)SMEMBER(bottom_reference_bar));
    SMEMBER(left_reference_bar)->base.free((boxing_component*)SMEMBER(left_reference_bar));
    SMEMBER(right_reference_bar)->base.free((boxing_component*)SMEMBER(right_reference_bar));
    SMEMBER(calibration_bar)->base.free((boxing_component*)SMEMBER(calibration_bar));
    SMEMBER(content_container)->base.free((boxing_component*)SMEMBER(content_container));
    SMEMBER(metadata_bar)->base.free((boxing_component*)SMEMBER(metadata_bar));
    SMEMBER(label_system_define)->base.free((boxing_component*)SMEMBER(label_system_define));
    SMEMBER(label_user_define)->base.free((boxing_component*)SMEMBER(label_user_define));

    free(SMEMBER(top_left_reference_point));
    free(SMEMBER(top_right_reference_point));
    free(SMEMBER(bottom_left_reference_point));
    free(SMEMBER(bottom_right_reference_point));
    free(SMEMBER(top_reference_bar));
    free(SMEMBER(bottom_reference_bar));
    free(SMEMBER(left_reference_bar));
    free(SMEMBER(right_reference_bar));
    free(SMEMBER(calibration_bar));
    free(SMEMBER(content_container));
    free(SMEMBER(metadata_bar));
    free(SMEMBER(label_system_define));
    free(SMEMBER(label_user_define));
    boxing_border_free((boxing_component*)&SMEMBER(base_border));

    boxing_generic_frame_free(frame);
}


static int frame_max_levels_per_symbol(boxing_frame * frame)
{
    return SMEMBER(max_levels_per_symbol);
}

static int levels_per_symbol(boxing_frame * frame)
{
    return SMEMBER(levels_per_symbol);
}

static void set_levels_per_symbol(boxing_frame * frame, int levels, DBOOL force /*  = false */)
{
    if (levels < 2)
    {
        levels = 2;
    }
    else if ((levels > SMEMBER(max_levels_per_symbol)) && !force)
    {
        levels = SMEMBER(max_levels_per_symbol);
    }
    SMEMBER(levels_per_symbol) = levels;
    SMEMBER(calibration_bar)->levels_per_symbol = levels;
    SMEMBER(base_border).base.foreground_color = levels - 1;
}

static void set_system_human_readable(boxing_frame * frame, const char * str)
{
    boxing_label_set_label(SMEMBER(label_system_define), str);
}

static void set_user_human_readable(boxing_frame * frame, const char * str)
{
    boxing_label_set_label(SMEMBER(label_user_define), str);
}

static void render(boxing_frame * frame, boxing_painter * painter)
{
    boxing_pointi pos = SMEMBER(base_border).base.pos;
    boxing_pointi size = frame->size(frame);
    painter->fill_rect(painter, pos.x, pos.y, size.x, size.y, boxing_component_get_background_color((boxing_component*)&SMEMBER(base_border)));
    SMEMBER(base_border).base.render((boxing_component*)&SMEMBER(base_border), painter);
}

static boxing_pointi size(const boxing_frame * frame)
{
    return SMEMBER(base_border).base.size;
}
