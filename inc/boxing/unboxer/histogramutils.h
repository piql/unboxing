#ifndef BOXING_HISTOGRAMUTILS_H
#define BOXING_HISTOGRAMUTILS_H

/*****************************************************************************
**
**  Definition of the histogram utilities interface
**
**  Creation date:  2014/12/16
**  Created by:     Piql AS
**
**
**  Copyright (c) 2014 Piql AS. All rights reserved.
**
**  This file is part of the boxing library
**
*****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#include "boxing/utils.h"
#include "boxing/image8.h"

#define BOXING_HISTOGRAM_SIZE BOXING_PIXEL_LEVELS


typedef int* boxing_histogram; 

boxing_histogram boxing_histogram_create(void);
void             boxing_histogram_free(boxing_histogram histogram);
void             boxing_histogram_reset(boxing_histogram histogram);
void             boxing_histogram_calc_hist(boxing_histogram histogram, const boxing_image8 * image, const boxing_pointi *point, const boxing_pointi *dimension);
void             boxing_histogram_equalize(boxing_histogram histogram, int * lut, boxing_float saturation);
void             boxing_histogram_calc_limits( boxing_histogram histogram, int * min, int * max, boxing_float saturation);
int              boxing_histogram_calc_measure(boxing_histogram histogram);
int              boxing_histogram_calc_measure_minmax(boxing_histogram histogram, int *minimum, int *maximum);
int              boxing_histogram_calc_measure_full(boxing_histogram histogram, const boxing_image8 * image, boxing_pointi *point, boxing_pointi *dimension, int *minimum, int *maximum);
int              boxing_histogram_calc_measure_image(boxing_histogram histogram, const boxing_image8 * image, boxing_pointi *point, boxing_pointi *dimension);
boxing_float     boxing_histogram_calc_average(boxing_histogram histogram);
boxing_float     boxing_histogram_calc_average_image(const boxing_image8 * image, boxing_pointi *point, boxing_pointi *dimension);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
