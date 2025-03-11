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

#include "boxing/unboxer_utility.h"
#include "boxing/platform/platform.h"
#include "boxing/string.h"
#include "boxing/config.h"

//----------------------------------------------------------------------------
/*!
 *  \ingroup boxing
 *  \struct  boxing_unboxer_utility_s   unboxer_utility.h
 *  \brief   Utility structure to unboxing data.
 *
 *  \param unboxer               Unboxer instance.
 *  \param parameters            Unboxer settings.
 *
 *  Wrapper around an unboxer and its configuration.
 */


// PUBLIC UNBOXING UTILITY FUNCTIONS
//


//----------------------------------------------------------------------------
/*!
 *  \brief Create the unboxer utility
 *
 *  \ingroup testutils
 *  \param[in]   format                Boxing format.
 *  \param[in]   is_raw                A sign that the input data is generated.
 *  \param[in]   on_all_complete       Unboxing complete callback (requires BOXINGLIB_CALLBACK define)
 *  \param[in]   on_metadata_complete  Metadata complete callback (requires BOXINGLIB_CALLBACK define)
 *  \return instance of boxing_unboxer_utility or NULL.
 */

boxing_unboxer_utility *boxing_unboxer_utility_create(
    boxing_config *format,
    DBOOL is_raw
#ifdef BOXINGLIB_CALLBACK
    ,boxing_all_complete_cb on_all_complete, boxing_metadata_complete_cb  on_metadata_complete
#endif
    )
{
    // Check the input data
    if (format == NULL)
    {
        return NULL;
    }

    // Create an instance of the object
    boxing_unboxer_utility *utility = malloc(sizeof(boxing_unboxer_utility));
    utility->unboxer = NULL;
    utility->parameters = NULL;

    // Create structure of boxing configuration parameters
    utility->parameters = malloc(sizeof(boxing_unboxer_parameters));
    boxing_unboxer_parameters_init(utility->parameters);
    memset(utility->parameters, 0, sizeof utility->parameters);

    // Set the format
    utility->parameters->format = format;

    // Set the parameters
    utility->parameters->is_raw = is_raw;
#ifdef BOXINGLIB_CALLBACK
    utility->parameters->on_all_complete = on_all_complete;
    utility->parameters->on_metadata_complete = on_metadata_complete;
#endif

    // Create the unboxer structure
    utility->unboxer = boxing_unboxer_create(utility->parameters);

    return utility;
}


//----------------------------------------------------------------------------
/*!
 *  \brief Unbox data
 *
 *  Processes the input image and returns a array of decoded data on success.
 *
 *  \ingroup testutils
 *  \param[in]   unboxer_utility  Unboxing utility instance.
 *  \param[in]   input_image      Input image instance.
 *  \param[out]  output_data      Output source file data.
 *  \return process result.
 */

int boxing_unboxer_utility_unbox(boxing_unboxer_utility *unboxer_utility, boxing_image8 *input_image, gvector *output_data)
{
    if (unboxer_utility == NULL || input_image == NULL)
    {
        return BOXING_UNBOXER_INPUT_DATA_ERROR;
    }

    boxing_metadata_list *metadata = boxing_metadata_list_create();
    
    int extract_result;
    int process_result = boxing_unboxer_unbox(output_data, metadata, input_image, unboxer_utility->unboxer, &extract_result, output_data, BOXING_METADATA_CONTENT_TYPES_DATA);
 
    boxing_metadata_list_free(metadata);

    return process_result;
}


//----------------------------------------------------------------------------
/*!
 *  \brief Frees unboxer utility
 *
 *  \ingroup testutils
 *  \param[in]   unboxer_utility  Unboxing utility instance.
 */

void boxing_unboxer_utility_free(boxing_unboxer_utility *unboxer_utility)
{
    if (unboxer_utility != NULL)
    {
        boxing_unboxer_free(unboxer_utility->unboxer);
        boxing_unboxer_parameters_free(unboxer_utility->parameters);
        free(unboxer_utility->parameters);
    }

    free(unboxer_utility);
}
