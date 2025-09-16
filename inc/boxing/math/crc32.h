#ifndef BOXING_MATH_CRC32_H
#define BOXING_MATH_CRC32_H

/*****************************************************************************
**
**  Definition of the CRC32 interface
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

//  BASE INCLUDES
#include "boxing/platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define POLY_CRC_32  0x04C11DB7UL
#define POLY_CRC_32C 0x1EDC6F41UL
#define POLY_CRC_32K 0x741B8CD7UL
#define POLY_CRC_32Q 0x814141ABUL

typedef struct dcrc32_s dcrc32;

dcrc32 *           boxing_math_crc32_create_def(void);
dcrc32 *           boxing_math_crc32_create(boxing_uint32 seed, boxing_uint32 polynom);
void               boxing_math_crc32_free(dcrc32 * dcrc32);
boxing_uint32      boxing_math_crc32_calc_crc(dcrc32 * dcrc32, const char * data, unsigned int size);
boxing_uint32      boxing_math_crc32_calc_crc_re(const dcrc32 * dcrc32, boxing_uint32 seed, const char * data, unsigned int size);
boxing_uint32      boxing_math_crc32_get_crc(dcrc32 * dcrc32);
void               boxing_math_crc32_reset(dcrc32 * dcrc32, boxing_uint32 seed);

#ifdef __cplusplus
} /* extern "C" */
#endif

//
//===================================EOF======================================

#endif
