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
#include "boxing/codecs/reedsolomon.h"
#include "boxing/log.h"
#include "boxing/platform/memory.h"
#include "boxing/utils.h"

//  DEFINES
//

#define MAX_DEGREE_LIMIT 256
#define CODEC_MEMBER(name) (((boxing_codec_reedsolomon *)codec)->name)
#define CODEC_BASE_MEMBER(name) (((boxing_codec_reedsolomon *)codec)->base.name)

//  CONSTANTS
//

static const char codec_name[] = "ReedSolomon";

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
 *  \struct  boxing_codec_reedsolomon_s  reedsolomon.h
 *  \brief   Reedsolomon codec data storage.
 *  
 *  \param base          Base boxing_codec structure instance.
 *  \param rs            Pointer to the rs_codec structure.
 *  \param message_size  Message size.
 *  \param parity_size   Parity size.
 *
 *  Reedsolomon codec data storage structure description.
 */


// PUBLIC REEDSOLOMON FUNCTIONS
//

//----------------------------------------------------------------------------
/*!
 *  \brief Create a boxing_codec_reedsolomon structure instance.
 *
 *  Allocate memory for the boxing_codec_reedsolomon type
 *  and initializes data according to the input values.
 *  Return instance of allocated boxing_codec_reedsolomon structure reduced to the boxing_codec structure.
 *
 *  \param[in] properties  Properties hash table.
 *  \param[in] config      Boxing configuration.
 *  \return instance of allocated afs_toc_data_file_metadata_source structure.
 */

boxing_codec * boxing_codec_reedsolomon_create(GHashTable * properties, const boxing_config * config)
{
    BOXING_UNUSED_PARAMETER( config );
    boxing_codec_reedsolomon * codec = BOXING_MEMORY_ALLOCATE_TYPE(boxing_codec_reedsolomon);
    codec->rs = NULL;
    g_variant * message_size = g_hash_table_lookup(properties, PARAM_NAME_MESSAGE_SIZE);
    if (message_size == NULL)
    {
        DLOG_ERROR1( "(boxing_codec_reedsolomon_create) Required property '%s' not set", PARAM_NAME_MESSAGE_SIZE );
        return NULL;
    }

    g_variant * parity_size = g_hash_table_lookup(properties, PARAM_NAME_PARITY_SIZE);
    if (parity_size == NULL)
    {
        DLOG_ERROR1( "(boxing_codec_reedsolomon_create) Required property '%s' not set", PARAM_NAME_PARITY_SIZE );
        return NULL;
    }

    boxing_codec_init_base((boxing_codec *)codec);
    codec->base.free = boxing_codec_reedsolomon_free;
    codec->base.is_error_correcting = DTRUE;
    codec->base.name = codec_name;
    codec->message_size = g_variant_to_uint(message_size);
    codec->parity_size = g_variant_to_uint(parity_size);
    codec->base.decoded_block_size = codec->message_size;
    codec->base.encoded_block_size = codec->message_size + codec->parity_size;
    codec->base.decode = codec_decode;
    codec->base.encode = codec_encode;
    codec->rs = rs_create(codec->message_size, codec->parity_size, RS_PRIM_POLY_285);
    if (!codec->rs)
    {
        DLOG_ERROR("(boxing_codec_reedsolomon_create) Codec creation failed");
        boxing_codec_reedsolomon_free((boxing_codec *)codec);
        return NULL;
    }

    return (boxing_codec *)codec;
}


//----------------------------------------------------------------------------
/*!
 *  \brief Frees occupied memory of boxing_codec_reedsolomon structure.
 *
 *  Frees occupied memory of all internal structure pointers and structure pointer.
 *
 *  \param[in]  codec  Pointer to the boxing_codec structure which was reduces from boxing_codec_reedsolomon structure.
 */

void boxing_codec_reedsolomon_free(boxing_codec * codec)
{
    boxing_codec_release_base(codec);
    rs_free(((boxing_codec_reedsolomon *)codec)->rs);
    boxing_memory_free(codec);
}


//----------------------------------------------------------------------------
/*!
  * \} end of codecs group
  */


// PRIVATE REEDSOLOMON FUNCTIONS
//

static DBOOL codec_encode(void * codec, gvector * data)
{
    uint32_t message_size = CODEC_MEMBER(message_size);
    uint32_t parity_size = CODEC_MEMBER(parity_size);

    uint32_t new_size = (((uint32_t)data->size / message_size) +
        ((uint32_t)data->size % message_size == 0 ? 0 : 1)) * (message_size + parity_size);
    gvector * data_encode = gvector_create_char(new_size, 0x00);


    rs_encode(((boxing_codec_reedsolomon *)codec)->rs, data, data_encode);

    gvector_swap(data, data_encode);
    gvector_free(data_encode);


    return DTRUE;
}

static DBOOL codec_decode(void * codec, gvector * data, gvector * erasures, boxing_stats_decode * stats, void* user_data)
{
    BOXING_UNUSED_PARAMETER(erasures);
    BOXING_UNUSED_PARAMETER(user_data);

    uint32_t message_size = CODEC_MEMBER(message_size);
    uint32_t parity_size = CODEC_MEMBER(parity_size);

    uint32_t block_size = message_size + parity_size;

    // let's trunckate the input data to N * block_size where N is type int
    uint32_t block_count = (uint32_t)data->size / block_size;
    gvector_resize(data, block_count * block_size);

    gvector * data_decode = gvector_create_char((uint32_t)block_count * message_size, 0x00);

    uint8_t * data_decode_pointer = (uint8_t *)data_decode->buffer;
    uint8_t * data_pointer = (uint8_t *)data->buffer;

    int max_errors_per_block = 0;

    int32_t data_size = (int32_t)data->size;
    unsigned int errors_recovered = 0;
    unsigned int errors_fatal = 0;
    boxing_float fec_amount = -1.0f;
    boxing_float fec_weight = -1.0f;

    if (CODEC_BASE_MEMBER(decode_cb))
    {
        if (data_size)
        {
            int decoded_size = (int)data_decode->size;

            unsigned int reedsolomondata[2] = { message_size, parity_size };
            int rs_errors_recovered = 0;
            int rs_errors_fatal = 0;
            int ret = CODEC_BASE_MEMBER(decode_cb)(user_data, reedsolomondata, data_pointer, data_size, data_decode_pointer, &decoded_size, &rs_errors_recovered, &rs_errors_fatal, &fec_amount, &fec_weight);
            errors_recovered = rs_errors_recovered;
            errors_fatal = (ret != DFALSE) ? rs_errors_fatal : 1;
            stats->resolved_errors = errors_recovered;
            stats->unresolved_errors = errors_fatal;
            stats->fec_accumulated_amount = fec_amount * fec_weight;
            stats->fec_accumulated_weight = fec_weight;
        }
    }
    else
    {
        rs_decode(((boxing_codec_reedsolomon *)codec)->rs, data, data_decode, &errors_recovered, &errors_fatal, &max_errors_per_block);

        stats->resolved_errors = errors_recovered;
        stats->unresolved_errors = errors_fatal;
        stats->fec_accumulated_weight = parity_size / (boxing_float)block_size;
        stats->fec_accumulated_amount = stats->fec_accumulated_weight *
            ((max_errors_per_block * 2 > (int)parity_size) ? (boxing_float)1.0f : (max_errors_per_block * 2 / (float)parity_size));
    }

    gvector_swap(data, data_decode);
    gvector_free(data_decode);



    return errors_fatal == 0;
}
