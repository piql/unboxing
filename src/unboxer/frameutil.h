#ifndef BOXING_FRAMEUTIL_H
#define BOXING_FRAMEUTIL_H

/*****************************************************************************
**
**  Definition of the frame utils interface
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

//  PROJECT INCLUDES
//
#include "boxing/unboxer/abstractframeutil.h"


typedef struct boxing_frame_util_s
{
    boxing_abstract_frame_util base;
} boxing_frame_util;


boxing_frame_util * boxing_frame_util_create(void);
void                boxing_frame_util_destroy(boxing_abstract_frame_util * util);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
