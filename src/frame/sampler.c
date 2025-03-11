/*****************************************************************************
**
**  Implementation of the boxing sampler interface
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
#include    "boxing/unboxer/sampler.h"
#include    "boxing/platform/platform.h"

//  CONSTANTS
//

const char * BOXING_SAMPLE_CONTAINER_CONTENT  = "sampler.container.content";
const char * BOXING_SAMPLE_CONTAINER_METADATA = "sampler.container.metadata";
const char * BOXING_SAMPLE_CORNERMARKS        = "sampler.referencemarks";
const char * BOXING_SAMPLE_BAR_REFERENCE      = "sampler.bar.reference";
const char * BOXING_SAMPLE_BAR_CALIBRATION    = "sampler.bar.calibration";
const char * BOXING_SAMPLE_HORIZONTALSHIFTS   = "sampler.horizontalshifts";


/*! 
  * \addtogroup frame
  * \{
  */


//----------------------------------------------------------------------------
/*!
 *  \var    BOXING_SAMPLE_CONTAINER_CONTENT
 *  \brief  Sample container content string.
 */


//----------------------------------------------------------------------------
/*!
 *  \var    BOXING_SAMPLE_CONTAINER_METADATA
 *  \brief  Sample container metadata string.
 */


//----------------------------------------------------------------------------
/*!
 *  \var    BOXING_SAMPLE_CORNERMARKS
 *  \brief  Sample corner marks string.
 */


//----------------------------------------------------------------------------
/*!
 *  \var    BOXING_SAMPLE_BAR_REFERENCE
 *  \brief  Sample reference bar string.
 */


//----------------------------------------------------------------------------
/*!
 *  \var    BOXING_SAMPLE_BAR_CALIBRATION
 *  \brief  Sample calibration bar string.
 */


//----------------------------------------------------------------------------
/*!
 *  \var    BOXING_SAMPLE_HORIZONTALSHIFTS
 *  \brief  Sample horizontal shifts string.
 */


//----------------------------------------------------------------------------
/*!
 *  \struct     boxing_sampler_s  sampler.h
 *  \brief      Sampler data storage.
 *  
 *  \param sample           Sample function. (boxing_image8 * (*sample)(struct boxing_sampler_s * sampler, const boxing_image8 * frame))
 *  \param free             Function to free memory. (void (*free)(struct boxing_sampler_s * sampler))
 *  \param location_matrix  Location matrix.
 *  \param state            Sampler state.
 *
 *  Sampler data storage structure description.
 */


// PUBLIC SAMPLER FUNCTIONS
//

//----------------------------------------------------------------------------
/*!
 *  \brief Initializes all boxing_sampler structure data.
 *
 *  Initializes location matrix width and height with input values and other data with default values.
 *
 *  \param[in] sampler  Pointer to the boxing_sampler structure.
 *  \param[in] width    Location matrix width.
 *  \param[in] height   Location matrix height.
 */

void boxing_sampler_init(boxing_sampler * sampler, int width, int height)
{
    sampler->state = DFALSE;
    boxing_matrixf_init_in_place(&sampler->location_matrix, width, height);
    sampler->free = boxing_sampler_free;
    sampler->sample = NULL;
}


//----------------------------------------------------------------------------
/*!
 *  \brief Frees occupied memory of sampler location matrix.
 *
 *  Frees occupied memory of sampler location matrix. Does not free the original pointer.
 *
 *  \param[in]  sampler  Pointer to the boxing_sampler structure.
 */

void boxing_sampler_free(boxing_sampler * sampler)
{
    if (sampler->location_matrix.is_owning_data)
    {
        free(sampler->location_matrix.data);
        sampler->location_matrix.data = NULL;
    }
}


//----------------------------------------------------------------------------
/*!
 *  \brief Frees occupied memory of boxing_sampler structure.
 *
 *  Frees occupied memory of all internal structure pointers and structure pointer.
 *  Frees the original pointer. Ascendant struct could be passed.
 *
 *  \param[in]  sampler  Pointer to the boxing_sampler structure.
 */

void boxing_sampler_destroy(boxing_sampler * sampler)
{
    if (sampler)
    {
        sampler->free(sampler);
        free(sampler);
    }
}


//----------------------------------------------------------------------------
/*!
  * \} end of frame group
  */
