#ifndef BOXING_ADAPTIVE_FILTER_H
#define BOXING_ADAPTIVE_FILTER_H

/*****************************************************************************
**
**  Definition of the adaptive filter C interface
**
**  Creation date:  2018/11/26
**  Created by:     Piql AS
**
**
**  Copyright (c) 2018 Piql AS. All rights reserved.
**
**  This file is part of the boxing library
**
*****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

//  PROJECT INCLUDES
//
#include "boxing/image8.h"

//  DEFINE
//

//  TYPES
//

//  FUNCTIONS
//

void adaptive_filter(boxing_image8 * out, const boxing_image8 * in, int symbols_per_pixel, int block_width, int block_height);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
