/*****************************************************************************
**
**  Implementation of the horizontal measures interface
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
#include "horizontalmeasures.h"
#include "boxing/platform/memory.h"
#include "boxing/platform/types.h"

//  DEFINES
//

#define KMEANS_ITERATIONS      6
#define KMEANS_HISTOGRAM_SIZE  256

typedef struct boxing_cluster_s
{
    int    sum;
    int    count;
} boxing_cluster;

//  PRIVATE INTERFACE
//
static void calculate_means(const boxing_image8 *image, int x, int y, int width, int height, boxing_float* means, boxing_float* variances, unsigned int means_size);
static void kmeans(int * sampels, int sampels_size, float *means, int means_size, int iterations);
static void calculate_inital_means(boxing_float* means, int means_size, int * histogram, int histogram_size, int histogram_samples);
static int  sample_histogram(const boxing_image8 *image, int x, int y, int width, int height, int *histogram);

/*! 
  * \addtogroup unboxer
  * \{
  */

// PUBLIC HORIZONTAL MEASURES FUNCTIONS
//

//----------------------------------------------------------------------------
/*!
 *  \brief Calculate thresholds.
 *
 *  boxing_calculate_thresholds divides the input image into blocks of size block_width and block_height. 
 *  Then it will generate a local histograms for each set of adjoining blocks and use this histogram to 
 *  run it through a k-means clustering algorithm to generate a given number of thresholds. The number of
 *  thresholds per block is cluster_count - 1. The returning theshold is a (cluster_count-1) x M x N matrix.
 *
 *  \param[in]  image           Image to be analysed.
 *  \param[in]  block_width     Sub image width.
 *  \param[out] block_height    Sub image height.
 *  \param[out] cluster_count   The number of symols being searched for in the local histograms.
 *  \return Calculated thresholds.
 */

boxing_matrix_float * boxing_calculate_thresholds(const boxing_image8 * image, int block_width, int block_height, int cluster_count)
{

    boxing_matrix_float *centeroids =  boxing_calculate_means(image, block_width, block_height, cluster_count);
    boxing_matrix_float *thresholds = boxing_matrix_float_multipage_create(centeroids->rows, cluster_count - 1, centeroids->pages);
    for (unsigned int n = 0; n < thresholds->pages; n++)
    {
        for (unsigned int m = 0; m < thresholds->rows; m++)
        {
            boxing_float * th = MATRIX_MULTIPAGE_ROW_PTR(thresholds, m, n);
            boxing_float * center = MATRIX_MULTIPAGE_ROW_PTR(centeroids, m, n);

            for (unsigned int i = 0; i < thresholds->cols; i++)
            {
                th[i] = (center[i] + center[i + 1]) / 2;
            }
        }
    }

    boxing_matrix_float_free(centeroids);

    return thresholds;
}

//----------------------------------------------------------------------------
/*!
 *  \brief Calculate means
 *
 *  boxing_calculate_means divides the input image into blocks of size block_width and block_height.
 *  Then it will generate a local histograms for each set of adjoining blocks and use this histogram to
 *  run it through a k-means clustering algorithm to generate a given number of means. The number of
 *  means per block is cluster_count. The returning theshold is a cluster_count x M x N matrix.
 *
 *  \param[in]  image           Image to be analysed.
 *  \param[in]  block_width     Sub image width.
 *  \param[out] block_height    Sub image height.
 *  \param[out] cluster_count   The number of symols being searched for in the local histograms.
 *  \return Calculated thresholds.
 */

boxing_matrix_float * boxing_calculate_means(const boxing_image8 * image, int block_width, int block_height, int cluster_count)
{
    int width = image->width;
    int height = image->height;
    int horizontal_block_count = (width + block_width - 1) / block_width;
    int vertical_block_count = (height + block_height - 1) / block_height;

    boxing_matrix_float *centeroids = boxing_matrix_float_multipage_create(horizontal_block_count,  cluster_count * 2, vertical_block_count);

    for (int n = 0; n < vertical_block_count; n++)
    {
        for (int m = 0; m < horizontal_block_count; m++)
        {
            unsigned int x = m * block_width;
            if ((x + block_width) >= (unsigned int)image->width)
                x = image->width - block_width;

            unsigned int y = n * block_height;
            if ((y + block_height) >= (unsigned int)image->height)
                y = image->height - block_height;

            boxing_float * means = MATRIX_MULTIPAGE_ROW_PTR(centeroids, m, n);
            boxing_float * variances = means + cluster_count;
            calculate_means(image, x, y, block_width, block_height, means, variances, cluster_count);
        }
    }

    return centeroids;
}


//----------------------------------------------------------------------------
/*!
  * \} end of unboxer group
  */


// PRIVATE HORIZONTAL MEASURES FUNCTIONS
//

static int sample_histogram(const boxing_image8 *image, int x, int y, int width, int height, int *histogram)
{
    int min_i = BOXING_PIXEL_MAX;
    int max_i = BOXING_PIXEL_MIN;
    int y_end = y + height;
    int x_end = x + width;
    int count = 0;

    // find max and min
    for (int y_pos = y; y_pos < y_end; y_pos++)
    {
        const boxing_image8_pixel *pixel = IMAGE8_PPIXEL(image, x, y_pos);

        if (min_i == BOXING_PIXEL_MIN && max_i == BOXING_PIXEL_MAX)
        {
            for (int x_pos = x; x_pos < x_end; x_pos++, pixel++)
            {
                histogram[*pixel]++;
                count++;
            }
        }
        else if (min_i == BOXING_PIXEL_MIN)
        {
            for (int x_pos = x; x_pos < x_end; x_pos++, pixel++)
            {
                histogram[*pixel]++;
                count++;
                if (max_i < *pixel)
                    max_i = *pixel;
            }
        }
        else if (max_i == BOXING_PIXEL_MAX)
        {
            for (int x_pos = x; x_pos < x_end; x_pos++, pixel++)
            {
                histogram[*pixel]++;
                count++;
                if (min_i > *pixel)
                    min_i = *pixel;
            }
        }
        else
        {
            for (int x_pos = x; x_pos < x_end; x_pos++, pixel++)
            {
                histogram[*pixel]++;
                count++;
                if (min_i > *pixel)
                    min_i = *pixel;
                else if (max_i < *pixel)
                    max_i = *pixel;
            }
        }
    }

    return count;
}

static void calulate_min_distance(boxing_float center, boxing_float* min_distance, int * histogram, int histogram_size, int histogram_samples)
{
    for (int n = 0; n < histogram_size; n++)
    {
        if (histogram[n])
        {
            min_distance[n] = center - n;
            min_distance[n] *= min_distance[n] * histogram[n] / histogram_samples;
        }
        else
            min_distance[n] = BOXING_FLOAT_MAX;
    }
}

static int update_min_distance(boxing_float* min_distance, boxing_float* distance, int size)
{
    int pos = 0;
    for (int n = 0; n < size; n++)
    {
        if (min_distance[n] > distance[n])
        {
            min_distance[n] = distance[n];
        }

        if (min_distance[pos] < min_distance[n])
        {
            pos = n;
        }
    }
    return pos;
}

static void calculate_inital_means(boxing_float* means, int means_size, int * histogram, int histogram_size, int histogram_samples)
{
    int * histogram_copy = BOXING_STACK_ALLOCATE_TYPE_ARRAY(int, histogram_size);

    memcpy(histogram_copy, histogram, histogram_size*sizeof(int));

    int i = 0;
    int h = 0;
    means[i] = 0;
    for (int n = 0; n < histogram_samples / means_size;)
    {
        if (h >(histogram_size - 1))
        {
            break;
        }

        if (histogram_copy[h])
        {
            means[i] += h;
            histogram_copy[h]--;
            n++;
        }
        else
        {
            h++;
        }
    }
    means[i] /= histogram_samples / means_size;
    BOXING_STACK_FREE( histogram_copy );    
    
    boxing_float * min_distance = BOXING_STACK_ALLOCATE_TYPE_ARRAY(boxing_float, histogram_size);
    for (int i = 0; i < histogram_size; i++)
    {
        if (histogram[i])
            min_distance[i] = BOXING_FLOAT_MAX;
        else
            min_distance[i] = 0;
    }

    boxing_float * distance = BOXING_STACK_ALLOCATE_TYPE_ARRAY (boxing_float, histogram_size);

    for (int k = 1; k < means_size; k++)
    {
        calulate_min_distance(means[k-1], distance, histogram, histogram_size, histogram_samples);
        means[k] = (boxing_float)update_min_distance(min_distance, distance, histogram_size);
    }
    BOXING_STACK_FREE( distance );    
    BOXING_STACK_FREE( min_distance );    
}


static void calc_variances(int * sampels, int sampels_size, float *means, int means_size, float *variances)
{
    boxing_cluster * var = BOXING_STACK_ALLOCATE_TYPE_ARRAY(boxing_cluster, means_size);

    boxing_memory_clear(var, sizeof(boxing_cluster) * means_size);
    --means_size;

    for (int sample_index = 0; sample_index < sampels_size; sample_index++)
    {
        boxing_float min_distance = BOXING_FLOAT_MAX;
        int index = 0;
        boxing_float sample = (boxing_float)sample_index;
        for (int i = 0; i <= means_size; i++)
        {
            boxing_float distance = fabsf(sample - means[i]);
            if (min_distance > distance)
            {
                index = i;
                min_distance = distance;
            }
        }
        int count = sampels[sample_index];
        var[index].sum += (int)(min_distance * min_distance * count);
        var[index].count += count;
    }

    for (int i = means_size; i >= 0; i--)
    {
        variances[i] = (boxing_float)var[i].sum / (var[i].count - 1);
        if (variances[i] < 0.1e-10f)
        {
            variances[i] = 0.1e-10f;
        }
    }
    
    BOXING_STACK_FREE( var );
}

static void kmeans(int * sampels, int sampels_size, float *means, int means_size, int iterations)
{
    boxing_cluster * clusters = BOXING_STACK_ALLOCATE_TYPE_ARRAY(boxing_cluster, means_size);

    while (iterations--)
    {
        boxing_memory_clear(clusters, sizeof(boxing_cluster) * means_size);

        for (int sample_index = 0; sample_index < sampels_size; sample_index++)
        {
            boxing_float min_distance = BOXING_FLOAT_MAX;
            int index = 0;
            boxing_float sample = (boxing_float)sample_index;
            for (int i = 0; i < means_size; i++)
            {
                boxing_float distance = fabsf(sample - means[i]);
                if (min_distance > distance)
                {
                    index = i;
                    min_distance = distance;
                }
            }
            int count = sampels[sample_index];
            clusters[index].sum += sample_index * count;
            clusters[index].count += count;
        }

        for (int i = means_size-1; i >= 0; i--)
        {
            means[i] = (boxing_float)clusters[i].sum / clusters[i].count;
        }
    }

    BOXING_STACK_FREE( clusters );
}

int qsort_compare(const void *a, const void *b) 
{
    if (*(boxing_float*)a > *(boxing_float*)b)
        return 1;
    else if (*(boxing_float*)a < *(boxing_float*)b)
        return -1;
    else
        return 0;
}

static void calculate_means(const boxing_image8 *image, int x, int y, int width, int height, boxing_float* means, boxing_float* variances, unsigned int means_size)
{
    const int  iterations = 6;
    const int histogram_size = 256;
    int * histogram  = BOXING_STACK_ALLOCATE_TYPE_ARRAY(int, histogram_size);

    boxing_memory_clear(histogram, histogram_size * sizeof(int));

    int samples = sample_histogram(image, x, y, width, height, histogram);

    // calculate initial means
    calculate_inital_means(means, means_size, histogram, histogram_size, samples);

    kmeans(histogram, 256, means, means_size, iterations);

    qsort(means, means_size, sizeof(boxing_float), qsort_compare);

    if (variances)
        calc_variances(histogram, 256, means, means_size, variances);

    BOXING_STACK_FREE( histogram );    
}

