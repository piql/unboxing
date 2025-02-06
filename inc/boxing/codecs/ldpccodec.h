#ifndef BOXING_LDPCCODEC_H
#define BOXING_LDPCCODEC_H

/*****************************************************************************
**
**  Definition of BCH codec functions
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

#include "boxing/codecs/codecbase.h"
#include "boxing/platform/types.h"


/*! 
  * \ingroup codecs
  * \{
  */


//----------------------------------------------------------------------------
/*!
 *  \var    codec_ldpc_name
 *  \brief  Ldpc codec name constant.
 *  
 *  Ldpc codec name constant.
 */
static const char codec_ldpc_name[] = "LDPC";


//----------------------------------------------------------------------------
/*!
 *  \struct     boxing_codec_ldpc_s  ldpccodec.h
 *  \brief      Ldpc codec data storage.
 *
 *  \param base        Base boxing_codec instance.
 *  \param iterations  Iteration.
 *  \param gen_matrix  Generator matrix.
 *
 *  Structure for storing ldpc codec data.
 */
typedef struct boxing_codec_ldpc_s
{
    boxing_codec       base;
    unsigned int       iterations;
    void * pchk_matrix;
    void * gen_matrix;
} boxing_codec_ldpc;

boxing_codec * boxing_codec_ldpc_create(GHashTable * properties, const boxing_config * config);
void           boxing_codec_ldpc_free(boxing_codec *codec);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
