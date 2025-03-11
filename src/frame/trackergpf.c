/*****************************************************************************
**
**  Implementation of the dframetrackergpf_b1 interface
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
#include    "boxing/frame/trackergpf.h"
#include    "boxing/unboxer/frametrackerutil.h"
#include    "boxing/string.h"

//  BASE INCLUDES
//
#include    "boxing/log.h"

//  DEFINES
//

#define SMEMBER(member) (((boxing_tracker_gpf *)tracker)->member)

//  PRIVATE INTERFACE
//

static int track_frame_simulated_mode(boxing_tracker_gpf * tracker, const boxing_image8 * frame);
static int boxing_tracker_gpf_track_corner_marks(
    void* user,
    const boxing_image8 * image,
    boxing_corner_mark_definition* definition,
    boxing_float sampling_rate_x,
    boxing_float sampling_rate_y,
    frame_corner_marks* corner_marks );


/*! 
  * \addtogroup frame
  * \{
  */


//----------------------------------------------------------------------------
/*!
 *  \enum   Mode
 *  \brief  The enumeration for specifying the mode of frame tracker.
 *  
 *  \param BOXING_TRACK_SIMULATED              (0x0001) Simulated tracking mode.
 *  \param BOXING_TRACK_REFERENCE_MARKS        (0x0002) Track reference mark mode.
 *  \param BOXING_TRACK_HORIZONTAL_SHIFT       (0x0004) Track horizontal shift mode.
 *  \param BOXING_TRACK_VERTICAL_SHIFT         (0x0008) Track vertical shift mode.
 *  \param BOXING_TRACK_CONTENT_CONTAINER      (0x0010) Track content conteiner mode.
 *  \param BOXING_TRACK_METADATA_CONTAINER     (0x0020) Track metadata container mode.
 *  \param BOXING_TRACK_CALIBRATION_BAR        (0x0040) Track calibration bar mode.
 *  \param BOXING_TRACK_SYSTEM_HUMAN_READABLE  (0x0080) Track system human readable mode.
 *  \param BOXING_TRACK_USER_HUMAN_READABLE    (0x0100) Track user human readable mode.
 *  \param BOXING_TRACK_SYNC_POINTS            (0x0200) Track sync points mode.
 *
 *  The enumeration for specifying the mode of frame tracker.
 */


/*! 
 *  \var typedef int (*boxing_tracker_frame_callback)(struct boxing_tracker_s * tracker, const boxing_image8 * frame)
 *  \brief Type definitions for tracker frame callback function.
 *   
 *  Type definitions for specifying tracker frame callback function.
 */


/*! 
 *  \var typedef void (*boxing_tracker_free_callback)(struct boxing_tracker_s * tracker)
 *  \brief Type definitions for tracker free callback function.
 *   
 *  Type definitions for specifying tracker free callback function.
 */


/*! 
 *  \var typedef int (*boxing_tracker_gpf_simulated_mode_callback)(struct boxing_tracker_gpf_s * tracker, const boxing_image8 * frame)
 *  \brief Type definitions for tracker gpf simulated mode callback function.
 *   
 *  Type definitions for specifying tracker gpf simulated mode callback function.
 */


/*! 
 *  \var typedef int (*boxing_tracker_gpf_analog_mode_callback)(struct boxing_tracker_gpf_s * tracker, const boxing_image8 * frame)
 *  \brief Type definitions for tracker gpf analog mode callback function.
 *   
 *  Type definitions for specifying tracker gpf analog mode callback function.
 */


//----------------------------------------------------------------------------
/*!
 *  \struct     boxing_tracker_s  frametracker.h
 *  \brief      Struct for boxing tracker data storage.
 *  
 *  \param track_frame              Pointer to the boxing_tracker_frame_callback callback function.
 *  \param free                     Pointer to the boxing_tracker_free_callback callback function.
 *  \param track_corner_mark        Pointer to the boxing_corner_mark_callback callback function.
 *  \param mode                     Mode of the frame tracker.
 *  \param container_sampler_list   Pointer to the conteiner sampler list.
 *  \param x_sampling_rate          Sampling rate along the X axis.
 *  \param y_sampling_rate          Sampling rate along the Y axis.
 *  \param type                     Type string.
 *  \param on_corner_mark_complete  Pointer to the boxing_corner_mark_complete_cb callback function.
 *  \param user_data                Pointer to the user data.
 *
 *  Struct for boxing tracker data storage.
 */


//----------------------------------------------------------------------------
/*!
 *  \struct     boxing_tracker_gpf_s  frametrackergpfbase.h
 *  \brief      Struct for boxing tracker gpf data storage.
 *  
 *  \param base                        Tracker base structure instance.
 *  \param track_frame_simulated_mode  Pointer to the boxing_tracker_gpf_simulated_mode_callback callback function.
 *  \param track_frame_analog_mode     Pointer to the boxing_tracker_gpf_analog_mode_callback callback function.
 *  \param generic_frame               Pointer to the boxing_frame structure.
 *  \param metadata_sampler            Metadata sampler.
 *  \param content_sampler             Content sampler.
 *  \param calibration_bar_sampler     Calibration bar sampler.
 *
 *  Struct for boxing tracker gpf data storage.
 */


// PUBLIC FRAME TRACKER GPF BASE FUNCTIONS
//

//----------------------------------------------------------------------------
/*!
 *  \brief Get the boxing_sampler structure instance.
 *
 * Get the boxing_sampler structure instance for the given key.
 *
 *  \param[in]  tracker  Pointer to the boxing_tracker_s structure.
 *  \param[in]  key      Key.
 *  \return Pointer to the boxing_sampler structure instance.
 */

boxing_sampler * boxing_tracker_get_container_sampler(struct boxing_tracker_s * tracker, const char * key)
{
    return g_hash_table_lookup(tracker->container_sampler_list, key);
}


//----------------------------------------------------------------------------
/*!
 *  \brief The function checks type of the frame tracker.
 *
 *  The function checks type of the frame tracker.
 *  If type of the frame tracker is equal to the input type, then function return DTRUE.
 *
 *  \param[in]  tracker  Pointer to the boxing_tracker_s structure.
 *  \param[in]  type     Type.
 *  \return DTRUE if type of the frame tracker is equal to the input type.
 */

DBOOL boxing_tracker_is_type(boxing_tracker * tracker, const char * type)
{
    return boxing_string_equal( tracker->type, type );
}


//----------------------------------------------------------------------------
/*!
 *  \brief Initializes all boxing_tracker_gpf structure data.
 *
 *  Initializes input generic_frame with specified input value,
 *  and initializes other data with default values.
 *
 *  \param[in]  tracker        Pointer to the boxing_tracker_s structure.
 *  \param[in]  generic_frame  Pointer to the boxing_frame structure.
 */

void boxing_tracker_gpf_init(boxing_tracker_gpf * tracker, boxing_frame * generic_frame)
{
    tracker->generic_frame = generic_frame;

    boxing_tracker_init(&tracker->base);
    tracker->base.container_sampler_list = g_hash_table_new_full(g_str_hash, g_str_equal, boxing_utils_g_hash_table_destroy_item_string, NULL);
    tracker->base.free = boxing_tracker_gpf_free;
    tracker->base.track_frame = boxing_tracker_gpf_track_frame;
    tracker->base.mode = BOXING_TRACK_CONTENT_CONTAINER|BOXING_TRACK_METADATA_CONTAINER;
    tracker->base.track_corner_mark = boxing_tracker_gpf_track_corner_marks;
    tracker->track_frame_simulated_mode = track_frame_simulated_mode;
    tracker->track_frame_analog_mode = NULL;
    tracker->content_sampler = NULL;
    tracker->metadata_sampler = NULL;
    tracker->calibration_bar_sampler = NULL;
}


//----------------------------------------------------------------------------
/*!
 *  \brief Frees occupied memory of boxing_tracker structure.
 *
 *  Frees occupied memory of all internal structure pointers. Does not free the original pointer.
 *
 *  \param[in]  tracker  Pointer to the boxing_tracker structure.
 */

void boxing_tracker_gpf_free(boxing_tracker * tracker)
{
    boxing_sampler_destroy(SMEMBER(content_sampler));
    boxing_sampler_destroy(SMEMBER(metadata_sampler));
    boxing_sampler_destroy(SMEMBER(calibration_bar_sampler));
    g_hash_table_destroy(tracker->container_sampler_list);
}


//----------------------------------------------------------------------------
/*!
 *  \brief Initializes all structure pointers and variables with default values.
 *
 *  Initializes all input structure pointers and variables with default values.
 *
 *  \param[in] tracker  Pointer to the boxing_tracker structure.
 */

void boxing_tracker_init(boxing_tracker * tracker)
{
    tracker->track_frame = NULL;
    tracker->free = NULL;
    tracker->track_corner_mark = NULL;
    tracker->container_sampler_list = NULL;
    tracker->mode = 0;
#ifdef BOXINGLIB_CALLBACK
    tracker->on_corner_mark_complete = NULL;
    tracker->user_data = NULL;
#endif
    tracker->type = NULL;
}


//----------------------------------------------------------------------------
/*!
 *  \brief Frees occupied memory of boxing_tracker structure.
 *
 *  Frees occupied memory of all internal structure pointers and structure pointer.
 *
 *  \param[in]  tracker  Pointer to the boxing_tracker structure.
 */

void boxing_tracker_destroy(boxing_tracker * tracker)
{
    if (tracker)
    {
        tracker->free(tracker);
        free(tracker);
    }
}


//----------------------------------------------------------------------------
/*!
 *  \brief Base track frame.
 *
 *  Base track frame function description.
 *
 *  \param[in]  tracker  Pointer to the boxing_tracker_s structure.
 *  \param[in]  frame    Image.
 *  \return function result.
 */

int boxing_tracker_gpf_track_frame(boxing_tracker * tracker, const boxing_image8 * frame)
{

    DLOG_INFO("boxing_tracker_gpf_track_frame Retrieving marks");

    for (int i = 0; i < tracker->container_sampler_list->size; i++)
    {
        guint node_hash = tracker->container_sampler_list->hashes[i];
        boxing_sampler * node_value = (boxing_sampler *)tracker->container_sampler_list->values[i];
        
        if (HASH_IS_REAL (node_hash) )
        {
            node_value->state = DFALSE;
        }
    }

    if(tracker->mode & BOXING_TRACK_SIMULATED)
        return SMEMBER(track_frame_simulated_mode)((boxing_tracker_gpf*)tracker, frame);
    else
        return SMEMBER(track_frame_analog_mode)((boxing_tracker_gpf*)tracker, frame);

}


//----------------------------------------------------------------------------
/*!
  * \} end of frame group
  */


// PRIVATE FRAME TRACKER GPF BASE FUNCTIONS
//

static boxing_image8 * crop_image(const boxing_image8 * image)
{
    int width = (int)image->width;
    int height = (int)image->height;

    // find top y
    int top_y;
    for (top_y = 0; top_y < height; top_y++)
    {
        if (IMAGE8_PIXEL(image, width / 2, top_y))
        {
            break;
        }
    }

    // find bottom y
    int bottom_y;
    for (bottom_y = height - 1; bottom_y > top_y; bottom_y--)
    {
        if (IMAGE8_PIXEL(image, width / 2, bottom_y))
        {
            break;
        }
    }

    // find left x
    int left_x;
    for (left_x = 0; left_x < width; left_x++)
    {
        if (IMAGE8_PIXEL(image, left_x, height / 2))
        {
            break;
        }
    }

    // find right x
    int right_x;
    for (right_x = width - 1; right_x > left_x; right_x--)
    {
        if (IMAGE8_PIXEL(image, right_x, height / 2))
        {
            break;
        }
    }

    return boxing_image8_crop(image, left_x, top_y, right_x - left_x + 1, bottom_y - top_y + 1);
}

static void generate_locations(boxing_matrixf * locations, const boxing_pointi start, const boxing_pointi inc)
{
    boxing_pointf * pos = locations->data;
    boxing_pointf* line = calloc(locations->width, sizeof(boxing_pointf));
    boxing_pointf* line_pos;
    boxing_pointf* line_end = line+locations->width;


    int x = start.x;
    for (line_pos = line; line_pos != line_end; line_pos++, x += inc.x)
    {
        line_pos->x = (boxing_float)x;
    }

    int y = start.y;
    for (unsigned int i = 0; i < locations->height; i++, y += inc.y)
    {
        for (line_pos = line; line_pos != line_end; line_pos++)
        {
            line_pos->y = (boxing_float)y;
        }

        memcpy(pos, line, locations->width*sizeof(boxing_pointf));
        pos += locations->width;
    }

    free(line);
}

static int track_frame_simulated_mode(boxing_tracker_gpf * tracker, const boxing_image8 * frame)
{
    boxing_pointi frame_dimension = tracker->generic_frame->size(tracker->generic_frame);

    boxing_image8 * cropped_frame = crop_image(frame); /// \todo Remove returning the whole image, as only it's width and height are used.
    if (cropped_frame == NULL)
    {
        DLOG_ERROR2("(track_frame_simulated_mode) Crop error WxH=%dx%d", frame->width, frame->height);
        return 1;
    }
    tracker->base.x_sampling_rate = (boxing_float)cropped_frame->width / (boxing_float)frame_dimension.x;
    tracker->base.y_sampling_rate = (boxing_float)cropped_frame->height / (boxing_float)frame_dimension.y;

    boxing_image8_free(cropped_frame);
    cropped_frame = NULL;

    /* _____________METADATA______________ */
    if (tracker->base.mode & BOXING_TRACK_METADATA_CONTAINER)
    {
        boxing_generic_container * container = tracker->generic_frame->metadata_container(tracker->generic_frame);
        boxing_pointi tile_size = container->get_tile_size(container);
        boxing_pointi location = container->location(container);

        boxing_pointi inc = { (int)(tile_size.x * tracker->base.x_sampling_rate), (int)(tile_size.y * tracker->base.y_sampling_rate) };
        boxing_pointi start = { (int)(location.x * tracker->base.x_sampling_rate + inc.x / 2), (int)(location.y * tracker->base.y_sampling_rate + inc.y / 2) };

        generate_locations(&tracker->metadata_sampler->location_matrix, start, inc);
        tracker->metadata_sampler->state = DTRUE;
    }

    /* _____________CONTENT______________ */
    if (tracker->base.mode & BOXING_TRACK_CONTENT_CONTAINER)
    {
        boxing_generic_container * container = tracker->generic_frame->container(tracker->generic_frame);
        boxing_pointi tile_size = container->get_tile_size(container);
        boxing_pointi location = container->location(container);

        boxing_pointi inc = { (int)(tile_size.x * tracker->base.x_sampling_rate), (int)(tile_size.y * tracker->base.y_sampling_rate) };
        boxing_pointi start = { (int)(location.x * tracker->base.x_sampling_rate + inc.x / 2), (int)(location.y * tracker->base.y_sampling_rate + inc.y / 2) };

        generate_locations(&tracker->content_sampler->location_matrix, start, inc);
        tracker->content_sampler->state = DTRUE;
    }

    /* _____________CALIBRATIONBAR______________ */

    return 0;
}

static int boxing_tracker_gpf_track_corner_marks(
    void* user,
    const boxing_image8 * image, 
    boxing_corner_mark_definition* definition, 
    boxing_float sampling_rate_x,
    boxing_float sampling_rate_y,
    frame_corner_marks* corner_marks )
{
    BOXING_UNUSED_PARAMETER( user );

    if (!boxing_frame_tracker_util_find_frame(image, corner_marks, sampling_rate_x, sampling_rate_y))
    {
        DLOG_ERROR( "(boxing_tracker_gpf_track_corner_marks) Finding reference marks failed" );
        return BOXING_CORNER_MARK_TRACKING_ERROR;
    }

    boxing_pointi ref_dim = {(int)( definition->corner_mark_symbol * sampling_rate_x ), (int)( definition->corner_mark_symbol * sampling_rate_y )};
    boxing_pointi* corners[] = { &corner_marks->top_left, &corner_marks->top_right, &corner_marks->bottom_left, &corner_marks->bottom_right };
    const char* names[] = { "Top left", "Top right", "Bottom left", "Bottom right" };
    const int corner_count = sizeof(corners)/sizeof(corners[0]);

    for ( int i = 0; i < corner_count; i++ )
    {
        DLOG_INFO3( "boxing_tracker_gpf_track_corner_marks %s reference mark     = (%i, %i)", names[i], corners[i]->x, corners[i]->y );
        if (!boxing_frame_tracker_util_validate_corner_mark(corners[i], &ref_dim, image))
        {
            DLOG_ERROR1( "(boxing_tracker_gpf_track_corner_marks) %s reference mark is not within image dimensions", names[i] );
            return BOXING_CORNER_MARK_TRACKING_ERROR;
        }
    }

    return BOXING_CORNER_MARK_OK;
}
