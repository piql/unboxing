/*****************************************************************************
**
**  Implementation of the crc64 interface
**
**  Creation date:  2012/09/25
**  Created by:     Haakon Larsson
**  Modified by:    Morgun Dmytro
**
**
**  Copyright (c) 2016 Piql AS. All rights reserved.
**
**  This file is part of the boxing library
**
*****************************************************************************/

//  PROJECT INCLUDES
//
#include "boxing/math/crc64.h"
#include "boxing/log.h"

//  PRIVATE INTERFACE
//

//per byte reverse for the 64 bit data
static inline uint64_t rev8(uint64_t a);

struct dcrc64_s
{
    boxing_uint64 crc;
    boxing_uint64 crc_table[256];
    boxing_uint64 crc_table_reversed[8][256];
};


/*! 
  * \addtogroup math
  * \{
  */


//----------------------------------------------------------------------------
/*!
 *  \def POLY_CRC_64() boxing/math/crc64.h
 *  \brief Macro value definition for the polynom CRC64.
 *
 *  Macro value definition for the polynom CRC64.
 *  Corresponds to the value 0x42F0E1EBA9EA3693ULL.
 */


//----------------------------------------------------------------------------
/*!
 *  \struct  dcrc64_s boxing/math/crc64.h
 *  \brief   Structure for storing CRC64 data.
 *
 *  Structure for storing CRC64 data. The parameters are hidden for this header file.
 */


// PUBLIC MATH CRC64 FUNCTIONS
//

//----------------------------------------------------------------------------
/*!
 *  \brief Create a dcrc64 instance with default values.
 *
 *  Allocate memory for the dcrc64 type
 *  and initializes all structure pointers with default values.
 *  Return instance of allocated structure.
 *
 *  \return instance of allocated dcrc64 structure.
 */

dcrc64 * boxing_math_crc64_create_def(void)
{
    return boxing_math_crc64_create(0, POLY_CRC_64);
}


//----------------------------------------------------------------------------
/*!
 *  \brief Create a dcrc64 instance with given values.
 *
 *  Allocate memory for the dcrc64 type,
 *  initialize seed value with input value and calculate crc according to the given polynom.
 *  Return instance of allocated structure.
 *
 *  \param[in] seed     Seed value.
 *  \param[in] polynom  Polynom value.
 *  \return instance of allocated dcrc64 structure.
 */

dcrc64 * boxing_math_crc64_create(boxing_uint64 seed, boxing_uint64 polynom)
{

    dcrc64 * dcrc = malloc(sizeof(dcrc64));
    DFATAL(dcrc, "Out of memory");

    dcrc->crc = seed;

    // precalculate crc64 for 8 bit data
    for (unsigned int i = 0; i < 256; i++)
    {
        int j;
        unsigned long long part = ((unsigned long long)i << 56);
        for (j = 0; j < 8; j++)
        {
            if ((part)& 1ULL << 63)
                part = (part << 1) ^ polynom;
            else
                part <<= 1;
        }
        dcrc->crc_table[i] = (part);
    }


    for (int n = 0; n < 256; n++) {
        boxing_uint64 temp_crc;
        temp_crc = dcrc->crc_table[n];
        dcrc->crc_table_reversed[0][n] = rev8(temp_crc);

        for (int k = 1; k < 8; k++) {
            temp_crc = dcrc->crc_table[(temp_crc >> 56) & 0xff] ^ (temp_crc << 8);
            dcrc->crc_table_reversed[k][n] = rev8(temp_crc);
        }
    }

    return dcrc;
}


//----------------------------------------------------------------------------
/*!
 *  \brief Frees occupied memory of dcrc64 structure.
 *
 *  Frees occupied memory of dcrc64 structure.
 *
 *  \param[in]  dcrc64  Pointer to the dcrc64 structure.
 */

void boxing_math_crc64_free(dcrc64 * dcrc64)
{
    free(dcrc64);
}


//----------------------------------------------------------------------------
/*!
 *  \brief Calculate CRC of the given data string.
 *
 *  Calculate CRC of the given data string.
 *
 *  \param[in] dcrc64  Pointer to the dcrc64 structure.
 *  \param[in] data    Data string to calculate CRC.
 *  \param[in] size    Data size.
 *  \return calculated CRC.
 */

boxing_uint64 boxing_math_crc64_calc_crc(dcrc64 * dcrc64, const char *data, unsigned int size)
{
    return (dcrc64->crc = boxing_math_crc64_calc_crc_re(dcrc64, dcrc64->crc, data, size));
}


//----------------------------------------------------------------------------
/*!
 *  \brief Calculate CRC of the given data string and seed value.
 *
 *  Calculate CRC of the given data string and seed value.
 *
 *  \param[in] dcrc64  Pointer to the dcrc64 structure.
 *  \param[in] seed    Seed value.
 *  \param[in] data    Data string to calculate CRC.
 *  \param[in] size    Data size.
 *  \return calculated CRC.
 */

boxing_uint64 boxing_math_crc64_calc_crc_re(const dcrc64 * dcrc64, boxing_uint64 seed, const char * data, unsigned int size)
{
    unsigned char *next = (unsigned char *)data;
    boxing_uint64 crc = seed;

    while (size && ((uintptr_t)next & 7) != 0) {
        crc = (dcrc64->crc_table[((crc >> 56) ^ *next++) & 0xff]) ^ (crc << 8);
        size--;
    }

    crc = rev8(dcrc64->crc);
    while (size >= 8) {
        crc ^= (*(uint64_t *)next);//8 byte aligned and 8 bytes at a time processed
        crc = dcrc64->crc_table_reversed[0][(crc >> 56) & 0xff] ^
            dcrc64->crc_table_reversed[1][(crc >> 48) & 0xff] ^
            dcrc64->crc_table_reversed[2][(crc >> 40) & 0xff] ^
            dcrc64->crc_table_reversed[3][(crc >> 32) & 0xff] ^
            dcrc64->crc_table_reversed[4][(crc >> 24) & 0xff] ^
            dcrc64->crc_table_reversed[5][(crc >> 16) & 0xff] ^
            dcrc64->crc_table_reversed[6][(crc >> 8) & 0xff] ^
            dcrc64->crc_table_reversed[7][(crc)& 0xff];
        next += 8;
        size -= 8;
    }
    crc = rev8(crc);

    while (size) {//no more than 8
        crc = dcrc64->crc_table[((crc >> 56) ^ *next++) & 0xff] ^ (crc << 8);
        size--;
    }
    return crc;
}


//----------------------------------------------------------------------------
/*!
 *  \brief Get the CRC value.
 *
 *  Get the CRC value from the dcrc64 structure.
 *
 *  \param[in] dcrc64  Pointer to the dcrc64 structure.
 *  \return CRC value.
 */

boxing_uint64 boxing_math_crc64_get_crc(dcrc64 * dcrc64)
{
    return dcrc64->crc;
}


//----------------------------------------------------------------------------
/*!
 *  \brief Set the CRC value by the given seed value.
 *
 *  Set the CRC value by the given seed value.
 *
 *  \param[in] dcrc64  Pointer to the dcrc64 structure.
 *  \param[in] seed    Seed value.
 */

void boxing_math_crc64_reset(dcrc64 * dcrc64, boxing_uint64 seed)
{
    dcrc64->crc = seed;
}


//----------------------------------------------------------------------------
/*!
  * \} end of math group
  */


// PRIVATE MATH CRC64 FUNCTIONS
//

static inline uint64_t rev8(uint64_t a)
{
#if defined(__GNUC__) || defined(__clang__)
    return __builtin_bswap64(a);
#else
    uint64_t m;

    m = UINT64_C(0xff00ff00ff00ff);
    a = ((a >> 8) & m) | (a & m) << 8;
    m = UINT64_C(0xffff0000ffff);
    a = ((a >> 16) & m) | (a & m) << 16;
    return a >> 32 | a << 32;
#endif
}

/********************************** EOF *************************************/
