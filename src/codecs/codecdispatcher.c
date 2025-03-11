/*****************************************************************************
**
**  Implementation of the codecdispatcher interface
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
#include "boxing/codecs/codecdispatcher.h"
#include "boxing/globals.h"
#include "boxing/log.h"
#include "boxing/platform/types.h"
#include "boxing/config.h"
#include "boxing/utils.h"

//  DEFINES
//
#define CODEC_MULTI_FRAME_CLASS_NAME "MultiFrameFormat"
#define CODEC_DISPATCHER_CLASS_NAME "CodecDispatcher"
#define CODEC_DISPATCHER_PARAM_VERSION "version"

//  GLOBALS
//
const boxing_codecdispatcher_version boxing_codecdispatcher_version_1_0 = { 1, 0 };
const boxing_codecdispatcher_version boxing_codecdispatcher_version_0_9 = { 0, 9 };

//  PRIVATE INTERFACE
//
static void initialize(boxing_codecdispatcher *dispatcher);
static DBOOL boxing_codecdispatcher_decode_er( boxing_codecdispatcher *dispatcher, gvector * data, gvector * erasures, boxing_stats_decode *stats, void* user_data );


/*! 
  * \addtogroup codecs
  * \{
  */


//----------------------------------------------------------------------------
/*!
 *  \def CODEC_DISPATCHER_DATA_CODING_SCHEME() codecdispatcher.h
 *  \brief Macro name definition for the parameter DATA_CODING_SCHEME.
 *
 *  Macro name definition for the parameter DATA_CODING_SCHEME.
 *  Corresponds to a string "DataCodingScheme".
 */


//----------------------------------------------------------------------------
/*!
 *  \def CODEC_DISPATCHER_METADATA_CODING_SCHEME() codecdispatcher.h
 *  \brief Macro name definition for the parameter METADATA_CODING_SCHEME.
 *
 *  Macro name definition for the parameter METADATA_CODING_SCHEME.
 *  Corresponds to a string "MetedataCodingScheme".
 */


//----------------------------------------------------------------------------
/*!
 *  \def BOXING_CODEC_MODULATION_PAM2() codecdispatcher.h
 *  \brief A macro for value of MODULATION_PAM2.
 *
 *  A macro for value of MODULATION_PAM2.
 *  Corresponds to a value "2" of integer type.
 */


//----------------------------------------------------------------------------
/*!
 *  \def BOXING_CODEC_MODULATION_PAM4() codecdispatcher.h
 *  \brief A macro for value of MODULATION_PAM4.
 *
 *  A macro for value of MODULATION_PAM4.
 *  Corresponds to a value "4" of integer type.
 */


//----------------------------------------------------------------------------
/*!
 *  \def BOXING_CODEC_MODULATION_PAM6() codecdispatcher.h
 *  \brief A macro for value of MODULATION_PAM6.
 *
 *  A macro for value of MODULATION_PAM6.
 *  Corresponds to a value "6" of integer type.
 */


//----------------------------------------------------------------------------
/*!
 *  \def BOXING_CODEC_DISPATCHER_1_0() codecdispatcher.h
 *  \brief A macro for codec dispatcher version 1.0.
 *
 *  A macro for codec dispatcher version 1.0.
 *  Corresponds to constant boxing_codecdispatcher_version_1_0.
 */


//----------------------------------------------------------------------------
/*!
 *  \def BOXING_CODEC_DISPATCHER_PRE_1_0() codecdispatcher.h
 *  \brief A macro for codec dispatcher version 0.9.
 *
 *  A macro for codec dispatcher version 0.9.
 *  Corresponds to constant boxing_codecdispatcher_version_0_9.
 */


//----------------------------------------------------------------------------
/*!
 *  \var enum   BOXING_CODEC_ORDER_ENCODE
 *  \brief      Boxing codec order encode type.
 *  
 *  Boxing codec order encode type.
 */


//----------------------------------------------------------------------------
/*!
 *  \var enum   BOXING_CODEC_ORDER_DECODE
 *  \brief      Boxing codec order decode type.
 *  
 *  Boxing codec order decode type.
 */


//----------------------------------------------------------------------------
/*!
 *  \var enum   BOXING_CODEC_SYMBOL_ALIGNMENT_BIT
 *  \brief      Type for the bit alignment.
 *  
 *  Type for the bit alignment.
 */


//----------------------------------------------------------------------------
/*!
 *  \var enum   BOXING_CODEC_SYMBOL_ALIGNMENT_BYTE
 *  \brief      Type for the byte alignment.
 *  
 *  Type for the byte alignment.
 */


//----------------------------------------------------------------------------
/*!
 *  \var      boxing_codecdispatcher_version_1_0
 *  \brief    Constant for the codec dispatcher version 1.0.
 */


//----------------------------------------------------------------------------
/*!
 *  \var      boxing_codecdispatcher_version_0_9
 *  \brief    Constant for the codec dispatcher version 0.9.
 */


//----------------------------------------------------------------------------
/*!
 *  \struct  boxing_codecdispatcher_version_s codecdispatcher.h
 *  \brief   Structure for storing codec versions.
 *
 *  \param major Major version
 *  \param minor Minor version
 *
 *  Structure for storing codec versions.
 */


//----------------------------------------------------------------------------
/*!
 *  \struct  boxing_codecdispatcher_s codecdispatcher.h
 *  \brief   This struct serves as an interface to encode and decode procedures.
 *
 *  \param decode_codecs    Vector of codec_base
 *  \param encode_codecs    Vector of codec_base
 *  \param order            Codec order
 *  \param symbol_alignment Symbol alignment
 *  \param packet_size      Max size of the encoded data block
 *  \param color_depth      Color depth
 *  \param codeing_scheme   Coding scheme
 *  \param config           Boxing xonfig
 *  \param version          Codec versions
 *  \param multi_frame_size Multi frame size
 *
 *  This struct serves as an interface to encode and decode procedures within 
 *  the box and unbox of archivator. Formally is a dispatcher, similar to a 
 *  web dispatcher. Your responsibility is to call the encode and decode 
 *  methods of correction of errors in the order specified in the 
 *  configuration file. The object for correction are in a internal table 
 *  storage of pointer to abstract object ErrorCorrectionBasePtr.
 */


// PUBLIC CODEC DISPATCHER FUNCTIONS
//

//----------------------------------------------------------------------------
/*!
 *  \brief Get stripe size from the config.
 *
 *  Get stripe size from the config.
 *
 *  \param[in]  config  Pointer to the boxing_config structure.
 *  \return Strip size.
 */

int boxing_codecdispatcher_get_stripe_size(const boxing_config * config)
{
    int stripe_size = 1;
    if (boxing_config_is_set(config, CODEC_MULTI_FRAME_CLASS_NAME, PARAM_NAME_CODEC_MULTI_FRAME_STRIPE_SIZE))
    {
        stripe_size = boxing_config_property_uint(config, CODEC_MULTI_FRAME_CLASS_NAME, PARAM_NAME_CODEC_MULTI_FRAME_STRIPE_SIZE);
    }
    return stripe_size;
}


//----------------------------------------------------------------------------
/*!
 *  \brief The function checks the stripe frames.
 *
 *  The function checks the stripe frames based on the specified configuration..
 *
 *  \param[in] config      Pointer to the boxing_config structure.
 *  \return Strip size.
 */

DBOOL boxing_codecdispatcher_is_data_striped(const boxing_config * config)
{
    return (boxing_codecdispatcher_get_stripe_size(config) > 1 ? DTRUE : DFALSE);
}


//----------------------------------------------------------------------------
/*!
 *  \brief This function creats a codec dispatcher with a given coding scheme.
 *
 *  This function creats a codec dispatcher with a given coding scheme.
 *
 *  \param[in]  packet_size       Max size of the encoded data block. 
 *  \param[in]  modulation_levels Tell the the modulator how many levels to use during modulation.
 *  \param[in]  config            Boxing configuration that include coding schemes.
 *  \param[in]  scheme            Select which coding scheme to use.
 *  \return Pointer to the new instance of codec dispatcher.
 *
 * \todo Remove packet_size and modulation_levels as they are a part of the boxing_config 
 */

boxing_codecdispatcher * boxing_codecdispatcher_create(uint32_t packet_size, uint32_t modulation_levels, const boxing_config * config, const char * scheme/* = FORWARD_ERROR_CORRECTION_PROCESS*/)
{
    boxing_codecdispatcher *dispatcher = malloc(sizeof(boxing_codecdispatcher));
    dispatcher->codeing_scheme = scheme;
    if (dispatcher->codeing_scheme == NULL)
    {
        dispatcher->codeing_scheme = FORWARD_ERROR_CORRECTION_PROCESS;
    }
    dispatcher->packet_size = packet_size;

    dispatcher->color_depth = 0;
    // determine required color depth with the given modulation levels
    uint32_t levels = modulation_levels - 1;
    while (levels)
    {
        dispatcher->color_depth++;
        levels >>= 1;
    }

    dispatcher->config = config;

    // required backward compatibility
    if (boxing_string_equal(dispatcher->codeing_scheme, "DataCodingScheme") &&
        !boxing_config_is_set(dispatcher->config, CODEC_DISPATCHER_CLASS_NAME, dispatcher->codeing_scheme))
    {
        dispatcher->codeing_scheme = FORWARD_ERROR_CORRECTION_PROCESS;
    } 
    else if(boxing_string_equal(dispatcher->codeing_scheme, "MetadataCodingScheme") &&
        !boxing_config_is_set(dispatcher->config, CODEC_DISPATCHER_CLASS_NAME, dispatcher->codeing_scheme))
    {
        dispatcher->codeing_scheme = CODEC_DISPATCHER_METADATA_CODING_SCHEME;
    }
    dispatcher->order = BOXING_CODEC_ORDER_ENCODE;
    dispatcher->symbol_alignment = BOXING_CODEC_SYMBOL_ALIGNMENT_BIT;

    gvector_create_inplace(&dispatcher->encode_codecs, sizeof(boxing_codec *), 0);
    gvector_create_inplace(&dispatcher->decode_codecs, sizeof(boxing_codec *), 0);

    initialize(dispatcher);
    return dispatcher;
}


//----------------------------------------------------------------------------
/*!
 *  \brief This function setup the codec callback function.
 *
 *  This function setup the codec callback function in the codec dispatcher.
 *
 *  \param[in]  codec_dispatcher  Max size of the encoded data block. 
 *  \param[in]  callbacks         Tell the the modulator how many levels to use during modulation.
 */

void boxing_codecdispatcher_callback_setup(boxing_codecdispatcher * codec_dispatcher, const boxing_codec_cb * callbacks)
{
    if (callbacks)
    {
        for (int step = 0; step < (int)codec_dispatcher->encode_codecs.size; step++)
        {
            boxing_codec * codec = boxing_codecdispatcher_get_decode_codec(codec_dispatcher, step);
            for (unsigned int j = 0; callbacks[j].name != NULL; ++j)
            {
                if (boxing_string_equal(callbacks[j].name, codec->name))
                {
                    codec->encode_cb = callbacks[j].encode;
                    codec->decode_cb = callbacks[j].decode;
                }
            }
        }
    }
}


//----------------------------------------------------------------------------
/*!
 *  \brief Frees occupied memory of boxing_codecdispatcher structure.
 *
 *  Frees occupied memory of all internal structure pointers and structure pointer.
 *
 *  \param[in]  dispatcher  Pointer to the boxing_codecdispatcher structure.
 */

void boxing_codecdispatcher_free(boxing_codecdispatcher * dispatcher)
{
    for (size_t i = 0; i < dispatcher->encode_codecs.size; ++i)
    {
        boxing_codec_release(GVECTORN(&dispatcher->encode_codecs, boxing_codec *, i));
    }
    free(dispatcher->encode_codecs.buffer);
    free(dispatcher->decode_codecs.buffer);
    free(dispatcher);
}


//----------------------------------------------------------------------------
/*!
 *  \brief Interface to launch encode procedures.
 *
 *  Interface to launch encode procedures.
 *
 *  \param[in]  dispatcher  Pointer to the boxing_codecdispatcher structure.
 *  \param[in]  data        Array of bytes to encode.
 *  \return DTRUE if success.
 */

DBOOL boxing_codecdispatcher_encode(boxing_codecdispatcher *dispatcher, gvector * data)
{
    DBOOL retval = DTRUE;
    for (unsigned int step = 0; step < dispatcher->encode_codecs.size; step++) 
    {
        DLOG_WARNING1( "Encoding step %i", step);
        retval &= boxing_codecdispatcher_encode_step(dispatcher, data, step);
    }
    DLOG_WARNING("Encoding finished");
    return retval;
}


//----------------------------------------------------------------------------
/*!
 *  \brief Interface to launch current step of encode procedure.
 *
 *  Interface to launch current step of encode procedure.
 *
 *  \param[in]  dispatcher  Pointer to the boxing_codecdispatcher structure.
 *  \param[in]  data        Array of bytes to encode.
 *  \param[in]  step        Current encode step.
 *  \return DTRUE if success.
 */

DBOOL boxing_codecdispatcher_encode_step(boxing_codecdispatcher *dispatcher, gvector * data, int step)
{
    if ((step > (int)dispatcher->encode_codecs.size) || (step < -1))
    {
        return DFALSE;
    }

    boxing_codec * codec = GVECTORN(&dispatcher->encode_codecs, boxing_codec *, step);

    return boxing_codecdispatcher_encode_step_codec(codec, data);
}


//----------------------------------------------------------------------------
/*!
 *  \brief Interface to launch codec encode procedure.
 *
 *  Interface to launch codec encode procedure.
 *
 *  \param[in]  codec  Pointer to the boxing_codec structure.
 *  \param[in]  data   Array of bytes to encode.
 *  \return DTRUE if success.
 */

DBOOL boxing_codecdispatcher_encode_step_codec(boxing_codec * codec, gvector * data)
{
    unsigned int symbol_byte_size = (codec->decoded_symbol_size + 7) / 8;
    if (data->item_size != symbol_byte_size)
    {
        return DFALSE;
    }

    if (data->size > (size_t)codec->decoded_data_size)
    {
        return DFALSE;
    }

    if (data->size < (size_t)codec->decoded_data_size)
    {
        if (codec->pre_zero_pad_data)
        {
            int i = (int)data->size;
            gvector_resize(data, codec->decoded_data_size);
            memset((char *)data->buffer + i*symbol_byte_size, 0, (data->size - i)*symbol_byte_size);
        }
    }

    
    return codec->encode(codec, data);
}


//----------------------------------------------------------------------------
/*!
 *  \brief Interface to launch decode procedures.
 *
 *  Interface to launch decode procedures.
 *
 *  \param[in]  dispatcher  Pointer to the boxing_codecdispatcher structure.
 *  \param[in]  data        Array of bytes to encode.
 *  \param[in]  stats       Pointer to the boxing_stats_decode structure.
 *  \param[in]  user_data   Array of user data bytes.
 *  \return DTRUE if success.
 */

DBOOL boxing_codecdispatcher_decode(boxing_codecdispatcher *dispatcher, gvector * data, boxing_stats_decode *stats, void* user_data)
{
    /// \todo Currently erasures are not used.
    gvector * erasures = gvector_create_char(data->size, 0);
    DBOOL return_value = boxing_codecdispatcher_decode_er(dispatcher, data, erasures, stats, user_data);
    gvector_free(erasures);
    return return_value;
}


//----------------------------------------------------------------------------
/*!
 *  \brief Interface to launch current step of decode procedure.
 *
 *  Interface to launch current step of decode procedure.
 *
 *  \param[in]  dispatcher  Pointer to the boxing_codecdispatcher structure.
 *  \param[in]  data        Array of bytes to encode.
 *  \param[in]  erasures    Array of erasures data bytes.
 *  \param[in]  step        Current encode step.
 *  \param[in]  stats       Pointer to the boxing_stats_decode structure.
 *  \param[in]  user_data   Array of user data bytes.
 *  \return DTRUE if success.
 */

DBOOL boxing_codecdispatcher_decode_step(boxing_codecdispatcher *dispatcher, gvector * data, gvector * erasures, int step, boxing_stats_decode *stats, void* user_data)
{
    if ((step > (int)dispatcher->decode_codecs.size) || (step < -1))
    {
        return DFALSE;
    }

    boxing_codec * codec = GVECTORN(&dispatcher->decode_codecs, boxing_codec *, step);

    return boxing_codecdispatcher_decode_step_codec(codec, data, erasures, stats, user_data);
}


//----------------------------------------------------------------------------
/*!
 *  \brief Interface to launch codec decode procedure.
 *
 *  Interface to launch codec decode procedure.
 *
 *  \param[in]  codec      Pointer to the boxing_codec structure.
 *  \param[in]  data       Array of bytes to encode.
 *  \param[in]  erasures   Array of erasures data bytes.
 *  \param[in]  stats      Pointer to the boxing_stats_decode structure.
 *  \param[in]  user_data  Array of user data bytes.
 *  \return DTRUE if success.
 */

DBOOL boxing_codecdispatcher_decode_step_codec(boxing_codec * codec, gvector * data, gvector * erasures, boxing_stats_decode *stats, void* user_data)
{
    DBOOL retval = DFALSE;

    /* The input data length of this stage may be smaller
    * that the output lenth of the previous step,
    * so any padding bytes have to be removed */

    unsigned int symbol_byte_size = (codec->encoded_symbol_size + 7) / 8;
    if (data->item_size != symbol_byte_size)
    {
        DLOG_WARNING2( "(codecdispatcher.c:boxing_codecdispatcher_decode_step_codec) Data type check failed: data->item_size(%d) != symbol_byte_size(%d)", data->item_size, symbol_byte_size );
        return DFALSE;
    }

    if (codec->encoded_data_size < data->size)
    {
        gvector_resize(data, codec->encoded_data_size);
    }

    boxing_stats_decode decode_stats;
    decode_stats.fec_accumulated_amount = 0;
    decode_stats.fec_accumulated_weight = 0;
    decode_stats.resolved_errors = 0;
    decode_stats.unresolved_errors = 0;
    retval = codec->decode(codec, data, erasures, &decode_stats, user_data);
    if (codec->is_error_correcting)
    {
        stats->unresolved_errors = 0;
    }
    stats->fec_accumulated_amount += decode_stats.fec_accumulated_amount;
    stats->fec_accumulated_weight += decode_stats.fec_accumulated_weight;
    stats->resolved_errors += decode_stats.resolved_errors;
    stats->unresolved_errors += decode_stats.unresolved_errors;

    return retval;
}


//----------------------------------------------------------------------------
/*!
 *  \brief Interface to launch codec dispatcher reset procedure.
 *
 *  Interface to launch codec dispatcher reset procedure.
 *
 *  \param[in]  dispatcher  Pointer to the boxing_codecdispatcher structure.
 */

void boxing_codecdispatcher_reset(boxing_codecdispatcher *dispatcher)
{
    for (int i = 0; i < (int)dispatcher->decode_codecs.size; i++)
    {
        boxing_codec * codec = GVECTORN(&dispatcher->decode_codecs, boxing_codec *, i);
        if (codec->reset)
        {
            codec->reset(codec);
        }
    }
}


//----------------------------------------------------------------------------
/*!
 *  \brief Get coding steps value from the given codec dispatcher instance.
 *
 *  Get coding steps value from the given codec dispatcher instance.
 *
 *  \param[in]  dispatcher  Pointer to the boxing_codecdispatcher structure.
 *  \return coding steps value.
 */

unsigned int boxing_codecdispatcher_get_coding_steps(boxing_codecdispatcher * dispatcher)
{
    return (unsigned int)dispatcher->encode_codecs.size;
}


//----------------------------------------------------------------------------
/*!
 *  \brief Get the encode codec for the given encode step.
 *
 *  Get the encode codec for the given encode step.
 *
 *  \param[in]  dispatcher  Pointer to the boxing_codecdispatcher structure.
 *  \param[in]  step        Current encode step.
 *  \return encode codec for the given encode step.
 */

boxing_codec * boxing_codecdispatcher_get_encode_codec(boxing_codecdispatcher * dispatcher, int step)
{
    if((step >= (int)dispatcher->encode_codecs.size) || (step < 0))
        return NULL;
    return GVECTORN(&dispatcher->encode_codecs, boxing_codec *, step);
}


//----------------------------------------------------------------------------
/*!
 *  \brief Get the decode codec for the given decode step.
 *
 *  Get the encode codec for the given decode step.
 *
 *  \param[in]  dispatcher  Pointer to the boxing_codecdispatcher structure.
 *  \param[in]  step        Current encode step.
 *  \return decode codec for the given decode step.
 */

boxing_codec * boxing_codecdispatcher_get_decode_codec(boxing_codecdispatcher * dispatcher, int step)
{
    if((step >= (int)dispatcher->decode_codecs.size) || (step < 0))
        return NULL;
    return GVECTORN(&dispatcher->decode_codecs, boxing_codec *, step);
}


//----------------------------------------------------------------------------
/*!
 *  \brief Get the max size of the encoded data block.
 *
 *  Get the max size of the encoded data block.
 *
 *  \param[in]  dispatcher  Pointer to the boxing_codecdispatcher structure.
 *  \return Max size of the encoded data block.
 */

uint32_t boxing_codecdispatcher_get_encoded_packet_size(boxing_codecdispatcher *dispatcher)
{
    return boxing_codecdispatcher_get_encoded_packet_size_step(dispatcher, (int)dispatcher->encode_codecs.size - 1);
}


//----------------------------------------------------------------------------
/*!
 *  \brief Get the max size of the encoded data block for a given step.
 *
 *  Get the max size of the encoded data block for a given step.
 *
 *  \param[in]  dispatcher  Pointer to the boxing_codecdispatcher structure.
 *  \param[in]  step        Current encode step.
 *  \return Max size of the encoded data block for a given step.
 */

uint32_t boxing_codecdispatcher_get_encoded_packet_size_step(boxing_codecdispatcher *dispatcher, int step)
{
    if(step >= (int)dispatcher->encode_codecs.size)
        step = (int)dispatcher->encode_codecs.size - 1;
    if(step < 0)
        step = -1;
    return GVECTORN(&dispatcher->encode_codecs, boxing_codec *, step)->encoded_data_size;
}


//----------------------------------------------------------------------------
/*!
 *  \brief Get the max size of the decoded data block.
 *
 *  Get the max size of the decoded data block.
 *
 *  \param[in]  dispatcher  Pointer to the boxing_codecdispatcher structure.
 *  \return Max size of the decoded data block.
 */

uint32_t boxing_codecdispatcher_get_decoded_packet_size(boxing_codecdispatcher *dispatcher)
{
    return boxing_codecdispatcher_get_decoded_packet_size_step(dispatcher, (int)dispatcher->decode_codecs.size - 1);
}


//----------------------------------------------------------------------------
/*!
 *  \brief Get the max size of the decoded data block for a given step.
 *
 *  Get the max size of the decoded data block for a given step.
 *
 *  \param[in]  dispatcher  Pointer to the boxing_codecdispatcher structure.
 *  \param[in]  step        Current decode step.
 *  \return Max size of the decoded data block for a given step.
 */

uint32_t boxing_codecdispatcher_get_decoded_packet_size_step(boxing_codecdispatcher *dispatcher, int step)
{
    if(step >= (int)dispatcher->decode_codecs.size)
        step = (int)dispatcher->decode_codecs.size - 1;
    if(step < 0)
        step = -1;
    return GVECTORN(&dispatcher->decode_codecs, boxing_codec *, step)->decoded_data_size;
}


//----------------------------------------------------------------------------
/*!
 *  \brief Get bytes per frame
 *
 *  Function retrieves number of bytes per frame from given codec dispatcher and returns it.
 *  If boxing_codecdispatcher is NULL, 0 is returned.
 *
 *  \param[in]  dispatcher   Pointer to the boxing_codecdispatcher structure.
 *  \return the current value of bytes per frame.
 */

uint32_t boxing_codecdispatcher_get_bytes_per_frame(boxing_codecdispatcher *dispatcher)
{
    if (dispatcher == NULL)
    {
        return 0;
    }

    return dispatcher->packet_size * dispatcher->color_depth / BITS_PER_CHAR;
}


//----------------------------------------------------------------------------
/*!
 *  \brief Compare codec dispatcher versions
 *
 *  The function compare versions of two instances boxing_codecdispatcher_version structure.
 *
 *  \param[in]  a   Pointer to the first boxing_codecdispatcher_version structure.
 *  \param[in]  b   Pointer to the second boxing_codecdispatcher_version structure.
 *  \return 1 if a > b, -1 if a < b, 0 if equal.
 */

int boxing_codecdispatcher_version_cmp(const boxing_codecdispatcher_version * a, const boxing_codecdispatcher_version *b)
{
    if (a->major > b->major)
    {
        return 1;
    }
    
    if (a->major < b->major)
    {
        return -1;
    }

    if (a->minor > b->minor)
    {
        return 1;
    }
    
    if (a->minor < b->minor)
    {
        return -1;
    }

    return 0;
}


//----------------------------------------------------------------------------
/*!
 *  \brief Calculate number of data frames needed for given file_size.
 *
 *  Return file_size divided by dispatcher packet size.
 *
 *  \param[in]  dispatcher   Pointer to the boxing_codecdispatcher structure.
 *  \param[in]  file_size    The file size in bytes.
 *  \return the number of data frames or 0 on error.
 */

#if defined ( D_OS_WIN32 )
unsigned int       boxing_codecdispatcher_get_data_frames(boxing_codecdispatcher *dispatcher, __int64 file_size)
#else
unsigned int       boxing_codecdispatcher_get_data_frames(boxing_codecdispatcher *dispatcher, off_t file_size)
#endif
{
    if (dispatcher == NULL)
    {
        return 0;
    }

    unsigned int bytes_per_frame = boxing_codecdispatcher_get_decoded_packet_size(dispatcher);

    if (bytes_per_frame == 0)
    {
        return 0;
    }

#if defined ( D_OS_WIN32 )
    __int64 result = file_size / bytes_per_frame;
#else
    off_t result = file_size / bytes_per_frame;
#endif
    if (file_size % bytes_per_frame)
    {
        result++;
    }

    return (unsigned int)result;
}


//----------------------------------------------------------------------------
/*!
  * \} end of codecs group
  */


// PRIVATE CODEC DISPATCHER FUNCTIONS
//

static DBOOL boxing_codecdispatcher_decode_er(boxing_codecdispatcher *dispatcher, gvector * data, gvector * erasures, boxing_stats_decode *stats, void* user_data)
{
    DBOOL retval = DTRUE;
    for (unsigned int step = 0; step < dispatcher->decode_codecs.size; step++)
    {
        retval &= boxing_codecdispatcher_decode_step(dispatcher, data, erasures, step, stats, user_data);
    }
    return retval;
}


static void calculate_packet_sizes(
    gvector * decoder_stack,
    uint32_t encoder_buffer_capacity,
    uint8_t symbol_size,
    int symbol_alignment)
{
    if (symbol_alignment == BOXING_CODEC_SYMBOL_ALIGNMENT_BIT)
    {
        symbol_size = 8;
        encoder_buffer_capacity /= 8;
    }
    if(decoder_stack->size == 0)
    {
        return;
    }

    boxing_codec ** decoder = (boxing_codec **)decoder_stack->buffer;
    do
    {
        // buffer matching
        (*decoder)->init_capacity(*decoder, encoder_buffer_capacity);
        encoder_buffer_capacity = (*decoder)->decoded_data_size;

        // symbol size matching
        if (((*decoder)->encoded_symbol_size+7)/8 != (unsigned int)(symbol_size+7)/8)
        {
            DLOG_ERROR("(calculate_packet_sizes) Encoded symbol size mismatch");
            boxing_throw("ERROR_UNDEFINED_PROPERTY");
            return;
        }
        symbol_size = (*decoder)->decoded_symbol_size;
        decoder++;
    } while (decoder != (boxing_codec **)decoder_stack->buffer + decoder_stack->size);

    if (symbol_size != 8)
    {
        boxing_throw("ERROR_ILLEGAL_SYMBOL_SIZE");
        return;
    }
}

static DBOOL get_version(const char* version, int* major, int* minor)
{
    gvector * items = boxing_string_split(version, ".");
    if (items->size < 2)
    {
        gvector_free(items);
        return DFALSE;
    }

    DBOOL ok =
        boxing_string_to_integer(major, GVECTORN(items, char*, 0)) == DTRUE &&
        boxing_string_to_integer(minor, GVECTORN(items, char*, 1)) == DTRUE;

    gvector_free(items);
    return ok;
}

static void initialize(boxing_codecdispatcher *dispatcher)
{

    dispatcher->multi_frame_size = boxing_codecdispatcher_get_stripe_size(dispatcher->config);

    if (boxing_config_is_set(dispatcher->config, CODEC_DISPATCHER_CLASS_NAME, CODEC_DISPATCHER_PARAM_VERSION))
    {
        const char * str_version = boxing_config_property(dispatcher->config, CODEC_DISPATCHER_CLASS_NAME, CODEC_DISPATCHER_PARAM_VERSION);

        DBOOL ok = get_version(str_version, &dispatcher->version.major, &dispatcher->version.minor);
        if (!ok)
        {
            DLOG_ERROR2("(codecdispatcher.c:initialize) Property %s has unknown value '%s'", CODEC_DISPATCHER_PARAM_VERSION, str_version);
            boxing_throw("ERROR_UNDEFINED_PROPERTY"); /// \todo replace with DFATAL()
        }
    }
    else
    {
        dispatcher->version = BOXING_CODEC_DISPATCHER_PRE_1_0;
    }

    if (boxing_config_is_set(dispatcher->config, CODEC_DISPATCHER_CLASS_NAME, "order"))
    {
        const char * str_order  = boxing_config_property(dispatcher->config, CODEC_DISPATCHER_CLASS_NAME, "order");
        if (boxing_string_equal(str_order, "decode"))
        {
            dispatcher->order = BOXING_CODEC_ORDER_DECODE;
        }
        else if (boxing_string_equal(str_order, "encode"))
        {
            dispatcher->order = BOXING_CODEC_ORDER_ENCODE;
        }
        else
        {
            DLOG_ERROR1("(codecdispatcher.c:initialize) Property order has unknown value '%s'", str_order);
            boxing_throw("ERROR_UNDEFINED_PROPERTY"); /// \todo replace with DFATAL()
        }
    }

    if (boxing_config_is_set(dispatcher->config, CODEC_DISPATCHER_CLASS_NAME, "symbolAlignment"))
    {
        const char * symbol_alignement_str = boxing_config_property(dispatcher->config, CODEC_DISPATCHER_CLASS_NAME, "symbolAlignment");
        if (boxing_string_equal(symbol_alignement_str, "byte"))
        {
            dispatcher->symbol_alignment = BOXING_CODEC_SYMBOL_ALIGNMENT_BYTE;
        }
        else if (boxing_string_equal(symbol_alignement_str, "bit"))
        {
            dispatcher->symbol_alignment = BOXING_CODEC_SYMBOL_ALIGNMENT_BIT;
        }
        else
        {
            DLOG_ERROR1("(codecdispatcher.c:initialize) Property symbolAlignment has unknown value '%s'", symbol_alignement_str);
            boxing_throw("ERROR_UNDEFINED_PROPERTY");
        }
    }

    if (boxing_config_is_set(dispatcher->config, CODEC_DISPATCHER_CLASS_NAME, dispatcher->codeing_scheme))
    {
        gvector * list_process = boxing_config_parse_list_properties((boxing_config *)dispatcher->config, CODEC_DISPATCHER_CLASS_NAME, dispatcher->codeing_scheme);

        for (unsigned int i = 0; i < list_process->size; i++)
        {
            const char * class_name_str = GVECTORN(list_process, char *, i);
            if (boxing_config_is_set(dispatcher->config, class_name_str, "codec"))
            {
                gvector * codec_names = boxing_config_parse_list_properties((boxing_config *)dispatcher->config, class_name_str, "codec");
                const char * codec_name = GVECTORN(codec_names, char *, 0);
                const GHashTable * properties_c; // Hash of <string, variant>
                GHashTable * properties = g_hash_table_new_full(g_str_hash, g_str_equal, boxing_utils_g_hash_table_destroy_item_string, boxing_utils_g_hash_table_destroy_item_g_variant);

                boxing_config_properties((boxing_config *)dispatcher->config, class_name_str, &properties_c);
                for (int j = 0; j < properties_c->size; j++)
                {
                    guint node_hash = properties_c->hashes[j];
                    gpointer node_key = properties_c->keys[j];
                    gpointer node_value = properties_c->values[j];

                    if (HASH_IS_REAL(node_hash))
                    {
                        g_hash_table_replace(properties, boxing_string_clone(node_key), g_variant_clone(node_value));
                    }
                }
                {
                    g_variant * it = g_hash_table_lookup(properties, "NumBitsPerPixel");
                    if (it != NULL)
                    {
                        const char * it_string = g_variant_if_string(it);
                        if (it_string != NULL && boxing_string_equal(it_string, "auto"))
                        {
                            g_variant_set_uint(it, dispatcher->color_depth);
                        }
                    }
                    it = g_hash_table_lookup(properties, PARAM_NAME_CODEC_MULTI_FRAME_STRIPE_SIZE);
                    if (it != NULL)
                    {
                        const char * it_string = g_variant_if_string(it);
                        if (it_string != NULL && boxing_string_equal(it_string, "auto"))
                        {
                            g_variant_set_uint(it, dispatcher->multi_frame_size);
                        }
                    }
                }
                boxing_codec * codec = boxing_codec_create(codec_name, properties, dispatcher->config);
                gvector_free(codec_names);
                if (!codec)
                {
                    DLOG_ERROR1("(codecdispatcher.c:initialize) Unknown codec %s", class_name_str);
                    boxing_throw("ERROR_CORRECTION_MODE_UNKNOW");
                }
                if (dispatcher->order == BOXING_CODEC_ORDER_ENCODE)
                {
                    gvector_append_data(&dispatcher->encode_codecs, 1, &codec);
                    gvector_append_data_at(&dispatcher->decode_codecs, 1, &codec, 0);
                }
                else
                {
                    gvector_append_data(&dispatcher->decode_codecs, 1, &codec);
                    gvector_append_data_at(&dispatcher->encode_codecs, 1, &codec, 0);
                }

                g_hash_table_destroy(properties);
            }
        }
        gvector_free(list_process);
        calculate_packet_sizes( &dispatcher->decode_codecs, dispatcher->packet_size, dispatcher->color_depth, dispatcher->symbol_alignment);
    }

}
