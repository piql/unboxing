/*****************************************************************************
**
**  Implementation of the ftf interleaving interface
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
#include "boxing/codecs/ftfinterleaving.h"
#include "boxing/log.h"
#include "boxing/utils.h"

//  DEFINES
//

#define CODEC_MEMBER(name) (((boxing_codec_ftf_interleaving *)codec)->name)
#define CODEC_BASE_MEMBER(name) (((boxing_codec_ftf_interleaving *)codec)->base.name)

//  PRIVATE INTERFACE
//

static GList * malloc_ftf_buffer(int blocks, size_t block_size);
static void    free_ftf_buffer(GList *buf);
static void    ftf_buffer_destroy_notify(gpointer data);
static void    ftf_interleave(boxing_codec_ftf_interleaving * codec, gvector * data);
static void    ftf_deinterleave(boxing_codec_ftf_interleaving * codec, gvector * data);
static DBOOL   ftf_encode(void * codec, gvector * data);
static DBOOL   ftf_decode(void * codec, gvector * data, gvector * erasures, boxing_stats_decode * stats, void* user_data);
static DBOOL   ftf_set_property(void * codec, const char * name, const g_variant * value);
static void    ftf_reset(void * codec);
static DBOOL   init_capacity(struct boxing_codec_s *codec, int size);


/*! 
  * \addtogroup codecs
  * \{
  */


//----------------------------------------------------------------------------
/*!
 *  \struct     boxing_codec_ftf_interleaving_s  ftfinterleaving.h
 *  \brief      Codec ftf interleaving data storage structure.
 *  
 *  \param base            Base boxing_codec structure instance.
 *  \param distance        Distance.
 *  \param preload_frames  Preload frames.
 *  \param encode_buffer   Encode buffer (GList).
 *  \param decode_buffer   Decode buffer (GList).
 *
 *  Codec ftf interleaving data storage structure description.
 */


// PUBLIC FTF INTERLEAVING FUNCTIONS
//

//----------------------------------------------------------------------------
/*!
 *  \brief Create an boxing_codec_ftf_interleaving structure instance.
 *
 *  Allocate memory for the boxing_codec_ftf_interleaving type
 *  and initializes internal values according to input data.
 *  Return instance of allocated structure.
 *
 *  \param[in] properties  Properties.
 *  \param[in] config      Configuration data.
 *  \return instance of allocated boxing_codec_ftf_interleaving structure reduced to boxing_codec structure.
 */

boxing_codec * boxing_ftf_interleaving_create(GHashTable * properties, const boxing_config * config)
{
    BOXING_UNUSED_PARAMETER( config ); 
    boxing_codec_ftf_interleaving * codec = malloc(sizeof(boxing_codec_ftf_interleaving));
    boxing_codec_init_base((boxing_codec *)codec);
    codec->base.free = boxing_ftf_interleaving_free;
    codec->base.is_error_correcting = DFALSE;
    codec->base.name = codec_ftf_interleaving_name;
    codec->base.decode = ftf_decode;
    codec->base.encode = ftf_encode;
    codec->base.set_property = ftf_set_property;
    codec->base.init_capacity = init_capacity;
    codec->base.reset = ftf_reset;
    codec->base.reentrant = 0;
    codec->encode_buffer = NULL;
    codec->decode_buffer = NULL;

    // interleaving distance
    g_variant * distance = g_hash_table_lookup(properties, PARAM_NAME_CODEC_MULTI_FRAME_STRIPE_SIZE);
    if (distance == NULL)
    {
        DLOG_ERROR1("Requires property '%s' to be set", PARAM_NAME_CODEC_MULTI_FRAME_STRIPE_SIZE);
        boxing_ftf_interleaving_free((boxing_codec *)codec);
        return NULL;
    }
    CODEC_MEMBER(distance) = g_variant_to_uint(distance);

    g_variant * message_size = g_hash_table_lookup(properties, PARAM_NAME_MESSAGE_SIZE);
    if (message_size)
    {
        codec->base.set_property(codec, PARAM_NAME_MESSAGE_SIZE, message_size);
    }

    return (boxing_codec *)codec;
}


//----------------------------------------------------------------------------
/*!
 *  \brief Frees occupied memory of boxing_codec_ftf_interleaving structure.
 *
 *  Frees occupied memory of all internal structure pointers and structure pointer.
 *
 *  \param[in]  codec  Pointer to the boxing_codec structure.
 */

void boxing_ftf_interleaving_free(boxing_codec * codec)
{
    boxing_codec_ftf_interleaving * ftf_codec = (boxing_codec_ftf_interleaving *)codec;
    boxing_codec_release_base(codec);
    free_ftf_buffer(ftf_codec->encode_buffer);
    free_ftf_buffer(ftf_codec->decode_buffer);
    free(codec);
}


//----------------------------------------------------------------------------
/*!
  * \} end of codecs group
  */


// PRIVATE FTF INTERLEAVING FUNCTIONS
//

static void ftf_reset(void * codec)
{
    boxing_codec_ftf_interleaving *ftf_codec = (boxing_codec_ftf_interleaving *)codec;
    ftf_codec->preload_frames = ftf_codec->distance - 1;
}


static void ftf_deinterleave(boxing_codec_ftf_interleaving * codec, gvector * data)
{
    GList *element;

    char *ptr;
    size_t size = data->item_size*data->size;
    size_t n;

    // advance to the next start position
    codec->decode_buffer = codec->decode_buffer->prev;

    // swap buffers
    gvector_swap((gvector *)codec->decode_buffer->data, data);

    if (codec->preload_frames)
    {
        codec->preload_frames--;
        gvector_resize(data, 0);
        return;
    }

    // interleave data
    element = codec->decode_buffer;
    ptr = (char *)data->buffer;
    for (n = 0; n < size; n++, ptr++, element = element->next)
    {
        *ptr = GVECTORN8((gvector *)element->data, n);
    }

}

static DBOOL ftf_decode(void * codec, gvector * data, gvector * erasures, boxing_stats_decode * stats, void* user_data)
{
    BOXING_UNUSED_PARAMETER(erasures);
    BOXING_UNUSED_PARAMETER(user_data);

    boxing_codec_ftf_interleaving *ftf_codec = (boxing_codec_ftf_interleaving *)codec;

    if (!ftf_codec->decode_buffer)
        ftf_codec->decode_buffer = malloc_ftf_buffer(ftf_codec->distance, data->size * data->item_size);

    ftf_deinterleave(ftf_codec, data);

    stats->fec_accumulated_amount = 0;
    stats->fec_accumulated_weight = 0;
    stats->resolved_errors = 0;
    stats->unresolved_errors = 0;
    return DTRUE;
}

static void ftf_interleave(boxing_codec_ftf_interleaving * codec, gvector * data)
{
    GList *element;

    char *ptr;
    size_t size = data->item_size * data->size;
    size_t n;

    // interleave data
    element = codec->encode_buffer;
    ptr = (char *)data->buffer;
    for (n = 0; n < size; n++, ptr++, element = element->next)
    {
        GVECTORN8((gvector *)element->data, n) = *ptr;
    }

    // advance to the next start position
    codec->encode_buffer = codec->encode_buffer->prev;

    // swap buffers
    gvector_swap((gvector *)codec->encode_buffer->data, data);


}

static GList * malloc_ftf_buffer(int blocks, size_t block_size)
{
    int n;
    GList *list = NULL;

    for (n = 0; n < blocks; n++)
    {
        gvector * data = gvector_create_char(block_size, 0);
        list = g_list_prepend(list, (gpointer *)data);
    }

    // close the buffer loop
    GList * last = g_list_last(list);
    list->prev = last;
    last->next = list;

    return list;
}

static void ftf_buffer_destroy_notify(gpointer data)
{
    gvector_free((gvector*)data);
}

static void free_ftf_buffer(GList *buf)
{
    if (!buf)
        return;

    // break buffer loop
    buf->prev->next = NULL;
    buf->prev = NULL;

    g_list_free_full(buf, ftf_buffer_destroy_notify);
}

static DBOOL ftf_encode(void * codec, gvector * data)
{
    boxing_codec_ftf_interleaving *ftf_codec = (boxing_codec_ftf_interleaving *)codec;

    if (!ftf_codec->encode_buffer)
        ftf_codec->encode_buffer = malloc_ftf_buffer(ftf_codec->distance, data->size * data->item_size);

    ftf_interleave(ftf_codec, data);
    return DTRUE;
}

static DBOOL ftf_set_property(void * codec, const char * name, const g_variant * value)
{
    BOXING_UNUSED_PARAMETER(codec);
    BOXING_UNUSED_PARAMETER(name);
    BOXING_UNUSED_PARAMETER(value);

    return DTRUE;
}

static DBOOL init_capacity(struct boxing_codec_s *codec, int size)
{

    codec->encoded_block_size = size;
    codec->encoded_data_size = size;
    codec->decoded_block_size = size;
    codec->decoded_data_size = size;
    return DTRUE;
}
