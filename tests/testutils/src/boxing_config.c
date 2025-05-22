/*****************************************************************************
**
**  Definition of the boxing config interface
**
**  Creation date:  2017/06/22
**  Created by:     Oleksandr Ivanov
**
**
**  Copyright (c) 2017 Piql AS. All rights reserved.
**
**  This file is part of the boxing library
**
*****************************************************************************/

//  PROJECT INCLUDES
//

#include "boxing/config.h"
#include "boxing_config.h"
#include "boxing/log.h"
#include <stdio.h>
#include "config_source_4kv6.h"
#include "config_source_4kv7.h"
#include "config_source_4kv8.h"
#include "config_source_4kv9.h"
#include "config_source_4kv10.h"
#include "config_source_4k_controlframe_v1.h"
#include "config_source_4k_controlframe_v2.h"
#include "config_source_4k_controlframe_v3.h"
#include "config_source_4k_controlframe_v4.h"
#include "config_source_4k_controlframe_v5.h"
#include "config_source_4k_controlframe_v6.h"
#include "config_source_4k_controlframe_v7.h"

//  DEFINES
//

static config_structure* boxing_formats[] = { &config_source_4kv6, &config_source_4kv7, &config_source_4kv8, &config_source_4kv9, &config_source_4kv10 };
static config_structure* boxing_control_frame_formats[] = { &config_source_v1, &config_source_v2, &config_source_v3, &config_source_v4, &config_source_v5, &config_source_v6, &config_source_v7 };

// PRIVATE INTERFACE
//

static config_structure* find_boxing_format(const char* boxing_format_name);
static const char *      get_format_name(config_structure* config);

// PUBLIC BOXING CONFIG FUNCTIONS
//

//----------------------------------------------------------------------------
/*!
 *  \brief Create boxing config
 *
 *  Create config instance from format name.
 *
 *  \ingroup unboxtests
 *  \param[in]   format_name  Boxing format name.
 *  \return boxing_config instance.
 */

boxing_config * boxing_create_boxing_config(const char* format_name)
{
    if (format_name == NULL)
    {
        return NULL;
    }

    // Find boxing format by its name
    config_structure* boxing_format = find_boxing_format(format_name);

    // Check the boxing format
    if (boxing_format == NULL)
    {
        DLOG_INFO1("Unsupported boxing format: %s\n", format_name);
        return NULL;
    }

    // Populate boxing format configuration
    // Create an instance of a configuration
    boxing_config * config = boxing_config_create_from_structure(boxing_format);
    if (!config)
    {
        DLOG_INFO("Failed to setting the config data from source!\n");
    }

    return config;
}


//----------------------------------------------------------------------------
/*!
*  \brief Get boxing format count.
*
*  The function provides the number of boxing formats.
*
*  \ingroup testutils
*  \return the number of boxing formats.
*/

int boxing_get_format_count()
{
    return CONFIG_ARRAY_SIZE(boxing_formats);
}


//----------------------------------------------------------------------------
/*!
*  \brief The function provides the name of boxing format.
*
*  The function provides the name of boxing format by its number in the boxing formats array.
*
*  \ingroup testutils
*  \return the name of boxing format.
*/

const char* boxing_get_configuration_name(int index)
{
    return get_format_name(boxing_formats[index]);
}


//----------------------------------------------------------------------------
/*!
 *  \brief Get control frame count.
 *
 *  The function provides the number of control frame formats.
 *
 *  \ingroup testutils
 *  \return the number of control frame formats.
 */

int boxing_get_control_frame_format_count()
{
    return CONFIG_ARRAY_SIZE(boxing_control_frame_formats);
}


//----------------------------------------------------------------------------
/*!
 *  \brief Get control frame format name
 *
 *  The function provides the name of control frame format by its number in the control frame formats array.
 *
 *  \ingroup testutils
 *  \return the name of control frame format.
 */

const char* boxing_get_control_frame_configuration_name(int index)
{
    return get_format_name(boxing_control_frame_formats[index]);
}


// PRIVATE BOXING CONFIG FUNCTIONS
//

//---------------------------------------------------------------------------- 
/*! \ingroup testutils
 *
 *  Searching the boxing format by its name.
 *
 *  \param[in] boxing_format_name   Name of the boxing format.
 *  \param[in] is_control_frame     Sign of the control frame format.
 *  \return pointer to the boxing format config_structure or NULL if the format with the specified name was not found.
 */

static config_structure* find_boxing_format(const char* boxing_format_name)
{
    unsigned int count = CONFIG_ARRAY_SIZE(boxing_formats); 

    // Looking for formats for data frames
    for (unsigned int i = 0; i < count; i++)
    {
        const char* format_name = get_format_name(boxing_formats[i]);
        if (format_name && strcmp(format_name, boxing_format_name) == 0)
        {
            return boxing_formats[i];
        }
    }

    count = CONFIG_ARRAY_SIZE(boxing_control_frame_formats);

    // Looking for formats for control frames
    for (unsigned int i = 0; i < count; i++)
    {
        const char* format_name = get_format_name(boxing_control_frame_formats[i]);
        if (format_name && strcmp(format_name, boxing_format_name) == 0)
        {
            return boxing_control_frame_formats[i];
        }
    }

    return NULL;
}


//---------------------------------------------------------------------------- 
/*! \ingroup testutils
 *
 *  Get format name.
 *
 *  \param[in] config   Input configuration structure.
 *  \return name of the input configuration structure or NULL if the name does not exist.
 */

static const char* get_format_name(config_structure* config)
{
    config_class * temp_classes = config->classes;
    for (unsigned int i = 0; i < config->count; i++)
    {
        if (strcmp("FormatInfo", temp_classes->name) == 0)
        {
            config_pair* temp_pairs = temp_classes->pairs;
            for (unsigned int j = 0; j < temp_classes->count; j++)
            {
                if (strcmp("name", temp_pairs->key) == 0)
                {
                    return temp_pairs->value;
                }
                temp_pairs++;
            }
        }
        temp_classes++;
    }

    return NULL;
}
