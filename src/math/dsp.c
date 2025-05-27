/*****************************************************************************
**
**  Implementation of the digital signal processing interface
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
#include "boxing/math/math.h"
#include "boxing/math/dsp.h"
#include "boxing/platform/platform.h"

//  PRIVATE INTERFACE
//

static boxing_float sine_cardinal(boxing_float x);

// PUBLIC DSP FUNCTIONS
//

/*! 
  * \addtogroup math
  * \{
  */


//----------------------------------------------------------------------------
/*!
 *  \brief Dsp low pass filter.
 *
 *  Dsp low pass filter
 *
 *  \param[in]  cut_off_frequency  Cut off frequency.
 *  \param[in]  sampling_rate      Sampling rate.
 *  \param[out] coeffs             Coefficients.
 *  \param[in]  length             Length.
 */

void boxing_dsp_low_pass_filter(const boxing_float cut_off_frequency, const boxing_float sampling_rate, boxing_float * coeffs, int length)
{
    assert(length % 2 != 0);

    boxing_float low_cut_off_frequency = 2.0f*(cut_off_frequency);
    boxing_float * window_coefficients = calloc(length, sizeof(boxing_float));
    boxing_dsp_blackman_window(window_coefficients, length);

    boxing_float * coeff = coeffs;
    boxing_float * coeffs_end = coeffs + length;
    boxing_float * window_coefficient = window_coefficients;

    boxing_float x = (boxing_float)(-length / 2);

    while (coeff != coeffs_end)
    {
        *coeff = (low_cut_off_frequency*sine_cardinal((x / sampling_rate) * (boxing_float)BOXING_MATH_PI * low_cut_off_frequency)) * (*window_coefficient);
        x++;
        coeff++;
        window_coefficient++;
    }
    free(window_coefficients);
}


//----------------------------------------------------------------------------
/*!
 *  \brief Dsp high pass filter.
 *
 *  Dsp high pass filter
 *
 *  \param[in]  cut_off_frequency  Cut off frequency.
 *  \param[in]  sampling_rate      Sampling rate.
 *  \param[out] coeffs             Coefficients.
 *  \param[in]  length             Length.
 */

void boxing_dsp_high_pass_filter(const boxing_float cut_off_frequency, const boxing_float sampling_rate, boxing_float * coeffs, int length)
{
    assert(length % 2 != 0);

    boxing_float high_cut_off_frequency = 2.0f*(cut_off_frequency);
    boxing_float * window_coefficients = calloc(length, sizeof(boxing_float));
    boxing_dsp_blackman_window(window_coefficients, length);

    boxing_float * coeff = coeffs;
    boxing_float * coeffs_end = coeffs + length;
    boxing_float * window_coefficient = window_coefficients;

    boxing_float x = (boxing_float)(-length / 2);
    
    while(coeff != coeffs_end)
    {
        
        *coeff = -(high_cut_off_frequency*sine_cardinal((x/sampling_rate) * (boxing_float)BOXING_MATH_PI * high_cut_off_frequency)) * (*window_coefficient);
        if(x == 0)
        {
            *coeff += 1;
        }
        x++;
        coeff++;
        window_coefficient++;
    }
    free(window_coefficients);
}


//----------------------------------------------------------------------------
/*!
 *  \brief Dsp band pass filter.
 *
 *  Dsp band pass filter
 *
 *  \param[in]  center_frequency  Center frequency.
 *  \param[in]  bandwidth         Band width.
 *  \param[in]  sampling_rate     Sampling rate.
 *  \param[out] coeffs            Coefficients.
 *  \param[in]  length            Length.
 */

void boxing_dsp_band_pass_filter(const boxing_float center_frequency, const boxing_float bandwidth, const boxing_float sampling_rate, boxing_float * coeffs, int length)
{
    assert(length % 2 != 0);

    boxing_float low_cut_off_frequency = 2.0f*(center_frequency - bandwidth / 2.0f);
    boxing_float high_cut_off_frequency = 2.0f*(center_frequency + bandwidth / 2.0f);
    boxing_float * window_coefficients = calloc(length, sizeof(boxing_float));
    boxing_dsp_blackman_window(window_coefficients, length);

    boxing_float * coeff = coeffs;
    boxing_float * coeffs_end = coeffs + length;
    boxing_float * window_coefficient = window_coefficients;

    boxing_float x = (boxing_float)(-length / 2);
    
    while(coeff != coeffs_end)
    {
        
        *coeff = (high_cut_off_frequency*sine_cardinal((x / sampling_rate)*(boxing_float)BOXING_MATH_PI*high_cut_off_frequency) -
                   low_cut_off_frequency*sine_cardinal((x / sampling_rate)*(boxing_float)BOXING_MATH_PI*low_cut_off_frequency)) *
                   *window_coefficient;
                    

        x++;
        coeff++;
        window_coefficient++;
    }
    free(window_coefficients);
}


//----------------------------------------------------------------------------
/*!
 *  \brief Dsp filter for the float data.
 *
 *  Dsp filter for the float data.
 *
 *  \param[in]  in                   Input data pointer.
 *  \param[in]  in_size              Input data size.
 *  \param[out] out                  Output data pointer.
 *  \param[in]  filter_coefficients  Filter coefficients.
 *  \param[in]  size                 Size.
 *  \param[in]  preload              Preload sign.
 */

void boxing_dsp_filter(const boxing_float *in, unsigned int in_size, boxing_float *out, const boxing_float *filter_coefficients, int size, DBOOL preload /*  = DFALSE */)
{
    boxing_float *m = calloc(size, sizeof(boxing_float));
    memset(m, 0, sizeof(boxing_float) * size);
    
    int index = size-1;

    if(preload)
    {
        for(int n = 0; n < size; n++)
        {
            m[index] = (boxing_float)in[0];
            //update index
            index = (index + size -1 ) % size;
        }
    }

    for(int n = 0; n < (int)in_size; n++)
    {
        m[index] = in[n];
        boxing_float y = 0.0f;
        for(int i = 0; i < size; i++)
        {   
            y += m[(index+i)%size]*filter_coefficients[i];
        }

        //update index
        index = (index + size -1 ) % size;
        out[n] = y;
    }
    free(m);
}


//----------------------------------------------------------------------------
/*!
 *  \brief Dsp filter for the integer data.
 *
 *  Dsp filter for the integer data.
 *
 *  \param[in]  in                   Input data pointer.
 *  \param[in]  in_size              Input data size.
 *  \param[out] out                  Output data pointer.
 *  \param[in]  filter_coefficients  Filter coefficients.
 *  \param[in]  size                 Size.
 *  \param[in]  preload              Preload sign.
 */

void boxing_dsp_filter_int(const int *in, unsigned int in_size, int *out, const boxing_float *filter_coefficients, int size, DBOOL preload /*  = DFALSE */)
{
    boxing_float *m = calloc(size, sizeof(boxing_float));
    memset(m, 0, sizeof(boxing_float) * size);

    int index = size - 1;

    if (preload)
    {
        for (int n = 0; n < size; n++)
        {
            m[index] = (boxing_float)in[0];
            //update index
            index = (index + size - 1) % size;
        }
    }

    for (int n = 0; n < (int)in_size; n++)
    {
        m[index] = (boxing_float)in[n];
        boxing_float y = 0.0f;
        for (int i = 0; i < size; i++)
        {
            y += m[(index + i) % size] * filter_coefficients[i];
        }

        //update index
        index = (index + size - 1) % size;
        out[n] = (int)y;
    }
    free(m);
}


//----------------------------------------------------------------------------
/*!
 *  \brief Dsp filter for the float data.
 *
 *  Dsp filter for the float data.
 *
 *  \param[in]  in                   Input data pointer.
 *  \param[in]  in_size              Input data size.
 *  \param[out] out                  Output data pointer.
 *  \param[in]  out_size             Output data size.
 *  \param[in]  filter_coefficients  Filter coefficients.
 *  \param[in]  size                 Size
 */

void boxing_dsp_filtfilt(const boxing_float *in, unsigned int in_size, boxing_float *out, unsigned int out_size, const boxing_float *filter_coefficients, unsigned int size)
{
    unsigned int buffer_size = (unsigned int)(in_size + size*1.5f + 1);
    boxing_float *buffer = calloc(buffer_size, sizeof(boxing_float));
    unsigned int i = 0;
    for (; i < size; i++)
    {
        buffer[i] = in[0];
    }

    for (; i < size + in_size; i++)
    {
        buffer[i] = in[i - size];
    }

    for (; i < buffer_size; i++)
    {
        buffer[i] = in[in_size - 1];
    }

    boxing_dsp_filter(buffer, buffer_size, buffer, filter_coefficients, size, DFALSE);

    for (i = 0; i < out_size; i++)
    {
        out[i] = buffer[i + (int)(size*1.5f)];
    }
    free(buffer);
}


//----------------------------------------------------------------------------
/*!
 *  \brief Dsp filter for the integer data.
 *
 *  Dsp filter for the integer data.
 *
 *  \param[in]  in                   Input data pointer.
 *  \param[in]  in_size              Input data size.
 *  \param[out] out                  Output data pointer.
 *  \param[in]  out_size             Output data size.
 *  \param[in]  filter_coefficients  Filter coefficients.
 *  \param[in]  size                 Size
 */

void boxing_dsp_filtfilt_int(const int * in, unsigned int in_size, boxing_float * out, unsigned int out_size, const boxing_float * filter_coefficients, unsigned int size)
{
    unsigned int buffer_size = (unsigned int)(in_size + size*1.5f + 1);
    int *buffer = calloc(buffer_size, sizeof(int));
    unsigned int i = 0;
    for(; i < size; i++)
    {
        buffer[i] = in[0];
    }

    for(; i < size + in_size; i++)
    {
        buffer[i] = in[i-size];
    }

    for(; i < buffer_size; i++)
    {
        buffer[i] = in[in_size-1];
    }

    boxing_dsp_filter_int(buffer, buffer_size, buffer, filter_coefficients, size, DFALSE);

    for(i = 0; i < out_size; i++)
    {
        out[i] = (boxing_float)buffer[i + (int)(size*1.5f)];
    }
    free(buffer);
}


//----------------------------------------------------------------------------
/*!
 *  \brief Calculation of the Blackman window..
 *
 *  Calculation of the Blackman window..
 *
 *  \param[in]  window  Input data pointer.
 *  \param[in]  size    Size.
 */

void boxing_dsp_blackman_window(boxing_float * window, int size)
{
    const boxing_float alpha = 0.16f;
    const boxing_float a[3] = { (1.0f - alpha) / 2.0f, 1.0f / 2.0f, alpha / 2.0f };

    for(int n = 0; n < size; n++)
    {
        window[n] = a[0] - a[1] * cosf(2.0f*(boxing_float)BOXING_MATH_PI*n / ((boxing_float)size - 1)) + a[2] * cosf(4.0f*(boxing_float)BOXING_MATH_PI*n / ((boxing_float)size - 1));
    }
}


//----------------------------------------------------------------------------
/*!
  * \} end of math group
  */


// PRIVATE DSP FUNCTIONS
//

// the code is taken from boost and deduced to only have float.
static boxing_float sine_cardinal(boxing_float x)
{
    static boxing_float const    taylor_0_bound = BOXING_MATH_EPSILON;
    static boxing_float taylor_2_bound = 1.49011612e-8f;//sqrt(taylor_0_bound);
    static boxing_float taylor_n_bound = 0.00012207031f;//sqrt(taylor_2_bound);

    if (fabs(x) >= taylor_n_bound)
    {
        return(sinf(x) / x);
    }
    else
    {
        // approximation by taylor series in x at 0 up to order 0
        boxing_float result = 1;

        if (fabs(x) >= taylor_0_bound)
        {
            boxing_float x2 = x*x;

            // approximation by taylor series in x at 0 up to order 2
            result -= x2 / 6.0f;

            if (fabs(x) >= taylor_2_bound)
            {
                // approximation by taylor series in x at 0 up to order 4
                result += (x2*x2) / 120.0f;
            }
        }

        return(result);
    }
}
