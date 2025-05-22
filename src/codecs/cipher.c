/*****************************************************************************
**
**  Implementation of the cipher codec interface
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
#include "boxing/utils.h"
#include "boxing/codecs/cipher.h"
#include "boxing/log.h"

//  DEFINES
//

#define CODEC_MEMBER(name) (((boxing_codec_cipher *)codec)->name)
#define CODEC_BASE_MEMBER(name) (((boxing_codec_cipher *)codec)->base.name)

//  CONSTANTS
//

static const char *PARAM_NAME_KEY = "key";
static const char codec_name[] = "Cipher";
static const uint32_t LFSR32_TAPS = 0xD0000001u;// Linear Feedback Shift Register

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
 *  \struct  boxing_codec_cipher_s  cipher.h
 *  \brief   Structure for storing codec cipher data.
 *
 *  \param base      boxing_codec base structure instance.
 *  \param auto_key  Sign to use auto key.
 *  \param ones      Integer array.
 *  \param key       Codec key.
 *
 *  Structure for storing codec cipher data.
 */

// PUBLIC CIPHER FUNCTIONS
//

//----------------------------------------------------------------------------
/*!
 *  \brief Create an boxing_codec_cipher instance.
 *
 *  Allocate memory for the boxing_codec_cipher type
 *  and initializes all structure data according to the input values.
 *  Return instance of allocated structure.
 *
 *  \param[in]  properties  Hash table with codec properties.
 *  \param[in]  config      Pointer to the boxing_config structure.
 *  \return instance of allocated boxing_codec_cipher structure reduced to boxing_codec structure.
 */

boxing_codec * boxing_codec_cipher_create(GHashTable * properties, const boxing_config * config)
{
    BOXING_UNUSED_PARAMETER(config);
    boxing_codec_cipher * codec = malloc(sizeof(boxing_codec_cipher));
    boxing_codec_init_base((boxing_codec *)codec);
    codec->base.free = boxing_codec_cipher_free;
    codec->base.is_error_correcting = DFALSE;
    codec->base.name = codec_name;
    codec->base.decode = codec_decode;
    codec->base.encode = codec_encode;

    if (!boxing_config_is_set(config, "FrameFormat", "type")) // DGenericFrameGpf_b1
    {
        DBOOL has_point = DFALSE;
        boxing_config_property_pointi((boxing_config *)config, "FrameBuilder", "dimensions", &has_point);

        if (!has_point)
        {
            return NULL;
        }
        else
        {
            // levels per symbol = 2
        }
    }

    // chipher key
    g_variant * key = g_hash_table_lookup(properties, PARAM_NAME_KEY);
    if (key == NULL)
    {
        DLOG_ERROR1("Requires property '%s' to be set", PARAM_NAME_KEY);
        boxing_codec_cipher_free((boxing_codec *)codec);
        return NULL;
    }

    const char * key_str = g_variant_if_string(key);
    if (key_str != NULL && strcmp(key_str, "auto") == 0)
    {
        CODEC_MEMBER(auto_key) = DTRUE;
        CODEC_MEMBER(key) = 1;
    }
    else
    {
        CODEC_MEMBER(auto_key) = DFALSE;
        CODEC_MEMBER(key) = g_variant_to_uint(key);
    }

    for (int i = 0; i < 256; i++)
    {
        unsigned char byte = i;
        CODEC_MEMBER(ones)[i] = 0;
        while (byte)
        {
            if (byte & 0x01)
            {
                CODEC_MEMBER(ones)[i]++;
            }
            byte >>= 1;
        }

    }

    return (boxing_codec *)codec;
}


//----------------------------------------------------------------------------
/*!
 *  \brief Frees occupied memory of boxing_codec_cipher structure.
 *
 *  Frees occupied memory of all internal structure pointers and structure pointer.
 *
 *  \param[in]  codec  Pointer to the boxing_codec structure.
 */

void boxing_codec_cipher_free(boxing_codec *codec)
{
    boxing_codec_release_base(codec);
    free(codec);
}


//----------------------------------------------------------------------------
/*!
  * \} end of codecs group
  */


// PRIVATE CIPHER FUNCTIONS
//

static DBOOL codec_encode_key(void * codec, gvector * data, uint32_t key)
{
    uint32_t lfsr = key;
    
    int block_size = 64;
    int data_size = (int)data->size;
    gvector * encoded_data = gvector_create(1, data_size);

    unsigned char * data_pointer = (unsigned char *)data->buffer;
    unsigned char * encoded_data_pointer = (unsigned char *)encoded_data->buffer;

    for(int i = 0; i < data_size;)
    {
        int ones = 0;
        int block_end = (i + block_size > data_size) ? data_size : i + block_size;
        for(; i < block_end; i++)
        {
            int bits = 8;
            char cipher_byte = 0;
            while(bits--)
            {
                lfsr = (lfsr >> 1) ^ ((~(lfsr & 1u)+1) & LFSR32_TAPS);
                cipher_byte = (cipher_byte << 1) | (0x1 & (lfsr >> 31));
            }
            encoded_data_pointer[i] = data_pointer[i] ^ cipher_byte;
            ones += CODEC_MEMBER(ones)[(unsigned char)encoded_data_pointer[i]];
        }
        if(ones < 8 || ones > (block_size - 1)*8)
        {
            gvector_free(encoded_data);
            return DFALSE;
        }
    }
    gvector_swap(data, encoded_data);
    gvector_free(encoded_data);
    return DTRUE;
}

static DBOOL codec_encode(void * codec, gvector * data)
{
    uint32_t max_key = CODEC_MEMBER(key) + 1;
    if(CODEC_MEMBER(auto_key))
    {
        max_key = (uint32_t)-1;
    }

    for(uint32_t key = CODEC_MEMBER(key); key < max_key; key++)
    {
        if(codec_encode_key(codec, data, key))
        {
            /* save key being used */
            CODEC_MEMBER(key) = key;
            return DTRUE;
        }
    }
    return DFALSE;
    
}

static DBOOL codec_decode(void * codec, gvector * data, gvector * erasures, boxing_stats_decode * stats, void* user_data)
{
    BOXING_UNUSED_PARAMETER( erasures );
    BOXING_UNUSED_PARAMETER( user_data );
    uint32_t m_lfsr = CODEC_MEMBER(key);

    for(unsigned int i = 0; i < data->size; i++)
    {
        int bits = 8;
        char cipher_byte = 0;
        while(bits--)
        {
            m_lfsr = (m_lfsr >> 1) ^ ((~(m_lfsr & 1u)+1) & LFSR32_TAPS);
            cipher_byte = (cipher_byte << 1) | (0x1 & (m_lfsr >> 31));
        }
        GVECTORN(data, unsigned char, i) = GVECTORN(data, unsigned char, i) ^ cipher_byte;
    }

    stats->resolved_errors = 0;
    stats->unresolved_errors = 0;
    stats->fec_accumulated_amount = 0;
    stats->fec_accumulated_weight = 0;
    return DTRUE;
}

