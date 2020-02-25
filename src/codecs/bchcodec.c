/*****************************************************************************
**
**  Implementation of the reed solomon codec interface
**
**  Creation date:  2016/06/28
**  Created by:     Piql
**
**
**  Copyright (c) 2016 Piql AS. All rights reserved.
**
**  This file is part of the boxing library
**
*****************************************************************************/

//  PROJECT INCLUDES
//
#include "boxing/codecs/bchcodec.h"
#include "boxing/log.h"
#include "boxing/platform/memory.h"
#include "boxing/utils.h"

//  DEFINES
//

#define MAX_DEGREE_LIMIT 256
#define CODEC_MEMBER(name) (((boxing_codec_bch *)codec)->name)
#define CODEC_BASE_MEMBER(name) (((boxing_codec_bch *)codec)->base.name)


#define BCH_PRIME_POLY_M14
#define BCH_PRIME_POLY_

//  CONSTANTS
//


//  PRIVATE INTERFACE
//

static DBOOL codec_encode(void * codec, gvector * data);
static DBOOL codec_decode(void * codec, gvector * data, gvector * erasures, boxing_stats_decode * stats, void* user_data);


/*! 
  * \addtogroup codecs
  * \{
  */


//----------------------------------------------------------------------------
/*!
 *  \struct     boxing_codec_bch_s  bchcodec.h
 *  \brief      Bch codec data storage.
 *
 *  \param base          Base boxing_codec instance.
 *  \param bch           Bch control.
 *  \param message_size  Message size.
 *  \param parity_size   Parity size.
 *  \param m             Description m parameter.
 *  \param t             Description tparameter.
 *  \param poly          Polynom.
 *
 *  Structure for storing bch codec data.
 */


// PUBLIC bch FUNCTIONS
//

//----------------------------------------------------------------------------
/*!
 *  \brief Create an boxing_codec_bch instance.
 *
 *  Allocate memory for the boxing_codec_bch type
 *  and initializes all structure data according to the input values.
 *  Return instance of allocated structure.
 *
 *  \param[in]  properties  Hash table with codec properties.
 *  \param[in]  config      Pointer to the boxing_config structure.
 *  \return instance of allocated boxing_codec_bch structure reduced to boxing_codec structure.
 */

boxing_codec * boxing_codec_bch_create(GHashTable * properties, const boxing_config * config)
{
    BOXING_UNUSED_PARAMETER( config );
    boxing_codec_bch * codec = BOXING_MEMORY_ALLOCATE_TYPE(boxing_codec_bch);
    codec->bch = NULL;
    g_variant * message_size = g_hash_table_lookup(properties, PARAM_NAME_MESSAGE_SIZE);
    if (message_size == NULL)
    {
        DLOG_ERROR("Requires property 'messageSize' to be set");
        return NULL;
    }

    g_variant * parity_size = g_hash_table_lookup(properties, PARAM_NAME_PARITY_SIZE);
    if (parity_size == NULL)
    {
        DLOG_ERROR("Requires property 'byteParityNumber' to be set");
        return NULL;
    }


    codec->poly = 0; // use default poly
    g_variant * poly = g_hash_table_lookup(properties, PARAM_NAME_POLYNOM);
    if (poly)
    {
        codec->poly = g_variant_to_uint(poly);
    }

    boxing_codec_init_base((boxing_codec *)codec);
    codec->base.free = boxing_codec_bch_free;
    codec->base.is_error_correcting = DTRUE;
    codec->base.name = codec_bch_name;
    codec->message_size = g_variant_to_uint(message_size);
    codec->parity_size = g_variant_to_uint(parity_size);

    codec->base.decoded_block_size = codec->message_size;
    codec->base.encoded_block_size = codec->message_size + codec->parity_size;
    codec->base.decode = codec_decode;
    codec->base.encode = codec_encode;

    int poly_dgree = deg(codec->poly);
    codec->bch = init_bch(poly_dgree, 8 * codec->parity_size / poly_dgree, codec->poly);
    if (!codec->bch)
    {
        DLOG_ERROR("Creating BCH failed");
        boxing_codec_bch_free((boxing_codec *)codec);
        return NULL;
    }
    
    if (codec->bch->n / 8 - codec->bch->ecc_bytes < codec->message_size)
    {
        DLOG_ERROR("Creating BCH failed");
        boxing_codec_bch_free((boxing_codec *)codec);
        return NULL;
    }    

    return (boxing_codec *)codec;
}


//----------------------------------------------------------------------------
/*!
 *  \brief Frees occupied memory of boxing_codec_bch structure.
 *
 *  Frees occupied memory of all internal structure pointers and structure pointer.
 *
 *  \param[in]  codec  Pointer to the boxing_codec structure.
 */

void boxing_codec_bch_free(boxing_codec *codec)
{
    boxing_codec_release_base(codec);
    free_bch(((boxing_codec_bch *)codec)->bch);
    boxing_memory_free(codec);
}


//----------------------------------------------------------------------------
/*!
  * \} end of codecs group
  */


// PRIVATE BCH CODEC FUNCTIONS
//

static DBOOL codec_encode(void * codec, gvector * data)
{
    uint32_t message_size = CODEC_MEMBER(message_size);
    uint32_t parity_size = CODEC_MEMBER(parity_size);
    uint32_t block_size = message_size + parity_size;
    struct bch_control * bch = CODEC_MEMBER(bch);

    uint32_t block_count = (((uint32_t)data->size / message_size) + ((uint32_t)data->size % message_size == 0 ? 0 : 1));
    gvector * data_encode = gvector_create_char(block_count * block_size, 0x00);


    const uint8_t *data_src = data->buffer;
    uint8_t *data_dst = data_encode->buffer;
    uint8_t *data_end = data_dst + block_count * block_size;

    for (; data_dst < data_end; data_dst += block_size, data_src += message_size)
    {
        memcpy(data_dst, data_src, message_size);
        encode_bch(bch, data_dst, message_size, data_dst + message_size);
    }

    gvector_swap(data, data_encode);
    gvector_free(data_encode);


    return DTRUE;
}


static DBOOL codec_decode_blocks(struct bch_control * bch, const uint8_t * src, uint8_t * dst, 
   unsigned int message_size, unsigned int block_size, unsigned int blocks, unsigned int *resolved_errors, unsigned int *unresolved_errors, unsigned int *max_errors_per_block)
{
    DBOOL success = DTRUE;

    *unresolved_errors = 0;
    *resolved_errors = 0;
    *max_errors_per_block = 0;

    unsigned int * errloc = BOXING_STACK_ALLOCATE_TYPE_ARRAY(unsigned int, block_size);
    const uint8_t * src_end = src + blocks * block_size;
    for (; src < src_end; src += block_size, dst += message_size)
    {
        memcpy(dst, src, message_size);

        int errors = decode_bch(bch, src, message_size, src + message_size, NULL, NULL, errloc);

        if ((errors < 0) || (errors >= (int)bch->ecc_bits))
        {
            *resolved_errors += 0;
            *unresolved_errors += (int)bch->ecc_bits;
            *max_errors_per_block = (int)bch->ecc_bits;
            success = DFALSE;
        }
        else if(errors > 0)
        {
            for (int n = 0; n < errors; n++)
            {
                if (errloc[n] < message_size * 8)
                {
                    dst[errloc[n] / 8] ^= 1 << (errloc[n] % 8);
                }
            }

            *resolved_errors += errors;
            *unresolved_errors += 0;
            if ((unsigned int)errors > *max_errors_per_block)
            {
                *max_errors_per_block = errors;
            }
        }

    }

    BOXING_STACK_FREE( errloc );
    return success;
}


static DBOOL codec_decode(void * codec, gvector * data, gvector * erasures, boxing_stats_decode * stats, void* user_data)
{
    BOXING_UNUSED_PARAMETER(erasures);
    BOXING_UNUSED_PARAMETER(user_data);

    uint32_t message_size = CODEC_MEMBER(message_size);
    uint32_t parity_size = CODEC_MEMBER(parity_size);
    struct bch_control * bch = CODEC_MEMBER(bch);
    uint32_t block_size = message_size + parity_size;

    // let's trunckate the input data to N * block_size where N is type int
    uint32_t block_count = (uint32_t)data->size / block_size;
    gvector_resize(data, block_count * block_size);

    gvector * data_decode = gvector_create_char((uint32_t)block_count * message_size, 0x00);

    unsigned int max_errors_per_block = 0;
    unsigned int errors_recovered = 0;
    unsigned int errors_fatal = 0;

    codec_decode_blocks(bch, (uint8_t *)data->buffer, (uint8_t *)data_decode->buffer, message_size, block_size, block_count, &errors_recovered, &errors_fatal, &max_errors_per_block);

    stats->resolved_errors = errors_recovered;
    stats->unresolved_errors = errors_fatal;
    stats->fec_accumulated_weight = parity_size / (boxing_float)block_size;
    stats->fec_accumulated_amount = stats->fec_accumulated_weight *
        ((max_errors_per_block * 2 > parity_size) ? (boxing_float)1.0f : (max_errors_per_block * 2 / (float)parity_size));

    gvector_swap(data, data_decode);
    gvector_free(data_decode);

    return errors_fatal == 0;
}
