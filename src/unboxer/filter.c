/*****************************************************************************
**
**  Implementation of the filter interface
**
**  Creation date:  2014/01/03
**  Created by:     Ole Liabo
**
**
**  Copyright (c) 2014 Piql AS. All rights reserved.
**
**  This file is part of the boxing library
**
*****************************************************************************/

//  PROJECT INCLUDES
//
#include    "boxing/filter.h"
#include    "boxing/platform/platform.h"

//  PRIVATE INTERFACE
//
static void boxing_filter_coefficients_init( boxing_filter_coeff_2d * filter_coeff, int rows, int cols, boxing_float *coefficients );


/*! 
  * \addtogroup unboxer
  * \{
  */


//----------------------------------------------------------------------------
/*!
 *  \enum       boxing_filter_callback_result filter.h
 *  \brief      Types of the filter callback result.
 *  
 *  \param BOXING_FILTER_CALLBACK_OK     (0) OK.
 *  \param BOXING_FILTER_CALLBACK_ERROR  (1) ERROR.
 *  \param BOXING_FILTER_CALLBACK_ABORT  (2) ABORT.
 *
 *  The enumeration for specifying the type of filter callback result.
 */


/*! 
 *  \var typedef int (*boxing_filter_callback)(void * user, boxing_image8 * image, const boxing_float* coeff, int filter_size)
 *  \brief Boxing filter callback function.
 *   
 *  Type definitions for specifying boxing filter callback function.
 */


//----------------------------------------------------------------------------
/*!
 *  \struct     boxing_filter_coeff_2d_s  unboxer.h
 *  \brief      Filter coefficients storage.
 *
 *  \param rows   Number of rows
 *  \param cols   Number of columns
 *  \param coeff  Pointer to the coefficients array
 *
 *  Structure storage of filter coefficients in the array with a specified size.
 */


//----------------------------------------------------------------------------
/*!
 *  \struct       boxing_filter_s  unboxer.h
 *  \brief        Data storage for the filter.
 *
 *  \param process  Callback function for the filter
 *  \param coeff    Filter coefficients
 *
 *  The structure of the filter data storage: a pointer to callback function and filter coefficients.
 */


// PUBLIC FILTER FUNCTIONS
//

//---------------------------------------------------------------------------- 
/*! \brief Initialize 5x5 filter.
 * 
 *  Initialize filter with default sharpness filter. Note that the filter 
 *  coefficients are reader dependant and should be overridden with coefficients
 *  matching the reader image capture device.
 *
 *  \param[in]  filter      Filter instance.
 */

void boxing_filter_init( boxing_filter *filter )
{
    static boxing_float default_filter_coefficients[] =
    {
        -0.085390f,  0.188372f, -0.363868f,  0.188372f, -0.085390f,
         0.188372f, -0.499956f,  0.441212f, -0.499956f,  0.188372f,
        -0.363868f,  0.441212f,  1.500000f,  0.441212f, -0.363868f,
         0.188372f, -0.499956f,  0.441212f, -0.499956f,  0.188372f,
        -0.085390f,  0.188372f, -0.363868f,  0.188372f, -0.085390f 
    };

    filter->coeff = malloc(sizeof(boxing_filter_coeff_2d));
    boxing_filter_coefficients_init( filter->coeff, 5, 5, default_filter_coefficients );
    filter->process = NULL;
    filter->scale = 1.0;
}


//---------------------------------------------------------------------------- 
/*! \brief Free filter data.
 * 
 *  Free data owned by the filter.
 *
 *  \param[in]  filter  Filter instance.
 */

void boxing_filter_free( boxing_filter *filter )
{
    free(filter->coeff);
}


//----------------------------------------------------------------------------
/*!
 * \} end of unboxer group
 */


// PRIVATE FILTER FUNCTIONS
//

static void  boxing_filter_coefficients_init( boxing_filter_coeff_2d* coeff, int rows, int columns, boxing_float *coefficients )
{
    coeff->rows = rows;
    coeff->cols = columns;
    coeff->coeff = coefficients;
}
