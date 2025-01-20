/*****************************************************************************
**
**  Implementation of the adaptive filter
**
**  Creation date:  2018/11/26
**  Created by:     Piql
**
**
**  Copyright (c) 2018 Piql AS. All rights reserved.
**
**  This file is part of the boxing library
**
*****************************************************************************/

//  PROJECT INCLUDES
//
#include "adaptive_filter.h"
#include "boxing/filter.h"
#include "boxing/viewport.h"
#include "boxing/unboxer/horizontalmeasures.h"
#include "boxing/platform/memory.h"


static void calc_signal_enegy(boxing_viewport * vp_energy, const boxing_viewport * vp, boxing_filter_coeff_2d * filter);
static void calc_filter_coeffs(boxing_viewport * vp, const boxing_viewport * vp_energy, boxing_filter_coeff_2d * filter, boxing_float * symbol_mean, int symbols);
static void quantisize(boxing_viewport * out, const boxing_viewport * in, const boxing_float * mean, int mean_size);
static void filter2DExtend(boxing_viewport * out, const boxing_viewport * in, boxing_filter_coeff_2d * coeff);

//----------------------------------------------------------------------------
/*!
 *  \defgroup   adaptive_filter  AdaptiveFilter
 *  \brief      Two dimensional view for accessing any type of two dimensional data.
 *  \ingroup    unbox
 *
 *  The viewport is a tool for viewing any arbitrary sections of data
 *  within a two dimensional buffer. The buffer it self is organized as 1 dimensional array
 *  of elements listed in linear memory space.
 *
 */


/*! 
  * \addtogroup viewport
  * \{
  */


// PUBLIC VIEWPORT FUNCTIONS
//

//----------------------------------------------------------------------------
/*!
 *  \brief Equalize a downsampled data frame.
 *
 *  The adaptive filter equalizes a downsampled frame. The agorithm is based upon a generic
 *  adaptive filter design using LMS to calculate the filter coefficients.
 *  The original input signal commprises a set of symbols represented as equadistent gray values.
 *
 *  The adaptive filter also support block processing, i.e it subdevides the image into blocks that are at least
 *  block size big. 
 *
 *  \param[in]  out                  A pointer to a preallocated image where the processed reslt will be stored.
 *  \param[in]  in                   A pointer to the original image.
 *  \param[in]  symbols_per_pixel    Number of symbols to be found in the input image.
 *  \param[in]  block_width          Block width.
 *  \param[in]  block_height         Block height.
 */

void adaptive_filter(boxing_image8 * out, const boxing_image8 * in, int symbols_per_pixel, int block_width, int block_height)
{
    boxing_matrix_float * means = boxing_calculate_means(in, block_width, block_height, symbols_per_pixel);
    
    boxing_float * fimage = boxing_memory_allocate(in->width * in->height * sizeof(boxing_float));
    boxing_viewport * tmp_vp = boxing_viewport_create(fimage, in->width, in->height, in->width, sizeof(boxing_float));

    boxing_float * eimage = boxing_memory_allocate(in->width * in->height * sizeof(boxing_float));
    boxing_viewport * enegy_vp = boxing_viewport_create(eimage, in->width, in->height, in->width, sizeof(boxing_float));

    boxing_viewport * in_vp = boxing_viewport_create(in->data, in->width, in->height, in->width, 1);
    boxing_viewport * out_vp = boxing_viewport_create(out->data, out->width, out->height, out->width, 1);

    int rows = (int)floor(in->height / block_height);
    if (rows < 1)
        rows = 1;

    int cols = (int)floor(in->width / block_width);
    if (cols < 1)
        cols = 1;   

    double vstep = (double)in->height / (double)rows;
    double hstep = (double)in->width / (double)cols;

    for (int row = 0; row < rows; row++)
    {
        int y = (int)floor(row*vstep + 0.5);
        int height = (int)floor((row + 1) * vstep + 0.5) - y;

        for (int col = 0; col < cols; col++)
        {
            int x = (int)floor(col*hstep + 0.5);
            int width = (int)floor((col + 1) * hstep + 0.5) - x;
            
            boxing_viewport_reset(in_vp);
            boxing_viewport_set_view(in_vp, width, height, x, y);

            boxing_float * symbol_mean = MATRIX_MULTIPAGE_ROW_PTR(means, col, row);
            boxing_float all_pass_coeffs[25] = 
            {
                (boxing_float)0, (boxing_float)0, (boxing_float)0, (boxing_float)0, (boxing_float)0,
                (boxing_float)0, (boxing_float)0, (boxing_float)0, (boxing_float)0, (boxing_float)0,
                (boxing_float)0, (boxing_float)0, (boxing_float)1, (boxing_float)0, (boxing_float)0,
                (boxing_float)0, (boxing_float)0, (boxing_float)0, (boxing_float)0, (boxing_float)0,
                (boxing_float)0, (boxing_float)0, (boxing_float)0, (boxing_float)0, (boxing_float)0
            };
            boxing_filter_coeff_2d filter_coeffs = {5, 5, all_pass_coeffs};

            boxing_viewport_reset(enegy_vp);
            boxing_viewport_set_view(enegy_vp, width, height, x, y);
            boxing_viewport_reset(out_vp);
            boxing_viewport_set_view(out_vp, width, height, x, y);
            boxing_viewport_reset(tmp_vp);
            boxing_viewport_set_view(tmp_vp, width, height, x, y);


            // process view

            calc_signal_enegy(enegy_vp, in_vp, &filter_coeffs);

            for(int coeffLoops = 0; coeffLoops < 10; coeffLoops++)
            {
                calc_filter_coeffs(in_vp, enegy_vp, &filter_coeffs, symbol_mean, symbols_per_pixel);
            }

            filter2DExtend(tmp_vp, in_vp, &filter_coeffs);
            quantisize(out_vp, tmp_vp, symbol_mean, symbols_per_pixel);
        }
    }

    boxing_matrix_float_free(means);
    
    boxing_viewport_free(tmp_vp);
    boxing_viewport_free(enegy_vp);
    boxing_viewport_free(in_vp);
    boxing_viewport_free(out_vp);

    boxing_memory_free(fimage);
    boxing_memory_free(eimage);
}

//----------------------------------------------------------------------------
/*!
  * \} end of adaptive_filter group
  */

// PRIVATE ADAPTIVE FILTER FUNCTIONS
//

static inline int identify_symbol(boxing_float symbol, boxing_float * threshold, int thresholds)
{
    int index = 0;

    for(; index < thresholds; index++, threshold++)
    {
        if(symbol < *threshold)
            break;
    }

    return index;
}

static void calc_signal_enegy(boxing_viewport * vp_energy, const boxing_viewport * vp, boxing_filter_coeff_2d * filter)
{
    int N = (int)filter->rows; // assume rows and colls are the same
    int d = N/2;
    int width = boxing_viewport_width(vp) - 2*d;
    int height = boxing_viewport_height(vp) - 2*d;

    for (int m = d; m < height+d; m++)
    {
        boxing_float *energy_pixel = (boxing_float*)boxing_viewport_scanline(vp_energy, m) + d;
        for (int n = d; n < width+d; n++, energy_pixel++)
        {
            // calc siganl energy
            boxing_float signalEnergy = 0;
            for(int i = 0; i < N; i++)
            {
                const unsigned char *sourcePixel = (const unsigned char*)boxing_viewport_scanline(vp, m - d + i) + n - d;
                for(int j = 0; j < N; j++, sourcePixel++)
                {
                     signalEnergy += (boxing_float)(*sourcePixel)*(*sourcePixel);
                }
            }
            *energy_pixel = signalEnergy;
        }
    }
}

static void calc_filter_coeffs(boxing_viewport * vp, const boxing_viewport * vp_energy, boxing_filter_coeff_2d * filter, boxing_float * symbol_mean, int symbols)
{
//    ERROR_F("adaptiveFilter");
    int N = (int)filter->rows; // assume rows and colls are the same
    int d = N/2;
    int width = boxing_viewport_width(vp) - 2*d;
    int height = boxing_viewport_height(vp) - 2*d;

    int thresholds = symbols -1;
    boxing_float *threshold = BOXING_STACK_ALLOCATE_TYPE_ARRAY(boxing_float, thresholds);
    for(int i = 0; i < thresholds; i++ )
    {
        threshold[i] = (symbol_mean[i+1] + symbol_mean[i])/2;
    }

    boxing_float update_factor = (boxing_float)1.0/(2*N*N);

    for (int m = d; m < height+d; m++)
    {
        const boxing_float* signalEnergy = (boxing_float*)boxing_viewport_scanline(vp_energy, m) + d;
        for (int n = d; n < width+d; n++, signalEnergy++)
        {

            // calc y = w(j)*x(j)
            boxing_float estimatedValue = 0;
            boxing_float * coeff_ptr = filter->coeff;
            for(int i = 0; i < N; i++)
            {
                const unsigned char *sourcePixel = (unsigned char*)boxing_viewport_scanline(vp, m - d + i) + n - d;
                for(int j = 0; j < N; j++, sourcePixel++, coeff_ptr++)
                {
                    estimatedValue += (*coeff_ptr)*(*sourcePixel);
                }
            }

            int index = identify_symbol(estimatedValue, threshold, thresholds);

            // calc error
            boxing_float e = symbol_mean[index] - estimatedValue;

            // update filter coeffs
            coeff_ptr = filter->coeff;
            boxing_float k = (update_factor*e)/(*signalEnergy);
            for(int i = 0; i < N; i++)
            {
                const unsigned char *sourcePixel = (unsigned char*)boxing_viewport_scanline(vp, m - d + i) + n - d;
                for(int j = 0; j < N; j++, sourcePixel++, coeff_ptr++)
                {
                    (*coeff_ptr) += k*(*sourcePixel);
                }
            }
        }
    }
}


static void quantisize(boxing_viewport * out, const boxing_viewport * in, const boxing_float * mean, int mean_size)
{

    int thresholds = mean_size -1;
    boxing_float *threshold = BOXING_STACK_ALLOCATE_TYPE_ARRAY(boxing_float, thresholds);
    for(int i = 0; i < thresholds; i++ )
    {
        threshold[i] = (mean[i+1] + mean[i])/2;
    }

    int width = boxing_viewport_width(in);
    int height = boxing_viewport_height(in);

    for (int m = 0; m < height; m++)
    {
        boxing_float  *sourcePixel = (boxing_float*)boxing_viewport_scanline(in, m);
        unsigned char *destinationPixel = (unsigned char*)boxing_viewport_scanline(out, m);
        for (int n = 0; n < width; n++, sourcePixel++, destinationPixel++)
        {
            *destinationPixel = (unsigned char)identify_symbol(*sourcePixel, threshold, thresholds);
        }
    }
}


static void filter2DExtend(boxing_viewport * out, const boxing_viewport * in, boxing_filter_coeff_2d * coeff)
{
    int cRows = (int)coeff->rows;
    int cCols = (int)coeff->cols;
    int d = cRows/2;
    int x_offset = boxing_viewport_offset_x(in);
    int y_offset = boxing_viewport_offset_y(in);
    int width = boxing_viewport_width(in);
    int height = boxing_viewport_height(in);
    int buffer_width = boxing_viewport_buffer_width(in);
    int buffer_height = boxing_viewport_buffer_height(in);
    int xClampMin = x_offset > d ? -d : -x_offset;
    int yClampMin = y_offset > d ? -d : -y_offset;
    int xClampMax = buffer_width -1 - (x_offset + width) > d ? width + d -1: buffer_width -1 - (x_offset);
    int yClampMax = buffer_height -1 - (y_offset + height) > d ? height + d -1: buffer_height -1 - (y_offset);

    // resize view port to adapt to filter dimensions
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            boxing_float sum = (boxing_float)0;
            boxing_float *coeffPtr = coeff->coeff;
            for (int m = 0; m < cRows; m++)
            {
                unsigned char *srcPixel = (unsigned char *)boxing_viewport_scanline(in, BOXING_MATH_CLAMP(yClampMin, yClampMax, y - (cRows / 2) + m));
                for (int n = 0; n < cCols; n++, coeffPtr++)
                {
                    sum += (*coeffPtr) * (*(srcPixel + BOXING_MATH_CLAMP(xClampMin, xClampMax, x - (cCols / 2) + n)));
                }
            }
            *(((boxing_float *)boxing_viewport_scanline(out, y)) + x) = sum;
        }
    }
}



