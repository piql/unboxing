#ifndef BOXING_HORIZONTALMEASURES_H
#define BOXING_HORIZONTALMEASURES_H

/*****************************************************************************
**
**  Definition of the horizontal measures interface
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

#ifdef __cplusplus
extern "C" {
#endif

#include "boxing/image8.h"
#include "boxing/utils.h"
#include "boxing/matrix.h"

boxing_matrix_float * boxing_calculate_thresholds(const boxing_image8 * image, int block_width, int block_height, int cluster_count);
boxing_matrix_float * boxing_calculate_means(const boxing_image8 * image, int block_width, int block_height, int cluster_count);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
