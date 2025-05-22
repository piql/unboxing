/*****************************************************************************
**
**  Implementation of the genericframefactory interface
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
#include    "boxing/graphics/genericframefactory.h"
#include    "boxing/graphics/genericframegpf_1.h"
#include    "boxing/log.h"

//  PRIVATE INTERFACE
//

static boxing_frame * generic_frame_gpf_1(const boxing_config * config);


/*! 
  * \addtogroup graphics
  * \{
  */


// PUBLIC GENERIC FRAME FACTORY FUNCTIONS
//

//----------------------------------------------------------------------------
/*!
 *  \brief Create a boxing_frame_s instance.
 *
 *  Allocate memory for the boxing_frame_s type
 *  and initializes all structure data according to the input config.
 *  Return instance of allocated structure.
 *
 *  \param[in] config  Configuration.
 *  \return instance of allocated afs_toc_data_file_metadata_source structure.
 */

struct boxing_frame_s * boxing_generic_frame_factory_create(const boxing_config * config)
{
    if(!config)
    {
        DLOG_INFO( "config is NULL" );
        return NULL;
    }

    const char * typestr = boxing_config_property(config, "FrameFormat", "type");
    if (typestr == NULL)
    {
        DLOG_INFO( "Key 'type' is missing from config" );
        return NULL;
    }
    if (
        strcmp("GPFv1.0", typestr) == 0 ||
        strcmp("GPFv1.1", typestr) == 0 ||
        strcmp("GPFv1.2", typestr) == 0
    )
    {
        return generic_frame_gpf_1(config);
    }
    else
    {
        DLOG_INFO( "Illegal frame type" );
        return NULL;
    }
}


//----------------------------------------------------------------------------
/*!
 *  \brief Frees occupied memory of boxing_frame_s structure.
 *
 *  Frees occupied memory of all internal structure pointers.
 *
 *  \param[in]  frame  Pointer to the boxing_frame_s structure.
 */

void boxing_generic_frame_factory_free(struct boxing_frame_s * frame)
{
    if (frame)
    {
        if (frame->free)
        {
            frame->free(frame);
        }
        free(frame);
    }
}


//----------------------------------------------------------------------------
/*!
  * \} end of graphics group
  */


// PRIVATE GENERIC FRAME FACTORY FUNCTIONS
//

static int get_parameter_int(const boxing_config * config, const char * class_name, const char * parameter, int default_value)
{
    return boxing_config_is_set(config, class_name, parameter) ?
           boxing_config_property_int(config, class_name, parameter) : default_value;
}

static const char * get_parameter_string(const boxing_config * config, const char * class_name, const char * parameter, const char * default_value)
{
    if (!boxing_config_is_set(config, class_name, parameter))
    {
        return default_value;
    }
    else
    {
        return boxing_config_property(config, class_name, parameter);
    }
}

static boxing_frame * generic_frame_gpf_1(const boxing_config * config)
{
    if(!(boxing_config_is_set(config, "FrameFormat", "width") && 
        boxing_config_is_set(config, "FrameFormat", "height")))
    {
        return 0;
    }

    
    const int width                       = boxing_config_property_int(config, "FrameFormat", "width");
    const int height                      = boxing_config_property_int(config, "FrameFormat", "height");
    const int border                      = get_parameter_int(config, "FrameFormat", "border",                    1);
    const int border_gap                  = get_parameter_int(config, "FrameFormat", "borderGap",                 1);
    const int corner_mark_size            = get_parameter_int(config, "FrameFormat", "cornerMarkSize",           32);
    const int corner_mark_gap             = get_parameter_int(config, "FrameFormat", "cornerMarkGap",             1);
    const int tiles_per_column            = get_parameter_int(config, "FrameFormat", "tilesPerColumn",            4);
    const int reference_bar_freq_divider  = get_parameter_int(config, "FrameFormat", "referenceBarFreqDivider",   1);
    const int analog_content_symbol_size  = get_parameter_int(config, "FrameFormat", "analogContentSymbolSize",   1);
    const int digital_content_symbol_size = get_parameter_int(config, "FrameFormat", "digitalContentSymbolSize",  1);
          int reference_bar_sync_distance = get_parameter_int(config, "FrameFormat", "refBarSyncDistance",       -1);
          int reference_bar_sync_offset   = get_parameter_int(config, "FrameFormat", "refBarSyncOffset",          0);
    const char * name                     = get_parameter_string(config, "FormatInfo",  "name",                     "");
    const char * short_description        = get_parameter_string(config, "FormatInfo",  "shortDescription",         "");
    const char * description              = get_parameter_string(config, "FormatInfo",  "description",              "");

    const char * frame_format_version = get_parameter_string(config, "FrameFormat", "type", "GPFv1.0");
    if (strcmp("GPFv1.0", frame_format_version) == 0)
    {
        reference_bar_sync_distance = -1;
        reference_bar_sync_offset = 0;
    }

    if(!boxing_config_is_set(config, "FrameFormat", "maxLevelsPerSymbol"))
    {
        return 0;
    }
    const int max_levels_per_symbol = boxing_config_property_int(config, "FrameFormat", "maxLevelsPerSymbol");

    boxing_frame * frame = malloc(sizeof(boxing_frame_gpf_1));
    boxing_generic_frame_gpf_1_init((boxing_frame_gpf_1*)frame, width, height, border, border_gap, corner_mark_size,
                                                             corner_mark_gap, tiles_per_column, max_levels_per_symbol, reference_bar_freq_divider,
                                                             analog_content_symbol_size, digital_content_symbol_size,
                                                             reference_bar_sync_distance, reference_bar_sync_offset);

    boxing_generic_frame_set_format(frame, frame_format_version);
    boxing_generic_frame_set_name(frame, name);
    boxing_generic_frame_set_short_description(frame, short_description);
    boxing_generic_frame_set_description(frame, description);

    /* set tracker spesific options */
    if (boxing_config_is_set(config, "CodecDispatcher", "DataCodingScheme")) 
    {
        gvector * list_process = boxing_config_parse_list_properties((boxing_config *)config, "CodecDispatcher", "DataCodingScheme");

        if ((strcmp("GPFv1.0", frame_format_version) == 0) || (strcmp("GPFv1.1", frame_format_version) == 0))
        {
            for (unsigned int i = 0; i < list_process->size; i++)
            {
                char * class_name_str = GVECTORN(list_process, char *, i);
                if (strcmp(class_name_str, "SyncPointInserter") == 0 && boxing_config_is_set(config, class_name_str, "codec"))
                {
                    const int sync_point_distance_pixel = get_parameter_int(config, "SyncPointInserter", "SyncPointDistancePixel", 100);
                    const int sync_point_radius_pixel = get_parameter_int(config, "SyncPointInserter", "SyncPointRadiusPixel", 2);

                    const int sync_point_h_distance_pixel = get_parameter_int(config, "SyncPointInserter", "SyncPointHDistancePixel", sync_point_distance_pixel);
                    const int sync_point_v_distance_pixel = get_parameter_int(config, "SyncPointInserter", "SyncPointVDistancePixel", sync_point_distance_pixel);
                    const int sync_point_h_offset_pixel = get_parameter_int(config, "SyncPointInserter", "SyncPointVOffsetPixel", -1);
                    const int sync_point_v_offset_pixel = get_parameter_int(config, "SyncPointInserter", "SyncPointHOffsetPixel", -1);

                    boxing_generic_frame_gpf_1_set_tracker_options((boxing_frame_gpf_1*)frame, sync_point_h_distance_pixel, sync_point_v_distance_pixel,
                        sync_point_h_offset_pixel, sync_point_v_offset_pixel, sync_point_radius_pixel);
                }
            }
        }
        else
        {
            const int sync_point_radius_pixel = get_parameter_int(config, "FrameFormat", "syncPointRadius", 3);
            const int sync_point_h_distance_pixel = get_parameter_int(config, "FrameFormat", "syncPointHDistance", 100);
            const int sync_point_v_distance_pixel = get_parameter_int(config, "FrameFormat", "syncPointVDistance", 100);
            const int sync_point_h_offset_pixel = get_parameter_int(config, "FrameFormat", "syncPointVOffset", -1);
            const int sync_point_v_offset_pixel = get_parameter_int(config, "FrameFormat", "syncPointHOffset", -1);

            boxing_generic_frame_gpf_1_set_tracker_options((boxing_frame_gpf_1*)frame, sync_point_h_distance_pixel, sync_point_v_distance_pixel,
                sync_point_h_offset_pixel, sync_point_v_offset_pixel, sync_point_radius_pixel);

        }
        gvector_free(list_process);
    }

    return frame;
}
