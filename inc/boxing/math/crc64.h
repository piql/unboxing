#ifndef BOXING_MATH_CRC64_H
#define BOXING_MATH_CRC64_H

/*****************************************************************************
**
**  Definition of the CRC64 interface
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

//  BASE INCLUDES
#include "boxing/platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define POLY_CRC_64 0x42F0E1EBA9EA3693ULL
typedef struct dcrc64_s dcrc64;

dcrc64 *      boxing_math_crc64_create_def(void);
dcrc64 *      boxing_math_crc64_create(boxing_uint64 seed, boxing_uint64 polynom);
void          boxing_math_crc64_free(dcrc64 * dcrc64);
boxing_uint64 boxing_math_crc64_calc_crc(dcrc64 * dcrc64, const char * data, unsigned int size);
boxing_uint64 boxing_math_crc64_calc_crc_re(const dcrc64 * dcrc64, boxing_uint64 seed, const char * data, unsigned int size);
boxing_uint64 boxing_math_crc64_get_crc(dcrc64 * dcrc64);
void          boxing_math_crc64_reset(dcrc64 * dcrc64, boxing_uint64 seed);

//
//===================================EOF======================================

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
