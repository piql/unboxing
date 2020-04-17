#ifndef UNBOXING_UTILITY_H
#define UNBOXING_UTILITY_H

/*****************************************************************************
**
**  Implementation of the unboxing utility
**
**  Creation date:  2017/04/05
**  Created by:     Aleksandr Ivanov
**
**
**  Copyright (c) 2017 Piql AS. All rights reserved.
**
**  This file is part of the boxing library
**
*****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#include "boxing/unboxer.h"
#include "boxing/config.h"

typedef struct boxing_unboxer_utility_s
{
    boxing_unboxer *            unboxer;
    boxing_unboxer_parameters * parameters;
} boxing_unboxer_utility;

#ifdef BOXINGLIB_CALLBACK
boxing_unboxer_utility* boxing_unboxer_utility_create(boxing_config* format, DBOOL is_raw, boxing_all_complete_cb on_all_complete, boxing_metadata_complete_cb  on_metadata_complete);
#else
boxing_unboxer_utility* boxing_unboxer_utility_create(boxing_config* format, DBOOL is_raw);
#endif

int                     boxing_unboxer_utility_unbox(boxing_unboxer_utility* utility, boxing_image8* input_image, gvector* output_data);
void                    boxing_unboxer_utility_free(boxing_unboxer_utility* utility);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
