/*****************************************************************************
**
**  Implementation of the symbol converter codec interface
**
**  Creation date:  2017/09/27
**  Created by:     Piql
**
**
**  Copyright (c) 2017 Piql AS. All rights reserved.
**
**  This file is part of the boxing library
**
*****************************************************************************/

//  PROJECT INCLUDES
//
#include "boxing/codecs/symbolconverter.h"
#include "boxing/log.h"
#include "boxing/utils.h"

//  DEFINES
//

#define CODEC_MEMBER(name) (((boxing_codec_byte_stream_encoder *)codec)->name)
#define CODEC_BASE_MEMBER(name) (((boxing_codec_byte_stream_encoder *)codec)->base.name)

//  CONSTANTS
//

//  PRIVATE INTERFACE
//

static DBOOL codec_decode(void * codec, gvector * data, gvector * erasures, boxing_stats_decode * stats, void* user_data);
static DBOOL codec_encode(void * codec, gvector * data);


/*! 
  * \addtogroup codecs
  * \{
  */


//----------------------------------------------------------------------------
/*!
 *  \struct     boxing_codec_symbol_converter_s  symbolconverter.h
 *  \brief      Codec symbol converter data storage.
 *  
 *  \param base                       Base boxing_codec structure instance.
 *  \param encoded_levels_per_symbol  Encoded levels per symbol.
 *  \param decoded_levels_per_symbol  Decoded levels per symbol.
 *
 *  Codec symbol converter data storage structure description.
 */


// PUBLIC MODULATOR FUNCTIONS
//

//----------------------------------------------------------------------------
/*!
 *  \brief Create a boxing_codec_symbol_converter structure instance.
 *
 *  Allocate memory for the boxing_codec_symbol_converter type
 *  and initializes all structure pointers with default values.
 *  Return instance of allocated structure.
 *
 *  \return instance of allocated boxing_codec_symbol_converter structure.
 */

boxing_codec * boxing_codec_symbol_converter_create(GHashTable * properties, const boxing_config * config)
{
    BOXING_UNUSED_PARAMETER( config );
    BOXING_UNUSED_PARAMETER(properties);
    boxing_codec_symbol_converter * codec = malloc(sizeof(boxing_codec_symbol_converter));
    boxing_codec_init_base((boxing_codec *)codec);
    codec->base.free = boxing_codec_symbol_converter_free;
    codec->base.is_error_correcting = DFALSE;
    codec->base.name = codec_symbol_converter_name;
    codec->base.decode = codec_decode;
    codec->base.encode = codec_encode;

    codec->base.encoded_symbol_size = 5;
    codec->base.decoded_symbol_size = 8;

    codec->base.decoded_block_size = codec->base.encoded_symbol_size;
    codec->base.encoded_block_size = codec->base.decoded_symbol_size;
    return (boxing_codec *)codec;
}


//----------------------------------------------------------------------------
/*!
 *  \brief Frees occupied memory of boxing_codec_symbol_converter structure.
 *
 *  Frees occupied memory of all internal structure pointers and structure pointer.
 *
 *  \param[in]  codec  Pointer to the boxing_codec structure.
 */

void boxing_codec_symbol_converter_free(boxing_codec * codec)
{
    boxing_codec_release_base(codec);
    free(codec);
}


//----------------------------------------------------------------------------
/*!
  * \} end of codecs group
  */


// PRIVATE MODULATOR FUNCTIONS
//

static DBOOL codec_decode(void * codec, gvector * data, gvector * erasures, boxing_stats_decode * stats, void* user_data)
{
    BOXING_UNUSED_PARAMETER(erasures);
    BOXING_UNUSED_PARAMETER(stats);
    BOXING_UNUSED_PARAMETER(codec);
    BOXING_UNUSED_PARAMETER(user_data);

    size_t size = data->size / 8;
    gvector * decoded = gvector_create_char(size * 5, 0);
    uint32_t      *src = (uint32_t *)data->buffer;
    unsigned char *dst = (unsigned char *)decoded->buffer;

    for (size_t i = 0; i < size; i++)
    {
        uint32_t value = *(uint32_t *)src;

        value = 
            (0xff000000 & value) >> 24 |
            (0x00ff0000 & value) >> 8 |
            (0x0000ff00 & value) << 8 |
            (0x000000ff & value) << 24;

        value =
            (0x1f000000 & value) << 3 |
            (0x001f0000 & value) << 6 |
            (0x00001f00 & value) << 9 |
            (0x0000001f & value) << 12;

        value =
            (0xff000000 & value) >> 24 |
            (0x00ff0000 & value) >> 8 |
            (0x0000ff00 & value) << 8 |
            (0x000000ff & value) << 24;

        *(uint32_t*)dst = value;

        dst += 1;
        src ++;

        value = *(uint32_t *)src;


        value =
            (0xff000000 & value) >> 24 |
            (0x00ff0000 & value) >> 8 |
            (0x0000ff00 & value) << 8 |
            (0x000000ff & value) << 24;

        value =
            (0x1f000000 & value) >> 9 |
            (0x001f0000 & value) >> 6 |
            (0x00001f00 & value) >> 3 |
            (0x0000001f & value);

        value =
            (0xff000000 & value) >> 24 |
            (0x00ff0000 & value) >> 8 |
            (0x0000ff00 & value) << 8 |
            (0x000000ff & value) << 24;

        *(uint32_t*)dst |= value;

        dst += 4;
        src ++;
    }

    gvector_swap(data, decoded);
    gvector_free(decoded);
    return DTRUE;
}

static DBOOL codec_encode(void * codec, gvector * data)
{
    BOXING_UNUSED_PARAMETER(codec);

    size_t size = data->size / 5;
    gvector * encoded = gvector_create_char(size * 8, 0);
    unsigned char *src = (unsigned char *)data->buffer;
    uint32_t      *dst = (uint32_t *)encoded->buffer;

    for (size_t i = 0; i < size; i++)
    {
        uint32_t value = *(uint32_t *)src;

        value =
            (0xff000000 & value) >> 24 |
            (0x00ff0000 & value) >> 8 |
            (0x0000ff00 & value) << 8 |
            (0x000000ff & value) << 24;

        value =
            (0x1f000000 & (value >> 3)) |
            (0x001f0000 & (value >> 6)) |
            (0x00001f00 & (value >> 9)) |
            (0x0000001f & (value >> 12));

        value =
            (0xff000000 & value) >> 24 |
            (0x00ff0000 & value) >> 8 |
            (0x0000ff00 & value) << 8 |
            (0x000000ff & value) << 24;


        *dst = value;
        dst++;
        src += 1;

        value = *(uint32_t *)src;

        value =
            (0xff000000 & value) >> 24 |
            (0x00ff0000 & value) >> 8 |
            (0x0000ff00 & value) << 8 |
            (0x000000ff & value) << 24;

        value =
            (0x1f000000 & (value << 9)) |
            (0x001f0000 & (value << 6)) |
            (0x00001f00 & (value << 3)) |
            (0x0000001f & (value));

        value =
            (0xff000000 & value) >> 24 |
            (0x00ff0000 & value) >> 8 |
            (0x0000ff00 & value) << 8 |
            (0x000000ff & value) << 24;

        *dst |= value;
        dst++;
        src += 4;
    }

    gvector_swap(data, encoded);
    gvector_free(encoded);
    return DTRUE;
}
