/*****************************************************************************
**
**  Implementation of the tracker utility interface
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
#include    "boxing/unboxer/frametrackerutil.h"
#include    "boxing/unboxer/sampleutil.h"
#include    "boxing/log.h"
#include    "boxing/unboxer/histogramutils.h"
#include    "boxing/math/dsp.h"
#include    "boxing/math/math.h"
#include    "gvector.h"

//  SYSTEM INCLUDES
//
#include    <stdlib.h> 
#include    <float.h>

//---------------------------------------------------------------------------- 
/*! 
 *  \file     frametrackerutil.h
 *  \defgroup frametrackerutil Frame tracking utility functions
 *  \ingroup  unbox
 *  \brief    Utility functions for tracking frame content
 *
 *  Functions for locating frame border, corner marks and reference bars within
 *  a sampled image.
 */ 

//  CONSTANTS
//

const boxing_float FRAME_GEOMETRY_BORDER_THRESHOLD = 0.30f;

//  PRIVATE INTERFACE
//

static void         median_filter(boxing_float * location, int height, int size);
static void         find_max_location_rate(const boxing_float * data, int width, boxing_float reference, boxing_float sampling_rate, boxing_float * location, int height);
static void         find_max_location_loc(const boxing_float * data, int width, boxing_float reference, boxing_float * location, int height);
static void         calculate_maxmin(const boxing_image8 * image, const boxing_pointi * corner_mark, boxing_float x_sampling_rate, boxing_float y_sampling_rate, boxing_float * max, boxing_float * min);
static DBOOL        track_reference_bar_location(gvector * samples, boxing_double * locations, unsigned int locations_size, boxing_float sampling_rate, boxing_float reference_point);
static boxing_float calculate_average(const boxing_float * matrix, int width, int x_location, int y_location, const boxing_pointi * dimension);
static boxing_float vertical_displacement(int x, int y, int i, int j, float k, float l, const boxing_matrixf * displacement_matix);


/*! 
  * \addtogroup frametrackerutil
  * \{
  */


//----------------------------------------------------------------------------
/*!
 *  \enum       BOXING_TRACKERUTIL_SCAN_DIRECTION frametrackerutil.h
 *  \brief      The enumeration for specifying the scan direction.
 *  
 *  \param BOXING_TRACKERUTIL_SCAN_DIRECTION_LEFT_TO_RIGHT  (0) Left to right scan direction.
 *  \param BOXING_TRACKERUTIL_SCAN_DIRECTION_RIGHT_TO_LEFT  (1) Right to left scan direction.
 *
 *  The enumeration for specifying the scan directin.
 */


// PUBLIC FRAME TRACKER UTIL FUNCTIONS
//

//----------------------------------------------------------------------------
/*!
 *  \brief Find reference bar edge.
 *
 *  Find reference bar edge function description.
 *
 *  \param[in]  image            Input image.
 *  \param[in]  start            Start point.
 *  \param[in]  stop             Stop point.
 *  \param[in]  x_sampling_rate  Sampling rate along the X axis.
 *  \param[in]  y_sampling_rate  Sampling rate along the Y axis.
 *  \return reference bar edge value.
 */

boxing_pointf boxing_frame_tracker_util_find_h_reference_bar_edge(const boxing_image8 * image, const boxing_pointi start, const boxing_pointi stop, boxing_float x_sampling_rate, boxing_float y_sampling_rate)
{
    int length;
    int inc;

    if( stop.y > start.y )
    {   
        length = stop.y - start.y;
        inc = 1;
    }
    else
    {
        length = start.y - stop.y;
        inc = -1;
    }

    boxing_float *samples = calloc(length, sizeof(boxing_float));
    int radius = (int)(25 * x_sampling_rate);
    // sample data
    int y = start.y;

    int x_start = start.x - radius;
    int x_end = start.x + radius;

    for(int i = 0; i < length; i++, y += inc)
    {
        int avg = 0;
        for(int x = x_start; x <= x_end; x++)
        {
            avg += IMAGE8_PIXEL(image, x, y);
        }
        samples[i] = (boxing_float)avg;
        samples[i] /= (boxing_float)radius*2+1;
    }

    // filter coeffs
    boxing_float filter_coefficients[3];
    filter_coefficients[0] = 1.0f;
    filter_coefficients[1] = 0.0f;
    filter_coefficients[2] = -1.0f;

    // calulate diff;
    boxing_dsp_filtfilt(samples, length, samples, length, filter_coefficients, 3);
    boxing_float location = 0.0f;
    // find location
    find_max_location_rate(samples, length, 0, y_sampling_rate, &location, 1);
    // recalc location
    free(samples);
    boxing_pointf return_value = {(boxing_float)start.x, (boxing_float)(location*inc)};
    return return_value;
}


//----------------------------------------------------------------------------
/*!
 *  \brief Track vertical border.
 *
 *  Track vertical border function description.
 *
 *  \param[in]  image            Input image.
 *  \param[in]  from             Start point.
 *  \param[in]  to               Stop point.
 *  \param[out] border_location  Border location.
 *  \param[in]  scan_direction   Scan direction.
 *  \param[in]  x_sampling_rate  Sampling rate along the X axis.
 */

void boxing_frame_tracker_util_track_vertical_border(const boxing_image8 * image, const boxing_pointi * from, const boxing_pointi * to, boxing_float * border_location, int scan_direction, boxing_float x_sampling_rate)
{
    // upper left corner of sampling area
    boxing_pointi location = {(from->x < to->x) ? from->x : to->x, from->y};
    location.x = (int)(location.x - 4*x_sampling_rate);
    const unsigned int location_x = location.x < 1 ? 1 : location.x;
    const unsigned int location_y = location.y;
    // sampling area dimension
    boxing_pointi dimension = {(from->x < to->x) ? to->x-from->x : from->x - to->x, to->y-from->y};
    dimension.x = (int)(dimension.x + 8*x_sampling_rate);

    boxing_float reference = (boxing_float)(from->x - location_x);

    unsigned int blackman_window_size = 15;
    const boxing_float blackman_window = 1.0f / 15.0f;

#define FIXED_POINT_BLACKMAN
#ifdef FIXED_POINT_BLACKMAN
    const unsigned int blackman_window_fixed = (unsigned int)(blackman_window * 10000000.0f);
    unsigned int image_width = image->width;
#endif

    unsigned int data_size_y = dimension.y;
    unsigned int data_size_x = dimension.x;
    boxing_float *data = calloc(data_size_y * data_size_x, sizeof(boxing_float));
    boxing_float *locations = calloc(data_size_y, sizeof(boxing_float));

    const boxing_float right_scan_filter[2] = { -1.0f, 1.0f };
    const boxing_float left_scan_filter[2] = { 1.0f, -1.0f };
    const boxing_float * scan_filter = (scan_direction == BOXING_TRACKERUTIL_SCAN_DIRECTION_LEFT_TO_RIGHT ? left_scan_filter : right_scan_filter);
    const boxing_float scan_filter_a = scan_filter[0];
    const boxing_float scan_filter_b = scan_filter[1];

    // sample and filter
    for(unsigned int row = 0, y = location_y; row < data_size_y; row++, y++)
    {
        boxing_float * it = data + row * data_size_x;

        // SLOW: Fix this
        for(unsigned int col = 0, x = location_x; col < data_size_x; col++, x++)
        {
#ifndef FIXED_POINT_BLACKMAN
            boxing_float a = 0.0f;
            boxing_float b = 0.0f;

            unsigned int blackman_y = y-blackman_window_size/2;
            for(unsigned int j = 0; j < blackman_window_size; j++)
            {
                a += (boxing_float)image.pixel(x-1, blackman_y + j)*blackman_window;
                b += (boxing_float)image.pixel(x+1, blackman_y + j)*blackman_window;
            }
            *it = scan_filter_a * a + scan_filter_b * b;
#else
            unsigned int a_fixed = 0;
            unsigned int b_fixed = 0;

            unsigned int blackman_y = y-blackman_window_size/2;
            const boxing_image8_pixel * data = IMAGE8_PPIXEL(image, x - 1, blackman_y);//image.data( x-1, blackman_y + 0 );
            for(unsigned int j = 0; j < blackman_window_size; j++)
            {
                a_fixed += (*data) * blackman_window_fixed;
                data += 2;
                b_fixed += (*data) * blackman_window_fixed;
                data += image_width - 2;
            }
            *it = scan_filter_a * (a_fixed/10000000.0f) + scan_filter_b * (b_fixed/10000000.0f);
            assert( isfinite( *it ) );
#endif
            ++it;
        }
    }

    find_max_location_rate(data, data_size_x, reference, x_sampling_rate, locations, data_size_y);

    // Normalize window
    boxing_float norm_filter[15];
    for (unsigned int i = 0; i < 15; i++)
        norm_filter[i] = (1.0f/15.0f);
    boxing_dsp_filtfilt(locations, data_size_y, locations, data_size_y, norm_filter, 15);
    
    for(unsigned int i = 0; i < data_size_y; i++)
    {
        border_location[i + location_y] = (boxing_float)(locations[i] + location_x);
        assert( isfinite( border_location[i + location_y] ) );
    }
    free(locations);
    free(data);
}


//----------------------------------------------------------------------------
/*!
 *  \brief Calculate average max/min value.
 *
 *  Calculate average max/min value function description.
 *
 *  \param[in]  image              Input image.
 *  \param[in]  corner_marks       Corner marks array.
 *  \param[in]  corner_marks_size  Corner marks array size.
 *  \param[out] x_sampling_rate    Sampling rate along the X axis.
 *  \param[in]  y_sampling_rate    Sampling rate along the Y axis.
 *  \param[in]  avg_max            Average max.
 *  \param[in]  avg_min            Average min.
 */

void boxing_frame_tracker_util_calculate_average_maxmin(
    const boxing_image8 * image, 
    const boxing_pointi *const * corner_marks, 
    int corner_marks_size, 
    boxing_float x_sampling_rate, 
    boxing_float y_sampling_rate, 
    boxing_float * avg_max, 
    boxing_float * avg_min)
{
    *avg_max = BOXING_PIXEL_MIN;
    *avg_min = BOXING_PIXEL_MIN;

    for(int i = 0; i < corner_marks_size; i++)
    {
        boxing_float max;
        boxing_float min;
        calculate_maxmin(image, corner_marks[i], x_sampling_rate, y_sampling_rate, &max, &min);
        *avg_max += max;
        *avg_min += min;
    }
    *avg_max /= corner_marks_size;
    *avg_min /= corner_marks_size;
}


//---------------------------------------------------------------------------- 
/*! 
 *  \brief  Track reference bar.
 *
 *  This function tracks the reference bar and returns the locations for each peak/valley in
 *  the reference bar signal. The reference bar is an area with alternating black/white lines, where
 *  the lines are perpendicular to the sample direction.
 *
 *  sample_start and sample_end gives the sampling direction and defines the sampling area.
 *  bar_points is a vector that defines the number of expected samples and will be populated with
 *  the coordinates found.
 *
 *  \param[in] image                  Image to be tracked
 *  \param[in] sample_start           Start coordinate for tracking
 *  \param[in] sample_end             End coordinate for tracking
 *  \param[in] bar_points             Positions all peaks and valleys in the reference bar signal.
 *  \param[in] bar_points_size        Number of positions in reference bar points.
 *  \param[in] reference_point        Expected position of first sample.
 *  \param[in] perpendicular_samples  Width of sample in pixels, perpendicular to sample line.
 *  \return a sign of the success of the function.
 *
 */ 

DBOOL boxing_frame_tracker_util_track_reference_bar(
    const boxing_image8 * image, 
    boxing_pointf * sample_start, boxing_pointf * sample_end, 
    boxing_pointf * bar_points, int bar_points_size, 
    boxing_pointf * reference_point,
    int perpendicular_samples)
{
    DLOG_INFO2( "sample_start = (%f, %f)", sample_start->x, sample_start->y);
    DLOG_INFO2( "sample_end = (%f, %f)" , sample_end->x, sample_end->y);

    boxing_pointf start = (*sample_start);
    boxing_pointf direction = {sample_end->x - sample_start->x, sample_end->y - sample_start->y};
    boxing_pointf reference_point_vector = {reference_point->x - sample_start->x, reference_point->y - sample_start->y};
    boxing_pointf dir_norm = boxing_math_pointf_normalize(&direction);
    boxing_float reference = BOXING_MATH_DOT_PRODUCT(dir_norm, reference_point_vector);

    boxing_float sample_rate = sqrtf(direction.x * direction.x + direction.y * direction.y) / (bar_points_size - 1);
    direction = dir_norm;
    gvector * samples = gvector_create(sizeof(int), (size_t)(bar_points_size * sample_rate));

    boxing_double *locations = calloc(bar_points_size, sizeof(boxing_double));
    boxing_pointf pos = start;
    int perpendicular_sample_start = -perpendicular_samples / 2;
    int perpendicular_sample_end = perpendicular_samples + perpendicular_sample_start;

    boxing_sampleutil_perpendicular_lines( samples, &pos, &direction, image, perpendicular_sample_start, perpendicular_sample_end );

    if (!track_reference_bar_location(samples, locations, bar_points_size, sample_rate, reference))
    {
        free(locations);
        gvector_free(samples);
        return DFALSE;
    }

    {
        // perpendicular vector
        boxing_pointf normal = {direction.y, -direction.x};
        boxing_pointf location = {(boxing_float)(direction.x*locations[2]) + start.x, (boxing_float)(direction.y*locations[2]) + start.y};
        
        // reference points
        boxing_float ref_h =  boxing_sampleutil_line(&location, &normal, image, perpendicular_sample_start, perpendicular_sample_end);
        location.x = (boxing_float)(direction.x*locations[1] + start.x);
        location.y = (boxing_float)(direction.y*locations[1] + start.y);
        boxing_float ref_l = boxing_sampleutil_line(&location, &normal, image, perpendicular_sample_start, perpendicular_sample_end);
        boxing_float threshold = (ref_h + ref_l)/2;

        // sample 1
        boxing_pointf location_high = { (boxing_float)(direction.x*locations[0] + start.x), (boxing_float)(direction.y*locations[0] + start.y) };
        boxing_pointf location_low = { (boxing_float)(location_high.x - direction.x*sample_rate), (boxing_float)(location_high.y - direction.y*sample_rate) };

        boxing_float low_sample;
        boxing_float high_sample;
        low_sample =  boxing_sampleutil_line(&location_low, &normal, image, perpendicular_sample_start, perpendicular_sample_end);
        high_sample = boxing_sampleutil_line(&location_high, &normal, image, perpendicular_sample_start, perpendicular_sample_end);
        DBOOL sample1 = (high_sample > threshold) && (low_sample < threshold);

        // sample 2 
        location_high.x -= (boxing_float)(direction.x*sample_rate*2);
        location_high.y -= (boxing_float)(direction.y*sample_rate*2);

        location_low.x  -= (boxing_float)(direction.x*sample_rate*2);
        location_low.y  -= (boxing_float)(direction.y*sample_rate*2);

        low_sample =  boxing_sampleutil_line(&location_low, &normal, image, perpendicular_sample_start, perpendicular_sample_end);
        high_sample = boxing_sampleutil_line(&location_high, &normal, image, perpendicular_sample_start, perpendicular_sample_end);
        DBOOL sample2 = (high_sample > threshold) && (low_sample < threshold);

        // sample 3 
        location_high.x -= (boxing_float)(direction.x*sample_rate*2);
        location_high.y -= (boxing_float)(direction.y*sample_rate*2);
        location_low.x  -= (boxing_float)(direction.x*sample_rate*2);
        location_low.y  -= (boxing_float)(direction.y*sample_rate*2);

        low_sample =  boxing_sampleutil_line(&location_low, &normal, image, perpendicular_sample_start, perpendicular_sample_end);
        high_sample = boxing_sampleutil_line(&location_high, &normal, image, perpendicular_sample_start, perpendicular_sample_end);
        DBOOL sample3 = (high_sample > threshold) && (low_sample < threshold);

        if(!sample1 && !sample2 && !sample3)
        {
            DLOG_INFO("Negative shift detected. Compensating... " );
            if(!track_reference_bar_location(samples, locations, bar_points_size, sample_rate, reference+sample_rate))
            {
                free(locations);
                gvector_free(samples);
                return DFALSE;
            }
        }
        else if(sample1 && !sample2 && !sample3)
        {
            // No error detected
        }
        else if(sample1 && sample2 && !sample3)
        {
            DLOG_INFO("(boxing_frame_tracker_util_track_reference_bar) Positive shift detected. Compensating... ");
            if(!track_reference_bar_location(samples, locations, bar_points_size, sample_rate, reference-sample_rate))
            {
                free(locations);
                gvector_free(samples);
                return DFALSE;
            }
        }
        else
        {
            DLOG_WARNING("(boxing_frame_tracker_util_track_reference_bar) Shift detected but cannot identify direction");
        }

    }

    for(int i = 0; i < bar_points_size; i++)
    {        
        bar_points[i].x = (boxing_float)(direction.x*locations[i]) + start.x;
        bar_points[i].y = (boxing_float)(direction.y*locations[i]) + start.y;
    }
    free(locations);
    gvector_free(samples);

    return DTRUE;
}


//---------------------------------------------------------------------------- 
/*! 
 *  \brief  Locate left or right vertical border of image.
 *
 *  Each frame has a 1 pixel border. This function can be used to detect the
 *  vertical border. It searches along a column stainting in from and the column
 *  lenght is given by length, both in sampled coordinate space.
 *
 *  \param[in]  image   Source image containing the full frame.
 *  \param[in]  from    Start point of search.
 *  \param[in]  lenght  Lenght in pixels (x-direction). Negative value will 
 *                      search for left border.
 *  \param[out] border  Cooridinate of vertical border (if found).
 *  \return DTRUE if border is found.
 *
 */

DBOOL boxing_frame_tracker_util_find_vertical_border(const boxing_image8 * image, const boxing_pointi * from, int lenght, boxing_pointi * border)
{

    // config parameters
    int rectangle_size = 200;
    int threshold;
    int pixel_size = 1;

    int start_x = from->x;
    int stop_x  = from->x + lenght;
    int y      = from->y;
    int x      = from->x;

    // calculate max and min
    int histogram_min = BOXING_PIXEL_MIN;
    int histogram_max = BOXING_PIXEL_MAX;
    boxing_histogram hist = boxing_histogram_create();
    
    if(start_x < stop_x)
    {
        boxing_pointi dim_point = {lenght, rectangle_size};
        boxing_pointi point = {start_x, y - rectangle_size/2};
        boxing_histogram_calc_hist(hist, image, &point, &dim_point);
    }
    else
    {
        boxing_pointi dim_point = {-lenght, rectangle_size};
        boxing_pointi point = {stop_x, y - rectangle_size/2};
        boxing_histogram_calc_hist(hist, image, &point, &dim_point);
    }

    boxing_histogram_calc_limits(hist, &histogram_min, &histogram_max, 0.05f);
    boxing_histogram_free(hist);
    hist = NULL;

    threshold = (int)(histogram_min + (histogram_max - histogram_min) * FRAME_GEOMETRY_BORDER_THRESHOLD);

    while(1)
    {
        if(start_x < stop_x)
        {
            if(!(x < stop_x))
                break; 
            x++;
        }
        else
        {
            if(!(x > stop_x))
                break; 
            x--;
        }

        int pixel_value = IMAGE8_PIXEL(image, x, y);

        if(pixel_value > threshold )
        {
            boxing_pointi point_from = {(start_x < stop_x ? x : x-pixel_size), y - rectangle_size/2};
            boxing_pointi dimension = {pixel_size, rectangle_size};

            boxing_float average = boxing_histogram_calc_average_image(image, &point_from, &dimension);

            if (average > threshold)
            {
                border->x = x-1;
                border->y = y;
                DLOG_INFO2("Found border candidate at: %ix%i", x, y-1);
                return DTRUE;
            }
        }
    }

    DLOG_ERROR("(boxing_frame_tracker_util_find_vertical_border) No border found");
    return DFALSE;
}

//---------------------------------------------------------------------------- 
/*! 
 *  \brief Locate top or bottom horizontal border of image. 
 * 
 *  Each frame has a 1 pixel border. This function can be used to detect the 
 *  horizontal border. It searches along a line stainting in from and the line
 *  lenght is given by length, both in sampled coordinate space.
 * 
 *  \param[in]  image   Source image containing the full frame. 
 *  \param[in]  from    Start point of search. 
 *  \param[in]  length  Lenght in pixels (y-direction). Negative value will 
 *                      search for bottom border. 
 *  \param[in]  border  Cooridinate of horizontal border (if found).
 *  \return DTRUE if border is found. 
 */ 

DBOOL boxing_frame_tracker_util_find_horizontal_border(const boxing_image8 * image, const boxing_pointi * from, int length, boxing_pointi * border)
{
    // config parameters 
    int rectangle_size = 200;
    int threshold;
    int pixel_size = 1;
    
    int start_y = from->y;
    int stop_y = from->y + length;
    int x      = from->x;
    int y      = from->y;
    
    // calculate max and min
    int histogram_min = BOXING_PIXEL_MIN;
    int histogram_max = BOXING_PIXEL_MAX;
    boxing_histogram hist = boxing_histogram_create();
    if(start_y < stop_y)
    {
        boxing_pointi point_dim = { rectangle_size, length };
        boxing_pointi point = {x - rectangle_size/2, start_y};
        boxing_histogram_calc_hist(hist, image, &point, &point_dim);
    }
    else
    {
        boxing_pointi point_dim = { rectangle_size, -length };
        boxing_pointi point = {x - rectangle_size/2, stop_y};
        boxing_histogram_calc_hist(hist, image, &point, &point_dim);
    }

    boxing_histogram_calc_limits(hist, &histogram_min, &histogram_max, 0.05f);
    boxing_histogram_free(hist);
    hist = NULL;

    threshold = (int)(histogram_min + (histogram_max - histogram_min) * FRAME_GEOMETRY_BORDER_THRESHOLD);

    int inc;
    int stop = stop_y;
    if(start_y < stop_y)
    {
        inc = 1;
    }
    else
    {
        inc = -1;
    }

    while ( y != stop )
    {
        y += inc;

        int pixel_value = IMAGE8_PIXEL(image, x, y);

        if(pixel_value > threshold )
        {
            boxing_pointi point_from = {x - rectangle_size/2, (start_y < stop_y ? y : y-pixel_size)};
            boxing_pointi dimension = {rectangle_size, pixel_size};

            boxing_float average = boxing_histogram_calc_average_image(image, &point_from, &dimension);

            if (average > threshold)
            {
                border->x = x;
                border->y = y-1;

                DLOG_INFO2("Found horizontal border candidate at: %ix%i", x, y-1);
                return DTRUE;
            }
        }
    }

    DLOG_ERROR("(boxing_frame_tracker_util_find_horizontal_border) No border found");
    return DFALSE;
}

//---------------------------------------------------------------------------- 
/*! 
 *  \brief Locate frame corner marks in image. 
 * 
 *  Each frame has a checkerboard corner mark in each of it�s four corners. 
 *  The checkerboard pattern is designed to be easily detected by computer pattern 
 *  recognition algorithms. The four corner marks define the global geometry 
 *  of the frame. 
 * 
 *  Each corner mark symbol is defined as a 2x2 black-white squared mosaic, being all  
 *  squares of the same size. The tiles are printed as black and white pixels, where  
 *  the top left is black. Surrounding the corner mark symbol is a border (quiet zone)  
 *  printed as black pixels. 
 *   
 *  The function searches for the best matching corner mark in the image in the given 
 *  search area specified by location and dimension parameters. If the search area is
 *  outside the image, it's snapped to within image borders.
 *
 *  \param[in]  image           Source image containing the full frame. 
 *  \param[in]  location        Top left corner of search area (image coordinates). 
 *  \param[in]  dimension       Size of search area (image coordinates). 
 *  \param[in]  x_sampling_rate Image pixels per logical pixel - x direction.
 *  \param[in]  y_sampling_rate Image pixels per logical pixel - y direction.
 *  \return location of best matching corner mark in given search area. 
 */ 

boxing_pointi boxing_frame_tracker_util_find_corner_mark(
    const boxing_image8 * image, 
    boxing_pointi location, 
    boxing_pointi dimension, 
    boxing_float x_sampling_rate, 
    boxing_float y_sampling_rate)
{

    // validate location
    location.x = BOXING_MATH_CLAMP(0, (int)image->width-1, location.x);
    location.y = BOXING_MATH_CLAMP(0, (int)image->height-1, location.y);

    if((int)image->width-1 < location.x + dimension.x)
    {
        dimension.x = image->width-1;
    }

    // validate dimension
    dimension.x = BOXING_MATH_CLAMP(0, (int)image->width-1, location.x + dimension.x) - location.x;
    dimension.y = BOXING_MATH_CLAMP(0, (int)image->height-1, location.y + dimension.y) - location.y;

    if(dimension.x == 0 || dimension.y == 0)
    {
        DLOG_WARNING( "(boxing_frame_tracker_util_find_corner_mark) Reference mark out of bounds" );
        return location;
    }

    int dimension_x = dimension.x;
    int dimension_y = dimension.y;
    int location_x = location.x;
    int location_y = location.y;

    boxing_float *matrix_black = calloc(dimension_y * dimension_x, sizeof(boxing_float));
    boxing_float *matrix_white = calloc(dimension_y * dimension_x, sizeof(boxing_float));

    // calculate max and min
    int histogram_min = BOXING_PIXEL_MIN;
    int histogram_max = BOXING_PIXEL_MAX;
    const boxing_float saturation = 0.05f;
    boxing_histogram hist = boxing_histogram_create();
    boxing_histogram_calc_hist(hist, image, &location, &dimension);
    boxing_histogram_calc_limits(hist, &histogram_min, &histogram_max, saturation );
    boxing_histogram_free(hist);
    hist = NULL;

    // precalulate values
    for(int y = 0; y < dimension_y; y++)
    {
        for(int x = 0; x < dimension_x; x++)
        {
            int pixel = IMAGE8_PIXEL(image, location_x + x, location_y + y);
            boxing_float min_delta = (boxing_float)(pixel - histogram_min);
            boxing_float max_delta = (boxing_float)(pixel - histogram_max);

            matrix_black[y * dimension_x + x] = min_delta * min_delta;
            matrix_white[y * dimension_x + x] = max_delta * max_delta;
        }
    }

    // create accumulated map
    for(int x = 1; x < dimension_x; x++)
    {
        for(int y = 0; y < dimension_y; y++)
        {
            matrix_black[y * dimension_x + x] += matrix_black[y * dimension_x + x-1]; 
            matrix_white[y * dimension_x + x] += matrix_white[y * dimension_x + x-1];
        }
    }

    for(int y = 1; y < dimension_y; y++)
    {
        for(int x = 0; x < dimension_x; x++)
        {
            matrix_black[y * dimension_x + x] += matrix_black[(y-1) * dimension_x + x]; 
            matrix_white[y * dimension_x + x] += matrix_white[(y-1) * dimension_x + x];
        }
    }

    boxing_float  min = BOXING_FLOAT_MAX;
    boxing_pointi min_location = location;

    const int corner_mark_width = 32;
    const int corner_mark_height = 32;
    boxing_pointi subpattern_dimension = {(int)((corner_mark_width/2)*0.9f*x_sampling_rate), (int)((corner_mark_height/2)*0.9f*y_sampling_rate)};

    if(subpattern_dimension.x % 2)
        subpattern_dimension.x = subpattern_dimension.x + 1;
    if(subpattern_dimension.y % 2)
        subpattern_dimension.y = subpattern_dimension.y + 1;

    for(int y = 0; y < dimension_y - subpattern_dimension.y*2; y++)
    {
        for(int x = 0; x < dimension_x - subpattern_dimension.x*2; x++)
        {
            boxing_float err = 0.0;
            err += calculate_average(matrix_black, dimension_x, x, y, &subpattern_dimension);
            err += calculate_average(matrix_black, dimension_x, x + subpattern_dimension.x, y + subpattern_dimension.y, &subpattern_dimension);
            err += calculate_average(matrix_white, dimension_x, x + subpattern_dimension.x, y, &subpattern_dimension);
            err += calculate_average(matrix_white, dimension_x, x, y + subpattern_dimension.y, &subpattern_dimension);

            if(min > err)
            {
                min = err;
                min_location.x = location_x + x + subpattern_dimension.x;
                min_location.y = location_y + y + subpattern_dimension.y;
            }

        }
    }
    free(matrix_black);
    free(matrix_white);
    
    DLOG_INFO3( "Corner mark match factor (x=%i y=%i) = %f", location_x, location_y, sqrt(min)/(subpattern_dimension.x*2*subpattern_dimension.y*2) );

    return min_location;
}


//---------------------------------------------------------------------------- 
/*! 
 *  \brief Validate corner mark. 
 * 
 *  The function checks the validity of corner mark.
 * 
 *  \param[in]  location   Corner mark location. 
 *  \param[in]  dimension  Corner mark dimension. 
 *  \param[in]  image      Input image. 
 *  \return DTRUE if corner mark is valid. 
 */

DBOOL boxing_frame_tracker_util_validate_corner_mark(boxing_pointi * location, boxing_pointi * dimension, const boxing_image8 * image)
{
    int width = image->width;
    int height = image->height;

    return ((BOXING_MATH_IS_WITHIN_LIMIT(0, width-1, location->x - dimension->x/2)) &&
            (BOXING_MATH_IS_WITHIN_LIMIT(0, width-1, location->x + dimension->x/2)) &&
            (BOXING_MATH_IS_WITHIN_LIMIT(0, height-1, location->y - dimension->y/2)) &&
            (BOXING_MATH_IS_WITHIN_LIMIT(0, height-1, location->y + dimension->y/2)));
}


//---------------------------------------------------------------------------- 
/*! 
 *  \brief Add dispalcement. 
 * 
 *  Add dispalcement function description.
 * 
 *  \param[in]  displacement_matrix  Displacement matrix. 
 *  \param[out] point_model          Point model. 
 */

void boxing_frame_tracker_util_add_displacement(const boxing_matrixf * displacement_matrix, boxing_matrixf * point_model)
{
    boxing_pointf * points = point_model->data;
    unsigned int height = point_model->height;
    unsigned int width = point_model->width;
    int i = width/(displacement_matrix->width-1);
    int j = height/(displacement_matrix->height-1);

    for (unsigned int y = 0; y < height; y++, points += point_model->width)
    {
        boxing_pointf * point = points;

        boxing_float l = (boxing_float)(y%j);
        l /= j;

        for (unsigned int x = 0; x < width; x++, ++point)
        {
            boxing_float k = (boxing_float)(x%i);
            k /= i;

            point->y += vertical_displacement(x, y, i, j, k, l, displacement_matrix);
        }
    }

}


//---------------------------------------------------------------------------- 
/*! 
 *  \brief Locate frame in image. 
 *  
 *  Locates frame by first locating frame borders, then locating corner marks.
 *
 *  \param[in]  image            Source image.
 *  \param[in]  corner_marks     Corner marks.
 *  \param[in]  x_sampling_rate  Sampling rate along the X axis.
 *  \param[in]  y_sampling_rate  Sampling rate along the Y axis.
 *  \return DTRUE if frame is found.
 */

DBOOL boxing_frame_tracker_util_find_frame(const boxing_image8 * image, frame_corner_marks * corner_marks, boxing_float x_sampling_rate, boxing_float y_sampling_rate)
{
    int width = image->width;
    int height = image->height;

    boxing_pointi top_samples[3];
    boxing_pointi bottom_samples[3];
    boxing_pointi left_samples[3];
    boxing_pointi right_samples[3];

    boxing_pointi top = {0,0};
    boxing_pointi bottom = {0,height};
    boxing_pointi left = {0,0};
    boxing_pointi right = {width,0};

    int step_x = width / 4;
    int step_y = height / 4;

    for ( int i = 1; i < 4; i++ )
    {
        boxing_pointi point = {i*step_x, 0};
        if (!boxing_frame_tracker_util_find_horizontal_border(image, &point, height / 4, &top_samples[i - 1]))
        {
            DLOG_ERROR("(boxing_frame_tracker_util_find_frame) Unable to find top horizontal border");
            return DFALSE;
        }
        point.x = i*step_x;
        point.y = height-1;
        if (!boxing_frame_tracker_util_find_horizontal_border(image, &point, -height / 4, &bottom_samples[i - 1]))
        {
            DLOG_ERROR("(boxing_frame_tracker_util_find_frame) Unable to find bottom horizontal border");
            return DFALSE;
        }
        point.x = 0;
        point.y = i*step_y;
        if (!boxing_frame_tracker_util_find_vertical_border(image, &point, width / 4, &left_samples[i - 1]))
        {
            DLOG_ERROR("(boxing_frame_tracker_util_find_frame) Unable to find left vertical border");
            return DFALSE;
        }
        point.x = width-1;
        point.y = i*step_y;
        if (!boxing_frame_tracker_util_find_vertical_border(image, &point, -width / 4, &right_samples[i - 1]))
        {
            DLOG_ERROR("(boxing_frame_tracker_util_find_frame) Unable to find left vertical border");
            return DFALSE;
        }

        if ( top.y < top_samples[i-1].y )
        {
            top = top_samples[i-1];
        }
        if ( bottom.y > bottom_samples[i-1].y )
        {
            bottom = bottom_samples[i-1];
        }
        if ( left.x < left_samples[i-1].x )
        {
            left = left_samples[i-1];
        }
        if ( right.x > right_samples[i-1].x )
        {
            right = right_samples[i-1];
        }
    }

    DLOG_INFO2("top    = (%i, %i)", top.x   , top.y   );
    DLOG_INFO2("bottom = (%i, %i)", bottom.x, bottom.y);
    DLOG_INFO2("left   = (%i, %i)", left.x  , left.y  );
    DLOG_INFO2("right  = (%i, %i)", right.x , right.y );

    boxing_pointi dimension = {(int)( 32*x_sampling_rate ), (int)( 32*y_sampling_rate )};
    boxing_pointi * top_left =     &corner_marks->top_left;
    boxing_pointi * top_right =    &corner_marks->top_right;
    boxing_pointi * bottom_left =  &corner_marks->bottom_left;
    boxing_pointi * bottom_right = &corner_marks->bottom_right;

    top_left->x = left.x  - dimension.x;
    top_left->y = top.y  - dimension.y;

    top_right->x = right.x - 2*dimension.x;
    top_right->y = top.y  - dimension.y;

    bottom_left->x = left.x  - dimension.x;
    bottom_left->y = bottom.y - 2*dimension.y;

    bottom_right->x = right.x - 2*dimension.x;
    bottom_right->y = bottom.y - 2*dimension.y;
    dimension.x = dimension.x*3;
    dimension.y = dimension.y*3;

    *top_left     = boxing_frame_tracker_util_find_corner_mark(image, *top_left, dimension, x_sampling_rate, y_sampling_rate);
    *top_right    = boxing_frame_tracker_util_find_corner_mark(image, *top_right, dimension, x_sampling_rate, y_sampling_rate);
    *bottom_left  = boxing_frame_tracker_util_find_corner_mark(image, *bottom_left, dimension, x_sampling_rate, y_sampling_rate);
    *bottom_right = boxing_frame_tracker_util_find_corner_mark(image, *bottom_right, dimension, x_sampling_rate, y_sampling_rate);

    return DTRUE;
}


//----------------------------------------------------------------------------
/*!
  * \} end of frametrackerutil group
  */


// PRIVATE FRAME TRACKER UTIL FUNCTIONS
//

static boxing_float find_max_location(const boxing_float * data, unsigned int data_size, boxing_float reference)
{
    boxing_float location = reference;

    const boxing_float * begin = data;
    const boxing_float * it = begin;
    const boxing_float * end = data + data_size;

    boxing_float last = *it;
    for (; it != end; ++it)
    {
        if (last < *it)
        {
            //if( (pow((boxing_float)(last - reference), 2.0f) + 1) >= pow((boxing_float)(n - reference), 2.0f))
            {
                last = *it;
                location = (boxing_float)(it - begin);
            }

        }
    }
    return location;
}

static void find_max_location_loc(const boxing_float * data, int width, boxing_float reference, boxing_float * location, int height)
{
    const boxing_float * it = data;
    const boxing_float * end = data + width * height;
    boxing_float * out = location;

    for (; it != end; it += width, ++out)
    {
        reference = *out = find_max_location(it, width, reference);
    }

}

static int compareDouble(const void * a, const void * b)
{
    if (*(boxing_float*)a > *(boxing_float*)b)
    {
        return 1;
    }
    else if (*(boxing_float*)a < *(boxing_float*)b)
    {
        return -1;
    }

    return 0;
}

static int search(boxing_float value, boxing_float * values, int low, int high)
{
    int center = (low + high) / 2;
    do {
        if (values[center] > value)
        {
            high = center;
        }
        else
        {
            low = center;
        }
        center = (low + high) / 2;
    } while (low != center);

    if (low + 1 <= high)
    {
        return (value < values[low + 1] ? low : low + 1);
    }
    return low;
}

static void shift_right(boxing_float* values, int low, int high)
{
    for (int i = high; i > low - 1; i--)
        values[i + 1] = values[i];
}


static void shift_left(boxing_float * values, int low, int high)
{
    for (int i = low; i < high + 1; i++)
        values[i - 1] = values[i];
}


static void update(boxing_float insert_value, boxing_float extract_value, boxing_float * values, int size)
{
    int insert_index = search(insert_value, values, 0, size - 1);
    int extract_index = search(extract_value, values, 0, size - 1);

    if (insert_index < extract_index)
    {
        if (insert_value > values[insert_index])
        {
            insert_index++;
        }
        shift_right(values, insert_index, extract_index - 1);
    }
    else if (insert_index > extract_index)
    {
        shift_left(values, extract_index + 1, insert_index);
    }

    values[insert_index] = insert_value;
}

// this one will be optimized
static void median_filter(boxing_float * location, int height, int size)
{
    if (size > height)
        size = height;

    boxing_float *values = calloc((unsigned)size, sizeof(boxing_float));
    boxing_float *result = calloc((unsigned)height, sizeof(boxing_float));

    int center = size / 2;


    // pre filter
    memcpy(values, location, sizeof(boxing_float)*size);
    qsort(values, size, sizeof(boxing_float), compareDouble);
    int i = 0;
    for (; i < center + 1; i++)
        result[i] = values[center];

    // main filter
    for (; i < height - center; i++)
    {
        update(location[i + center], location[i - center - 1], values, size);
        result[i] = values[center];
    }

    // post filter
    for (; i < height; i++)
        result[i] = values[center];

    // copyback
    memcpy(location, result, sizeof(boxing_float)*height);

    free(result);
    free(values);
}

static boxing_float vertical_displacement(int x, int y, int i, int j, float k, float l, const boxing_matrixf * displacement_matix)
{

    boxing_float a = MATRIX_ELEMENT(displacement_matix, y / j, x / i).y;
    boxing_float b = MATRIX_ELEMENT(displacement_matix, y / j, x / i + 1).y;
    boxing_float c = MATRIX_ELEMENT(displacement_matix, y / j + 1, x / i).y;
    boxing_float d = MATRIX_ELEMENT(displacement_matix, y / j + 1, x / i + 1).y;

    boxing_float r = (a*(1 - k) + b*k)*(1 - l) + (c*(1 - k) + d*k)*(l);

    assert(isfinite(r));

    return r;

}


static void find_max_location_rate(const boxing_float * data, int width, boxing_float reference, boxing_float sampling_rate, boxing_float * location, int height)
{
    find_max_location_loc(data, width, reference, location, height);

    const boxing_float epsylon = 1E-38f;// 100;
    boxing_float res[3];
    boxing_float vector_y[3];
    boxing_double matrix_invariant_a[3][3]; // the inverse A matrix
    boxing_double matrix_a[3][3] =
    {
        { cos(-BOXING_MATH_PI / sampling_rate), sin(-BOXING_MATH_PI / sampling_rate), 1 },
        { 1, 0, 1 },
        { cos(BOXING_MATH_PI / sampling_rate), sin(BOXING_MATH_PI / sampling_rate), 1 }
    };

    boxing_math_matrix3x3_invariant(matrix_a, matrix_invariant_a);


    // find average
    boxing_float edge_average = 0.0f;
    for (int k = 0; k < height; k++)
    {
        boxing_float pos = location[k];
        if (pos < 1.0f)
        {
            pos = 1.0f;
        }
        else if (pos > width - 2)
        {
            pos = (boxing_float)(width - 2);
        }

        edge_average += data[k * width + (int)pos] / (boxing_float)height;
    }

    boxing_float *edge_vector = calloc(height, sizeof(boxing_float));
    boxing_float *edge_pow_vector = calloc(height, sizeof(boxing_float));
    boxing_float *coeffs127 = calloc(127, sizeof(boxing_float));
    boxing_float *coeffs41 = calloc(127, sizeof(boxing_float));
    for (int i = 0; i < 127; ++i)
    {
        coeffs127[i] = (1.0f / 127.0f);
    }
    for (int i = 0; i < 41; ++i)
    {
        coeffs41[i] = (1.0f / 41.0f);
    }
    {

        for (int k = 0; k < height; k++)
        {
            boxing_float pos = location[k];
            if (pos < 1)
            {
                pos = 1;
            }
            else if (pos > width - 2)
            {
                pos = (boxing_float)(width - 2);
            }

            edge_vector[k] = data[k * width + (int)pos];
        }

        edge_average = edge_vector[0] / height;
        for (int i = 1; i < height; ++i)
        {
            edge_average += edge_vector[i] / height;
        }

        boxing_dsp_filtfilt(edge_vector, height, edge_pow_vector, height, coeffs127, 127);
        for (int k = 0; k < height; k++)
        {
            edge_pow_vector[k] = powf(edge_vector[k] - edge_pow_vector[k], 2);
        }
        boxing_dsp_filtfilt(edge_pow_vector, height, edge_pow_vector, height, coeffs41, 41);
    }
    free(coeffs41);
    coeffs41 = NULL;
    free(coeffs127);
    coeffs127 = NULL;


    DBOOL drop_out = DFALSE;
    /// \todo warning: variable 'overshoot' set but not used
    DBOOL overshoot = DFALSE;
    (void)overshoot;
    int drop_out_position = 0;

    for (int k = 0; k < height; k++)
    {
        boxing_float pos = location[k];
        if (pos < 1)
        {
            pos = 1;
        }
        else if (pos > width - 2)
        {
            pos = (boxing_float)(width - 2);
        }

        for (int N = 0; N < 3; N++)
        {
            vector_y[N] = data[k * width + (int)pos - 1 + N];
        }

        res[0] = (boxing_float)matrix_invariant_a[0][0] * vector_y[0] + (boxing_float)matrix_invariant_a[0][1] * vector_y[1] + (boxing_float)matrix_invariant_a[0][2] * vector_y[2];
        res[1] = (boxing_float)matrix_invariant_a[1][0] * vector_y[0] + (boxing_float)matrix_invariant_a[1][1] * vector_y[1] + (boxing_float)matrix_invariant_a[1][2] * vector_y[2];

        if (fabs(res[0]) < epsylon)
        {
            res[0] = BOXING_MATH_SIGN(res[0])*epsylon;
        }

        boxing_float offset = atanf(res[1] / res[0]);

        if (res[0] * cosf(offset*(boxing_float)BOXING_MATH_PI / sampling_rate) + res[1] * sinf(offset*(boxing_float)BOXING_MATH_PI / sampling_rate) < 0)
        {
            offset += sampling_rate;
        }

        location[k] += offset;
    }

    drop_out = 0;
    drop_out_position = 1;
    overshoot = 0;
    for (int k = 0; k < height; k++)
    {

        if ((edge_pow_vector[k] > edge_average*edge_average*0.02))
        {
            if (!drop_out)
            {
                drop_out_position = k;
                drop_out = DTRUE;
            }
            continue;
        }

        if (drop_out)
        {
            if ((drop_out_position + 1 < k) && (drop_out_position > 0))
            {
                boxing_float n_dk = (boxing_float)(location[k] - location[drop_out_position - 1]) / (boxing_float)(k - (drop_out_position - 1));
                for (int d_pos = drop_out_position; d_pos < k; d_pos++)
                {
                    location[d_pos] = n_dk*(d_pos + 1 - drop_out_position) + location[drop_out_position - 1];
                }
            }
            drop_out = DFALSE;
        }
    }
    free(edge_pow_vector);
    free(edge_vector);

    median_filter(location, height, 101);
}


static void calculate_maxmin(
    const boxing_image8 * image,
    const boxing_pointi * corner_mark,
    boxing_float x_sampling_rate,
    boxing_float y_sampling_rate,
    boxing_float * max,
    boxing_float * min)
{
    boxing_pointi dimension = { (int)(10 * x_sampling_rate), (int)(10 * y_sampling_rate) };
    boxing_pointi offset = { (int)(2 * x_sampling_rate), (int)(2 * y_sampling_rate) };
    boxing_pointi location;

    *max = BOXING_PIXEL_MIN;

    /* upper right */
    location.x = BOXING_MATH_CLAMP(0, (int)image->width - 1, corner_mark->x + offset.x);
    location.y = BOXING_MATH_CLAMP(0, (int)image->height - 1, corner_mark->y - offset.y - dimension.y);
    *max += (boxing_float)boxing_histogram_calc_average_image(image, &location, &dimension);

    /* lower left */
    location.x = BOXING_MATH_CLAMP(0, (int)image->width - 1, corner_mark->x - offset.x - dimension.x);
    location.y = BOXING_MATH_CLAMP(0, (int)image->height - 1, corner_mark->y + offset.y);
    *max += (boxing_float)boxing_histogram_calc_average_image(image, &location, &dimension);

    *max /= 2;

    *min = BOXING_PIXEL_MIN;

    /* upper left */
    location.x = BOXING_MATH_CLAMP(0, (int)image->width - 1, corner_mark->x - offset.x - dimension.x);
    location.y = BOXING_MATH_CLAMP(0, (int)image->height - 1, corner_mark->y - offset.y - dimension.y);
    *min += (boxing_float)boxing_histogram_calc_average_image(image, &location, &dimension);

    /* lower right */
    location.x = BOXING_MATH_CLAMP(0, (int)image->width - 1, corner_mark->x + offset.x);
    location.y = BOXING_MATH_CLAMP(0, (int)image->height - 1, corner_mark->y + offset.y);
    *min += (boxing_float)boxing_histogram_calc_average_image(image, &location, &dimension);

    *min /= 2;
}


/*
*  \brief   track peak and valley locations in the reference bar
*
*
*  The tracking algorithm uses the function
*
*  f(n,m) = sin(n*pi/samplingRate+m*pi/samplingRate) + c
*
*  for finding each peak in the reference bar where n and m are samples.
*  By substituting n*pi/samplingRate = x and m*pi/samplingRate = phase
*  we get
*
*  f(x,phase) = sin(x+phase) + c = sin(phase)*cos(x) + cos(phase)*sin(x) + c
*
*  Since phase is constant we simplify the equation further by
*  substituting sin(phase) = a and cos(phase) = b and we get
*
*  f(x) = a*cos(x) + b*sin(x) + c = y
*
*  This can be written in matrix form X*C=Y
*   _                   _     _   _     _    _
*  |  cos(x0) sin(x0) 1  |   |  a  |   |  y0  |
*  |  cos(x1) sin(x1) 1  | * |  b  | = |  y1  |
*  |_ cos(x2) sin(x2) 1 _|   |_ c _|   |_ y2 _|
*
*  then we have C=Y/X
*   _   _     _                   _        _    _
*  |  a  |   |  cos(x0) sin(x0) 1  |-1    |  y0  |
*  |  b  | = |  cos(x1) sin(x1) 1  |   *  |  y1  |
*  |_ c _|   |_ cos(x2) sin(x2) 1 _|      |_ y2 _|
*
*  Knowing sample positions and their correspondig values, a b and c
*  can be solwed.
*
*  Once a and b is found we can calculate the phase
*
*  phase = atan( a/b )
*
*  and its value in saples
*
*  m = phase*samplingRate/pi
*
*
*  \param[in]  samples                A vector of the sampled reference bar. Values are stored as int
*  \param[out] locations              Peak and valley locations found
*  \param[in]  locations_size         Size of the location array
*  \param[in]  sampling_rate          Sampling rate
*  \param[in]  reference_point        Expected position of first peak location.
*  \return Unboxing result status code
*/
static DBOOL track_reference_bar_location(gvector * samples, boxing_double * locations, unsigned int locations_size, boxing_float sampling_rate, boxing_float reference_point)
{
    DBOOL retval = DTRUE;
    int filter_order = (int)(22 * sampling_rate);
    // make sure we use odd number of filter coeffs
    filter_order += (filter_order % 2 == 0) ? 1 : 0;

    boxing_float *filter_coefficients = calloc(filter_order, sizeof(boxing_float));
    boxing_float center_frequency = 0.5f;
    boxing_float bandwidth = 0.18f;


    /* create a narrow bandpass filter to remove unwanted frequencies
    in the sampled reference bar. The output is a  sinusoid (the reference bar carrier frequency). */
    boxing_dsp_band_pass_filter(center_frequency, bandwidth, sampling_rate, filter_coefficients, filter_order);

    gvector_resize(samples, (unsigned int)(samples->size + filter_order / 2));

    boxing_float *sync = calloc(samples->size, sizeof(boxing_float));

    /* filter the reference bar */
    boxing_dsp_filtfilt_int((int*)samples->buffer, (unsigned int)samples->size, sync, (unsigned int)samples->size, filter_coefficients, filter_order);

    /* create a lowpass filter for mearuring the enery in the reference bar signal. */
    boxing_dsp_low_pass_filter(0.41f, sampling_rate, filter_coefficients, filter_order);

    boxing_float *energy = calloc(samples->size, sizeof(boxing_float));
    memcpy(energy, sync, samples->size * sizeof(boxing_float));
    boxing_float max_enery = energy[0];
    for (unsigned int i = 0; i < samples->size; ++i)
    {
        energy[i] = fabsf(energy[i]);
    }
    boxing_dsp_filtfilt(energy, (unsigned int)samples->size, energy, (unsigned int)samples->size, filter_coefficients, filter_order);
    for (unsigned int i = 1; i < samples->size; ++i)
    {
        if (energy[i] > max_enery)
        {
            max_enery = energy[i];
        }
    }

    free(filter_coefficients);
    filter_coefficients = NULL;

    /* normalize energy measure */
    max_enery = 1 / max_enery; // scale
    for (unsigned int i = 0; i < samples->size; ++i)
    {
        energy[i] *= max_enery;
    }

    /* location matrix with corresponding sample locations */
    boxing_double matrix_a[3][3] =
    {
        { cos(0.0*BOXING_MATH_PI / sampling_rate), sin(0.0*BOXING_MATH_PI / sampling_rate), 1.0 },
        { cos(1.0*BOXING_MATH_PI / sampling_rate), sin(1.0*BOXING_MATH_PI / sampling_rate), 1.0 },
        { cos(2.0*BOXING_MATH_PI / sampling_rate), sin(2.0*BOXING_MATH_PI / sampling_rate), 1.0 },
    };

    boxing_double res[3];
    boxing_double vector_y[3];

    boxing_double matrix_invariant_a[3][3];
    /* calculate the inverse location matrix */
    boxing_math_matrix3x3_invariant(matrix_a, matrix_invariant_a);

    int pos = -2;
    boxing_double last_position = reference_point - sampling_rate * 2;

    for (int index = (int)reference_point; index < (int)(samples->size - (2));)
    {
        if (pos > 0 && (unsigned int)pos < locations_size)
        {
            if ((locations[0] - reference_point)*(locations[0] - reference_point) >(locations[pos] - reference_point)*(locations[pos] - reference_point))
            {
                locations[0] = locations[pos];
                pos = 0;
            }
        }

        vector_y[0] = sync[0 + index];
        vector_y[1] = sync[1 + index];
        vector_y[2] = sync[2 + index];

        /* We only calculate a anb b since we only interested in phase */
        res[0] = matrix_invariant_a[0][0] * vector_y[0] + matrix_invariant_a[0][1] * vector_y[1] + matrix_invariant_a[0][2] * vector_y[2];
        res[1] = matrix_invariant_a[1][0] * vector_y[0] + matrix_invariant_a[1][1] * vector_y[1] + matrix_invariant_a[1][2] * vector_y[2];

        if (res[0] == 0)
        {
            res[0] = 1;
            res[1] = 0;
        }
        /* calculate phase shift in samples */
        boxing_double alpha = atan(res[1] / res[0]) / (BOXING_MATH_PI / sampling_rate);
        boxing_double offset;
        if (res[0] * cos(alpha*BOXING_MATH_PI / sampling_rate) + res[1] * sin(alpha*BOXING_MATH_PI / sampling_rate) > 0)
        {
            offset = alpha;
        }
        else
        {
            offset = sampling_rate + alpha;
        }

        /* Is the new positione a plausible candidate ? */
        if (last_position + sampling_rate*1.5 < (index + 1) + offset)
        {
            pos += 2;
            if (pos < (int)locations_size)
            {
                /* If the signal strength dropps below 30% of max then
                we can't rely the calculated position and we have to
                use the last good position as reference to simulate
                the new
                */
                if ((energy[index] < 0.3f) && (pos > 1))
                {
                    if (pos < 4)
                    {
                        locations[pos] = locations[pos - 2] + sampling_rate * 2;
                    }
                    else
                    {
                        locations[pos] = 2 * locations[pos - 2] - locations[pos - 4];
                    }
                    last_position = locations[pos];

                    index = (int)(last_position + sampling_rate);
                    continue;
                }
                else
                {
                    locations[pos] = last_position = (index + 0) + offset;
                }


                if ((pos == 0) && (locations[pos] < 0))
                {
                    pos = -2;
                    last_position = -sampling_rate;
                    index++;
                    continue;
                }

                if (locations[pos] > (locations_size + 2)*sampling_rate)
                {
                    DLOG_WARNING("(track_reference_bar_location) reference bar out of range");
                    break;
                }
            }
        }
        int inc = (int)floor(offset + sampling_rate);
        if (inc == 0)
            inc++;
        index += inc;

    }
    free(energy);
    energy = NULL;
    free(sync);
    sync = NULL;

    /* generate the remaining locations in case not all are found */
    if (pos + 2 < (int)locations_size)
    {
        DLOG_WARNING2("(track_reference_bar_location) found %d of %d locations", pos / 2, locations_size / 2);
        for (int i = pos; i + 2 < (int)locations_size; i += 2)
        {
            locations[i + 2] = locations[i] + 2 * sampling_rate;
        }
    }

    /* validate */
    for (int i = 0; i < (int)locations_size; i += 2)
    {
        if (locations[i] < 0)
        {
            locations[i] = 0;
        }
        else if (locations[i] > (locations_size)*sampling_rate)
        {
            locations[i] = (locations_size)*sampling_rate;
        }
    }

    /* Valley locations must to calculated since only the peaks are found */
    for (int i = locations_size - 4; i > -1; i -= 2)
    {
        locations[i + 1] = (locations[i] + locations[i + 2]) / 2;
    }

    locations[locations_size - 1] = locations[locations_size - 2] + sampling_rate;


    return retval;
}


static boxing_float calculate_average(const boxing_float * matrix, int width, int x_location, int y_location, const boxing_pointi * dimension)
{
    boxing_float retval = matrix[(y_location + dimension->y - 1) * width + x_location + dimension->x - 1];

    if (x_location && y_location)
    {
        retval += matrix[(y_location - 1) * width + x_location - 1];
        retval -= matrix[(y_location - 1) * width + x_location + dimension->x - 1];
        retval -= matrix[(y_location + dimension->y - 1) * width + x_location - 1];
    }
    else if (y_location)
        retval -= matrix[(y_location - 1) * width + x_location + dimension->x - 1];
    else if (x_location)
        retval -= matrix[(y_location + dimension->y - 1) * width + x_location - 1];
    return retval;
}
