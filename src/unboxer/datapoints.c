/*****************************************************************************
**
**  Implementation of the datapoints interface
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
#include "datapoints.h"

//  PRIVATE INTERFACE
//

static int quantisize(const boxing_float * threshold, int threshold_size, int value);

// PUBLIC DATA POINTS FUNCTIONS
//


//----------------------------------------------------------------------------
/*! 
  * \addtogroup unboxer
  * \{
  */


//----------------------------------------------------------------------------
/*!
 *  \brief Quantize datapoints.
 *
 *  Boxing_datapoints_quantize divides the input image into sub images of blocks size, block_width and block_height.
 *  Then it will generate a local histograms for each set of adjoining blocks and use this histogram to
 *  run it through a k-means clustering algorithm to determine which bin to stor each induvidual pixel into.
 *
 *  \param[in]  image           Image to be analysed.
 *  \param[in]  block_width     Sub image width.
 *  \param[out] block_height    Sub image height.
 *  \param[out] bins            The number of symols being searched for in the local histograms.
 *  \return The output vector contains the bin index (e.g. 0,1,2,3 for 2bit) , and NOT the actual cluster levels
 */

gvector * boxing_datapoints_quantize(const boxing_image8 * image, int block_width, int block_height, int bins)
{
    // theshold is a (cluster_count-1) x M x N matrix
    boxing_matrix_float * thresholds = boxing_calculate_thresholds(image, block_width, block_height, bins);

    gvector * data = gvector_create(1, image->width * image->height);

    boxing_pointi block_size = { (int)ceilf(image->width / (boxing_float)thresholds->rows), (int)ceilf(image->height / (boxing_float)thresholds->pages) };
    int width = image->width;

    for (unsigned int i = 0; i < image->height; i++)
    {
        const boxing_image8_pixel* pixel_data = image->data + i * image->width;
        boxing_image8_pixel *byte_array = (boxing_image8_pixel *)data->buffer + (i*width);
        const boxing_float * threshold = MATRIX_MULTIPAGE_PAGE_PTR(thresholds, i / block_size.y);

        for (int j = 0; j < width; j++)
        {
            *byte_array = quantisize(threshold + (j / block_size.x) * thresholds->cols, thresholds->cols, *pixel_data);
            byte_array++;
            pixel_data++;
        }
    }
    
    boxing_matrix_float_free(thresholds);
    return data;
}


//----------------------------------------------------------------------------
/*!
  * \} end of unboxer group
  */


// PRIVATE DATA POINTS FUNCTIONS
//

static int quantisize(const boxing_float * threshold, int threshold_size, int value)
{
    int i = 0;
    for (; i < threshold_size; i++)
    {
        if (value < threshold[i])
        {
            return i;
        }
    }
    return i;
}
