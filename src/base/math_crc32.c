/*****************************************************************************
**
**  Implementation of the crc32 interface
**
**  Creation date:  2013/08/20
**  Created by:     Haakon Larsson
**
**
**  Copyright (c) 2016 Piql AS. All rights reserved.
**
**  This file is part of the boxing library
**
*****************************************************************************/

//  PROJECT INCLUDES
//
#include "boxing/math/crc32.h"
#include "boxing/log.h"

struct dcrc32_s
{
    boxing_uint32 crc;
    boxing_uint32 CRCTable[256];
};


/*! 
  * \addtogroup math
  * \{
  */


//----------------------------------------------------------------------------
/*!
 *  \def POLY_CRC_32() boxing/math/crc32.h
 *  \brief Macro value definition for the polynom CRC32.
 *
 *  Macro value definition for the polynom CRC32.
 *  Corresponds to the value 0x04C11DB7UL.
 */


//----------------------------------------------------------------------------
/*!
 *  \def POLY_CRC_32C() boxing/math/crc32.h
 *  \brief Macro value definition for the polynom CRC32C.
 *
 *  Macro value definition for the polynom CRC32C.
 *  Corresponds to the value 0x1EDC6F41UL.
 */


//----------------------------------------------------------------------------
/*!
 *  \def POLY_CRC_32K() boxing/math/crc32.h
 *  \brief Macro value definition for the polynom CRC32K.
 *
 *  Macro value definition for the polynom CRC32K.
 *  Corresponds to the value 0x741B8CD7UL.
 */


//----------------------------------------------------------------------------
/*!
 *  \def POLY_CRC_32Q() boxing/math/crc32.h
 *  \brief Macro value definition for the polynom CRC32Q.
 *
 *  Macro value definition for the polynom CRC32Q.
 *  Corresponds to the value 0x814141ABUL.
 */


//----------------------------------------------------------------------------
/*!
 *  \struct  dcrc32_s boxing/math/crc32.h
 *  \brief   Structure for storing CRC32 data.
 *
 *  Structure for storing CRC32 data. The parameters are hidden for this header file.
 */


// PUBLIC MATH CRC32 FUNCTIONS
//

//----------------------------------------------------------------------------
/*!
 *  \brief Create a dcrc32 instance with default values.
 *
 *  Allocate memory for the dcrc32 type
 *  and initializes all structure pointers with default values.
 *  Return instance of allocated structure.
 *
 *  \return instance of allocated dcrc32 structure.
 */

dcrc32 * boxing_math_crc32_create_def()
{
    return boxing_math_crc32_create(0, POLY_CRC_32C);
}


//----------------------------------------------------------------------------
/*!
 *  \brief Create a dcrc32 instance with given values.
 *
 *  Allocate memory for the dcrc32 type,
 *  initialize seed value with input value and calculate crc according to the given polynom.
 *  Return instance of allocated structure.
 *
 *  \param[in] seed     Seed value.
 *  \param[in] polynom  Polynom value.
 *  \return instance of allocated dcrc32 structure.
 */

dcrc32 *boxing_math_crc32_create(boxing_uint32 seed, boxing_uint32 polynom)
{
    dcrc32 *dcrc = malloc(sizeof(dcrc32));
    DFATAL(dcrc, "Out of memory");

    dcrc->crc = seed;

    // precalculate crc32 for 8 bit data
    for (unsigned int i = 0; i < 256; i++) 
    {
        int j;
        boxing_uint32 part = ((unsigned long long)i << 24);
        for (j = 0; j < 8; j++) 
        {
            if ((part) & 1UL<<31)
                part = (part << 1) ^ polynom;
            else
                part <<= 1;
        }
        dcrc->CRCTable[i] = part;
    }

    return dcrc;
}


//----------------------------------------------------------------------------
/*!
 *  \brief Frees occupied memory of dcrc32 structure.
 *
 *  Frees occupied memory of dcrc32 structure.
 *
 *  \param[in]  dcrc32  Pointer to the dcrc32 structure.
 */

void boxing_math_crc32_free(dcrc32 * dcrc32)
{
    free(dcrc32);
}


//----------------------------------------------------------------------------
/*!
 *  \brief Calculate CRC of the given data string.
 *
 *  Calculate CRC of the given data string.
 *
 *  \param[in] dcrc32  Pointer to the dcrc32 structure.
 *  \param[in] data    Data string to calculate CRC.
 *  \param[in] size    Data size.
 *  \return calculated CRC.
 */

boxing_uint32 boxing_math_crc32_calc_crc(dcrc32 * dcrc32, const char * data, unsigned int size)
{
    return (dcrc32->crc = boxing_math_crc32_calc_crc_re(dcrc32, dcrc32->crc, data, size));
}


//----------------------------------------------------------------------------
/*!
 *  \brief Calculate CRC of the given data string and seed value.
 *
 *  Calculate CRC of the given data string and seed value.
 *
 *  \param[in] dcrc32  Pointer to the dcrc32 structure.
 *  \param[in] seed    Seed value.
 *  \param[in] data    Data string to calculate CRC.
 *  \param[in] size    Data size.
 *  \return calculated CRC.
 */

boxing_uint32 boxing_math_crc32_calc_crc_re(const dcrc32 * dcrc32, boxing_uint32 seed, const char * data, unsigned int size)
{
    boxing_uint32 crc = seed;
    while(size)
    {
        crc = (crc << 8) ^ dcrc32->CRCTable[((crc >> 24) ^ *data) & 0xff];
        data++;
        size--;
    }
    return crc;
}


//----------------------------------------------------------------------------
/*!
 *  \brief Get the CRC value.
 *
 *  Get the CRC value from the dcrc32 structure.
 *
 *  \param[in] dcrc32  Pointer to the dcrc32 structure.
 *  \return CRC value.
 */

boxing_uint32 boxing_math_crc32_get_crc(dcrc32 * dcrc32)
{
    return dcrc32->crc;
}


//----------------------------------------------------------------------------
/*!
 *  \brief Set the CRC value by the given seed value.
 *
 *  Set the CRC value by the given seed value.
 *
 *  \param[in] dcrc32  Pointer to the dcrc32 structure.
 *  \param[in] seed    Seed value.
 */

void boxing_math_crc32_reset(dcrc32 * dcrc32, boxing_uint32 seed)
{
    dcrc32->crc = seed;
}


//----------------------------------------------------------------------------
/*!
  * \} end of math group
  */

/********************************** EOF *************************************/
