#ifndef TEST_UNBOXING_UTILITY_H
#define TEST_UNBOXING_UTILITY_H

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

#include "boxing/unboxer_utility.h"
    
DBOOL                   boxing_unboxer_set_frame_format(boxing_unboxer_utility* utility, const char* format_name);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
