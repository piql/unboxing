/*****************************************************************************
**
**  Implementation of the unboxing utility
**
**  Creation date:  2017/04/05
**  Created by:     Aleksandr Ivanov
**
**
**  Copyright (c) 2017 Piql AS. All rights reserved.
**
**  This file is part of the boxing library
**
*****************************************************************************/

//  PROJECT INCLUDES
//

#include "unboxingutility.h"
#include "boxing/platform/memory.h"
#include "boxing/string.h"
#include "boxing/config.h"
#include "boxing_config.h"

//----------------------------------------------------------------------------
/*!
 *  \ingroup testutils
 *  \struct  boxing_unboxer_utility_s   unboxingutility.h
 *  \brief   Utility structure to unboxing data.
 *
 *  \param unboxer               Boxer instance.
 *  \param parameters            Structure to storage unboxer parameters..
 *
 *  Structure to storage all neccessary tools to unboxing data.
 */


// PUBLIC UNBOXING UTILITY FUNCTIONS
//


//----------------------------------------------------------------------------
/*!
 *  \brief The function sets the frame format by its name.
 *
 *  The function sets the frame format by its name.
 *
 *  \ingroup testutils
 *  \param[in]   utility           Instance of boxing_unboxer_utility structure.
 *  \param[in]   format_name       Name of the format.
 *  \return TRUE on success
 */

DBOOL boxing_unboxer_set_frame_format(boxing_unboxer_utility* utility, const char* format_name)
{
    if (utility == NULL || format_name == NULL)
    {
        return DFALSE;
    }

    if (utility->parameters == NULL)
    {
        return DFALSE;
    }

    // Create an instance of the configuration
    boxing_config * config = boxing_create_boxing_config(format_name);

    // Set the format
    utility->parameters->format = config;

    return DTRUE;
}

