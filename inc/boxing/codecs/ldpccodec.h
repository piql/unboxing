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

typedef struct ldpc_generator_s
{
    mod2sparse *H;	/**< Parity check matrix */

    int M;		    /**< Number of rows in parity check matrix */
    int N;		    /**< Number of columns in parity check matrix */
                    
    char type;	    /**< Type of generator matrix representation */
    int *cols;	    /**< Ordering of columns in generator matrix */
    int *rows;	    /**< Ordering of rows in generator matrix (type 's') */
    mod2sparse *L;  /**< Sparse L decomposition, if type=='s' */
    mod2sparse *U;  /**< Sparse U decomposition, if type=='s' */

    mod2dense *G;	/**< Dense or mixed representation of generator matrix,
                    if type=='d' or type=='m' */
}ldpc_generator;

typedef ldpc_generator generator_matrix;


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
    generator_matrix * gen_matrix;
} boxing_codec_ldpc;

boxing_codec * boxing_codec_ldpc_create(GHashTable * properties, const boxing_config * config);
void           boxing_codec_ldpc_free(boxing_codec *codec);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
