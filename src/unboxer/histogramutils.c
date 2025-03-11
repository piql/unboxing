/*****************************************************************************
**
**  Implementation of the histogram utils interface
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
#include "boxing/unboxer/histogramutils.h"
#include "boxing/platform/platform.h"
#include "boxing/math/math.h"


/*! 
  * \addtogroup unboxer
  * \{
  */


//----------------------------------------------------------------------------
/*!
 *  \def BOXING_HISTOGRAM_SIZE() histogramutils.h
 *  \brief A macro for value of histogram size.
 *
 *  A macro for value of MODULATION_PAM6histogram size.
 *  Corresponds to a macro BOXING_PIXEL_LEVELS.
 */


/*! 
 *  \typedef int* boxing_histogram
 *  \brief Integer array.
 *   
 *  Description.
 */


// PUBLIC HISTOGRAM UTILS FUNCTIONS
//

//----------------------------------------------------------------------------
/*!
 *  \brief Create a boxing_histogram instance.
 *
 *  Allocate memory for the boxing_histogram type (integers array).
 *  Return instance of allocated array.
 *
 *  \return instance of allocated boxing_histogram array.
 */

boxing_histogram boxing_histogram_create()
{
    boxing_histogram utils = calloc(BOXING_HISTOGRAM_SIZE, sizeof(int));
    memset(utils, 0, sizeof(int) * BOXING_HISTOGRAM_SIZE);
    return utils;
}


//----------------------------------------------------------------------------
/*!
 *  \brief Frees occupied memory of boxing_histogram type.
 *
 *  Frees occupied memory of boxing_histogram type.
 *
 *  \param[in]  histoutils  Histogram integer array.
 */

void boxing_histogram_free(boxing_histogram histoutils)
{
    free(histoutils);
}


//----------------------------------------------------------------------------
/*!
 *  \brief Reset boxing_histogram array.
 *
 *  Reset boxing_histogram array to zero values.
 *
 *  \param[in]  utils  Histogram integer array.
 */

void   boxing_histogram_reset(boxing_histogram utils)
{
    memset(utils, 0, BOXING_HISTOGRAM_SIZE * sizeof(int));
}


//----------------------------------------------------------------------------
/*!
 *  \brief Calculate histogram.
 *
 *  Calculate histogram function description.
 *
 *  \param[in]  utils      Histogram integer array.
 *  \param[in]  image      Image.
 *  \param[in]  point      Point.
 *  \param[in]  dimension  Dimension.
 */

void   boxing_histogram_calc_hist(boxing_histogram utils, const boxing_image8 * image, const boxing_pointi * point, const boxing_pointi * dimension)
{
    boxing_histogram_reset(utils);

    int y_end = (point->y + dimension->y) < (int)image->height ? (point->y + dimension->y) : (int)image->height;
    int x_end = (point->x + dimension->x) < (int)image->width ? (point->x + dimension->x) : (int)image->width;

    for (int y = point->y; y < y_end; y++)
    {
        const boxing_image8_pixel *scan_line = IMAGE8_PPIXEL(image, point->x, y);
        for (int x = point->x; x < x_end; x++)
        {
            utils[*scan_line++]++;
        }
    }
}


//----------------------------------------------------------------------------
/*!
 *  \brief Equalize histogram.
 *
 *  Equalize histogram function description.
 *
 *  \param[in]  histogram   Histogram integer array.
 *  \param[in]  lut         Lut.
 *  \param[in]  saturation  Saturation.
 */

void   boxing_histogram_equalize(boxing_histogram histogram, int * lut, boxing_float saturation)
{
    int min = BOXING_PIXEL_MIN;
    int max = BOXING_PIXEL_MAX;

    boxing_histogram_calc_limits(histogram, &min, &max, saturation);

    int index = 0;
    for(;index < min; index++)
    {
        lut[index] = BOXING_PIXEL_MIN;
    }

    for(; index < max + 1; index++)
    {
        lut[index] = (int)floorf(BOXING_PIXEL_MAX*(boxing_float)(index - min)/(boxing_float)(max - min) + 0.5f);
    }

    for(;index < BOXING_HISTOGRAM_SIZE; index++)
    {
        lut[index] = BOXING_PIXEL_MAX;
    }
}


//----------------------------------------------------------------------------
/*!
 *  \brief Histogram calculate limits.
 *
 *  Histogram calculate limits function description.
 *
 *  \param[in]  histogram   Histogram integer array.
 *  \param[in]  min         Minimum.
 *  \param[in]  max         Maximum.
 *  \param[in]  saturation  Saturation.
 */

void   boxing_histogram_calc_limits( boxing_histogram histogram, int * min, int * max, boxing_float saturation/* = 0.0*/)
{
    *min = BOXING_PIXEL_MIN;
    *max = BOXING_PIXEL_MAX;

    boxing_float total = 0.0f;
    boxing_float saturation_level;
    for(int unsigned i = 0; i < BOXING_HISTOGRAM_SIZE; i++)
    {
        total += histogram[i];
    }

    saturation_level = 0.0f;
    for(int unsigned i = 0; i < BOXING_HISTOGRAM_SIZE; i++)
    {
        saturation_level += histogram[i];
        if(saturation_level > (total * saturation))
        {
            *min = i;
            break;
        }
    }

    saturation_level = 0.0f;
    for(int i = BOXING_HISTOGRAM_SIZE-1; i > -1; i--)
    {
        saturation_level += histogram[i];
        if(saturation_level > (total * saturation))
        {
            *max = i;
            break;
        }
    }
}


//----------------------------------------------------------------------------
/*!
 *  \brief Get the average level in the histogram.
 *
 *  Get the average between the minimum and the maximum level in the histogram.
 *
 *  \param[in]  histogram  Histogram integer array.
 *  \return Histogram measure.
 */

int    boxing_histogram_calc_measure(boxing_histogram histogram)
{
    int minimum = -1;
    int maximum = -1;

    for (int i = 0; i<BOXING_HISTOGRAM_SIZE && minimum == -1; i++)
    {
        if (histogram[i] > 0)
        {
            minimum = i;
        }
    }

    for (int i = BOXING_HISTOGRAM_SIZE-1; i>-1 && maximum == -1; i--)
    {
        if (histogram[i] > 0) 
        {
            maximum = i;
        }
    }

    return (minimum + maximum)/2;
}


//----------------------------------------------------------------------------
/*!
 *  \brief Get the minimum and maximum values in the histogram.
 *
 *  Get the minimum and maximum values in the histogram.
 *
 *  \param[in]  histogram  Histogram integer array.
 *  \param[out] minimum    Minimum value in the histogram integer array.
 *  \param[out] maximum    Maximum value in the histogram integer array.
 *  \return Histogram minimum and maximum values.
 */

int boxing_histogram_calc_measure_minmax(boxing_histogram histogram, int * minimum, int * maximum)
{
    *minimum = -1;
    *maximum = -1;
    for ( int i = 0; i < BOXING_HISTOGRAM_SIZE; i++ )
    {
        if ( histogram[i] > 0 )
        {
            *minimum = i;
            break;
        }
    }

    for ( int i = BOXING_HISTOGRAM_SIZE - 1; i > -1; i-- )
    {
        if ( histogram[i] > 0 )
        {
            *maximum = i;
            break;
        }
    }

    return (*minimum + *maximum)/2;
}


//----------------------------------------------------------------------------
/*!
 *  \brief Calculate full histogram measure.
 *
 *  Calculate full histogram measure.
 *
 *  \param[in]  utils      Histogram integer array.
 *  \param[in]  image      Image.
 *  \param[in]  point      Point.
 *  \param[in]  dimension  Dimension.
 *  \param[out] minimum    Minimum value in the histogram integer array.
 *  \param[out] maximum    Maximum value in the histogram integer array.
 *  \return Full histogram measure.
 */

int boxing_histogram_calc_measure_full(boxing_histogram utils, const boxing_image8 * image, boxing_pointi * point, boxing_pointi * dimension, int * minimum, int * maximum)
{
    boxing_histogram_calc_measure_image(utils, image, point, dimension);
    return boxing_histogram_calc_measure_minmax(utils, minimum, maximum);
}


//----------------------------------------------------------------------------
/*!
 *  \brief Calculate image measure.
 *
 *  Calculate image measure.
 *
 *  \param[in]  utils      Histogram integer array.
 *  \param[in]  image      Image.
 *  \param[in]  point      Point.
 *  \param[in]  dimension  Dimension.
 *  \return Image measure.
 */

int boxing_histogram_calc_measure_image(boxing_histogram utils, const boxing_image8 * image, boxing_pointi *point, boxing_pointi *dimension)
{
    boxing_histogram_reset(utils);
    boxing_histogram_calc_hist(utils, image, point, dimension);
    return boxing_histogram_calc_measure(utils);
}


//----------------------------------------------------------------------------
/*!
 *  \brief Calculate average value of the histogram.
 *
 *  Calculate average value of the histogram.
 *
 *  \param[in]  histogram      Histogram integer array.
 *  \return Average value.
 */

boxing_float boxing_histogram_calc_average(boxing_histogram histogram)
{
    int element_in_sample = 0;
    int accumulate_ample = 0;
    for (int i = 0; i < BOXING_HISTOGRAM_SIZE; i++)
    {
        accumulate_ample += (histogram[i] * i);
        element_in_sample += histogram[i];
    }
    return (boxing_float)accumulate_ample / (boxing_float)element_in_sample;
}


//----------------------------------------------------------------------------
/*!
 *  \brief Calculate average value of the image.
 *
 *  Calculate average value of the image.
 *
 *  \param[in]  image      Image.
 *  \param[in]  point      Point.
 *  \param[in]  dimension  Dimension.
 *  \return Average value.
 */

boxing_float boxing_histogram_calc_average_image(const boxing_image8 * image, boxing_pointi * point, boxing_pointi * dimension)
{
    long long sum = 0;
    int y_end = (point->y + dimension->y) < (int)image->height ? (point->y + dimension->y) : (int)image->height;
    int x_end = (point->x + dimension->x) < (int)image->width ? (point->x + dimension->x) : (int)image->width;

    for (int y = point->y; y < y_end; y++)
    {
        const boxing_image8_pixel *scan_line = IMAGE8_PPIXEL(image, point->x, y);//image.data(point->x,y);
        for (int x = point->x; x < x_end; x++)
        {
            sum += *scan_line++;
        }
    }

    return sum / (boxing_float)(dimension->x*dimension->y);
}


//----------------------------------------------------------------------------
/*!
  * \} end of unboxer group
  */
