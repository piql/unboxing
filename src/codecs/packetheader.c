/*****************************************************************************
**
**  Implementation of the packet header interface
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
#include "boxing/codecs/packetheader.h"
#include "boxing/log.h"
#include "boxing/math/bitutils.h"
#include "boxing/utils.h"

//  DEFINES
//

#define CODEC_MEMBER(name) (((boxing_codec_packet_header *)codec)->name)
#define CODEC_BASE_MEMBER(name) (((boxing_codec_packet_header *)codec)->base.name)
#define CRC_SIZE sizeof(unsigned long)

typedef struct packet_header_s
{
    uint32_t header_size;
    uint32_t size;
} packet_header;

//  PRIVATE INTERFACE
//

static DBOOL codec_decode(void * codec, gvector * data, gvector * erasures, boxing_stats_decode * stats, void* user_data);
static DBOOL codec_encode(void * codec, gvector * data);
static DBOOL codec_packet_header_set_property(void * codec, const char * name, const g_variant * value);
static DBOOL codec_init_capacity(struct boxing_codec_s *codec, int size);


/*! 
  * \addtogroup codecs
  * \{
  */


//----------------------------------------------------------------------------
/*!
 *  \struct     boxing_codec_packet_header_s  packetheader.h
 *  \brief      Codec packet header data storage.
 *  
 *  \param base  Base boxing_codec structure instance.
 *
 *  Codec packet header data storage structure description.
 */


// PUBLIC PACKET HEADER FUNCTIONS
//

//----------------------------------------------------------------------------
/*!
 *  \brief This function creates a boxing_codec_packet_header structure instance.
 *
 *  This function creates a boxing_codec_packet_header structure instance according to the input data.
 *
 *  \param[in]  properties  Boxing properties. 
 *  \param[in]  config      Boxing configuration.
 *  \return Pointer to the new instance of boxing_codec_packet_header structure reduced to the boxing_codec structure.
 */

boxing_codec * boxing_codec_packet_header_create(GHashTable * properties, const boxing_config * config)
{
    BOXING_UNUSED_PARAMETER( config ); 
    g_variant * parameter;
    boxing_codec_packet_header * codec = malloc(sizeof(boxing_codec_packet_header));
    boxing_codec_init_base((boxing_codec *)codec);
    codec->base.free = boxing_codec_packet_header_free;
    codec->base.is_error_correcting = DFALSE;
    codec->base.pre_zero_pad_data = DFALSE;
    codec->base.name = codec_packet_header_name;
    codec->base.decode = codec_decode;
    codec->base.encode = codec_encode;
    codec->base.set_property = codec_packet_header_set_property;
    codec->base.init_capacity = codec_init_capacity;

    parameter = g_hash_table_lookup(properties, PARAM_NAME_MESSAGE_SIZE);
    if (parameter)
    {
        codec->base.set_property(codec, PARAM_NAME_MESSAGE_SIZE, parameter);
    }

    (void)codec_init_capacity((boxing_codec *)codec, sizeof(packet_header));
    return (boxing_codec *)codec;
}


//----------------------------------------------------------------------------
/*!
 *  \brief Frees occupied memory of boxing_codec_packet_header structure.
 *
 *  Frees occupied memory of all internal structure pointers and structure pointer.
 *
 *  \param[in]  codec  Pointer to the boxing_codec_packet_header structure that reduced from boxing_codec_packet_header structure.
 */

void boxing_codec_packet_header_free(boxing_codec * codec)
{
    boxing_codec_release_base(codec);
    free(codec);
}


//----------------------------------------------------------------------------
/*!
  * \} end of codecs group
  */


// PRIVATE PACKET HEADER FUNCTIONS
//

static DBOOL codec_decode(void * codec, gvector * data, gvector * erasures, boxing_stats_decode * stats, void* user_data)
{
    BOXING_UNUSED_PARAMETER(erasures);
    BOXING_UNUSED_PARAMETER(codec);
    BOXING_UNUSED_PARAMETER(user_data);

    gvector * packet;
    packet_header * header = (packet_header *)data->buffer;

    if (data->size == 0)
    {
        return DTRUE;
    }

    // conver header from BE byte alignment to host byte alignment
    header->header_size = betohl(header->header_size);
    header->size = betohl(header->size);

    if ((data->size < header->size) || (header->header_size > header->size))
    {
        return DFALSE;
    }

    packet = gvector_create(1, header->size - header->header_size);
    memcpy(packet->buffer, ((char *)data->buffer) + header->header_size, packet->size);

    gvector_swap(data, packet);
    gvector_free(packet);

    stats->fec_accumulated_amount = 0;
    stats->fec_accumulated_weight = 0;
    stats->resolved_errors = 0;
    stats->unresolved_errors = 0;

    return DTRUE;
}

static DBOOL codec_encode(void * codec, gvector * data)
{
    //BOXING_UNUSED_PARAMETER(codec);
    boxing_codec_packet_header * ph_codec = (boxing_codec_packet_header *)codec;

    gvector * packet = gvector_create(data->item_size, ph_codec->base.encoded_data_size);
    if (ph_codec->base.decoded_data_size > data->size)
    {
        memset(packet->buffer, 0, ph_codec->base.encoded_data_size*data->item_size);
    }
    packet_header * header = (packet_header *)packet->buffer;
    header->header_size = sizeof(packet_header);
    header->size = (uint32_t)(data->size*data->item_size + sizeof(packet_header));

    memcpy(((char *)packet->buffer) + header->header_size, data->buffer, data->size*data->item_size);

    // conver header from host byte alignment to BE byte alignment
    header->header_size = htobel(header->header_size);
    header->size = htobel(header->size);

    gvector_swap(data, packet);
    gvector_free(packet);

    return DTRUE;
}

static DBOOL codec_packet_header_set_property(void * codec, const char * name, const g_variant * value)
{
    BOXING_UNUSED_PARAMETER(codec);
    BOXING_UNUSED_PARAMETER(name);
    BOXING_UNUSED_PARAMETER(value);

    return DTRUE;

}

static DBOOL codec_init_capacity(struct boxing_codec_s *codec, int size)
{
    if ((unsigned int)size < sizeof(packet_header))
    {
        return DFALSE;
    }
    codec->encoded_block_size = size;
    codec->encoded_data_size = size;
    codec->decoded_block_size = size - sizeof(packet_header);
    codec->decoded_data_size = size - sizeof(packet_header);
    return DTRUE;
}
