/*****************************************************************************
**
**  Implementation of the codecbase interface
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
#include "boxing/codecs/codecbase.h"
#include "boxing/codecs/reedsolomon.h"
#include "boxing/codecs/interleaving.h"
#include "boxing/codecs/cipher.h"
#include "boxing/codecs/modulator.h"
#include "boxing/codecs/syncpointinserter.h"
#include "boxing/codecs/crc32.h"
#include "boxing/codecs/crc64.h"
#include "boxing/codecs/packetheader.h"
#include "boxing/codecs/ftfinterleaving.h"
#include "boxing/codecs/2dpam.h"
#include "boxing/codecs/bchcodec.h"
#include "boxing/utils.h"
#include <stdio.h>
#include <string.h>
#include "mod2sparse.h"
#include "mod2dense.h"
#include "rcode.h"
#include "boxing/codecs/ldpccodec.h"
#include "boxing/codecs/symbolconverter.h"


/*! 
  * \addtogroup codecs
  * \{
  */


//----------------------------------------------------------------------------
/*!
 *  \def PARAM_NAME_CODEC_MULTI_FRAME_STRIPE_SIZE() codecbase.h
 *  \brief Macro name definition for the parameter CODEC_MULTI_FRAME_STRIPE_SIZE.
 *
 *  Macro name definition for the parameter CODEC_MULTI_FRAME_STRIPE_SIZE.
 *  Corresponds to a string "DataStripeSize".
 */


//----------------------------------------------------------------------------
/*!
 *  \def PARAM_NAME_DISTANCE() codecbase.h
 *  \brief Macro name definition for the parameter DISTANCE.
 *
 *  Macro name definition for the parameter DISTANCE.
 *  Corresponds to a string "distance".
 */


//----------------------------------------------------------------------------
/*!
 *  \def PARAM_NAME_MESSAGE_SIZE() codecbase.h
 *  \brief Macro name definition for the parameter MESSAGE_SIZE.
 *
 *  Macro name definition for the parameter MESSAGE_SIZE.
 *  Corresponds to a string "messageSize".
 */


//----------------------------------------------------------------------------
/*!
 *  \def PARAM_NAME_PARITY_SIZE() codecbase.h
 *  \brief Macro name definition for the parameter PARITY_SIZE.
 *
 *  Macro name definition for the parameter PARITY_SIZE.
 *  Corresponds to a string "byteParityNumber".
 */


//----------------------------------------------------------------------------
/*!
 *  \def PARAM_NAME_POLYNOM() codecbase.h
 *  \brief Macro name definition for the parameter POLYNOM.
 *
 *  Macro name definition for the parameter POLYNOM.
 *  Corresponds to a string "polynom".
 */


//----------------------------------------------------------------------------
/*!
 *  \def PARAM_NAME_SEED() codecbase.h
 *  \brief Macro name definition for the parameter SEED.
 *
 *  Macro name definition for the parameter SEED.
 *  Corresponds to a string "seed".
 */


//----------------------------------------------------------------------------
/*!
 *  \def PARAM_NAME_INTERLEAVING_TYPE_BLOCK() codecbase.h
 *  \brief Macro name definition for the parameter INTERLEAVING_TYPE_BLOCK.
 *
 *  Macro name definition for the parameter INTERLEAVING_TYPE_BLOCK.
 *  Corresponds to a string "block".
 */


//----------------------------------------------------------------------------
/*!
 *  \def PARAM_NAME_INTERLEAVING_TYPE_FRAME() codecbase.h
 *  \brief Macro name definition for the parameter INTERLEAVING_TYPE_FRAME.
 *
 *  Macro name definition for the parameter INTERLEAVING_TYPE_FRAME.
 *  Corresponds to a string "frame".
 */


//----------------------------------------------------------------------------
/*!
 *  \def PARAM_NAME_SYMBOL_TYPE() codecbase.h
 *  \brief Macro name definition for the parameter SYMBOL_TYPE.
 *
 *  Macro name definition for the parameter SYMBOL_TYPE.
 *  Corresponds to a string "symboltype".
 */


//----------------------------------------------------------------------------
/*!
 *  \def PARAM_NAME_SYMBOL_TYPE_BIT() codecbase.h
 *  \brief Macro name definition for the parameter SYMBOL_TYPE_BIT.
 *
 *  Macro name definition for the parameter SYMBOL_TYPE_BIT.
 *  Corresponds to a string "bit".
 */


//----------------------------------------------------------------------------
/*!
 *  \def PARAM_NAME_SYMBOL_TYPE_BYTE() codecbase.h
 *  \brief Macro name definition for the parameter SYMBOL_TYPE_BYTE.
 *
 *  Macro name definition for the parameter SYMBOL_TYPE_BYTE.
 *  Corresponds to a string "byte".
 */


/*! 
 *  \typedef DBOOL (*boxing_codec_pencode)(void * codec, gvector * data)
 *  \brief Callback function to encode codec.
 *
 *  \param codec  Codec structure pointer.
 *  \param data   Data vector.
 *   
 *  Callback function to encode codec.
 */


/*! 
 *  \typedef DBOOL (*boxing_codec_pdecode)(void * codec, gvector * data, gvector * erasures, boxing_stats_decode * stats, void* user_data)
 *  \brief Callback function to decode codec.
 *
 *  \param codec      Codec structure pointer.
 *  \param data       Data vector.
 *  \param erasures   Erasures vector.
 *  \param stats      boxing_stats_decode structure pointer.
 *  \param user_data  User data vector pointer.
 *   
 *  Callback function to decode codec.
 */


/*! 
 *  \typedef DBOOL (*boxing_codec_pset_property)(void * codec, const char * name, const g_variant * value)
 *  \brief Callback function to set property.
 *
 *  \param codec  Codec structure pointer.
 *  \param name   Data vector.
 *  \param value  Erasures vector.
 *   
 *  Callback function to set property.
 */


/*! 
 *  \typedef void  (*reset_callback)(void *codec)
 *  \brief Callback function to reset codec.
 *
 *  \param codec  Codec structure pointer.
 *   
 *  Callback function to reset.
 */


/*! 
 *  \typedef void  (*free_callback)(struct boxing_codec_s *codec)
 *  \brief Callback function to free codec.
 *
 *  \param codec  Codec structure pointer.
 *   
 *  Callback function to free.
 */


/*! 
 *  \typedef DBOOL (*boxing_codec_init_capacity)(struct boxing_codec_s *codec, int size)
 *  \brief Callback function to init capacity.
 *
 *  \param codec  Codec structure pointer.
 *  \param size   New capacity.
 *   
 *  Callback function to init capacity.
 */


//----------------------------------------------------------------------------
/*!
 *  \struct   boxing_codec_s  boxing/codecs/codecbase.h
 *  \brief    Boxing codec data storage.
 *
 *  \param is_error_correcting  Error correcting.
 *  \param pre_zero_pad_data    Pre zero pad data.
 *  \param name                 Codec name.
 *  \param encode               Encode function.
 *  \param decode               Decode function.
 *  \param set_property         Set property function.
 *  \param init_capacity        Init capacity function.
 *  \param decoded_data_size    Decoded data size.
 *  \param encoded_data_size    Encoded data size.
 *  \param decoded_block_size   Decoded block size.
 *  \param encoded_block_size   Encoded block size.
 *  \param encoded_symbol_size  Encoded symbol size.
 *  \param decoded_symbol_size  Decoded symbol size.
 *  \param reentrant            Reentrant value.
 *  \param decode_cb            Decode callback function.
 *  \param encode_cb            Encode callback function.
 *  \param reset                Codec reset function.
 *  \param free                 Free codec function.
 *
 *  Boxing codec data storage.
 */


// PUBLIC CODEC BASE FUNCTIONS
//

//----------------------------------------------------------------------------
/*!
 *  \brief Initializes new capacity value.
 *
 *  Initializes new capacity value boxing_codec instance.
 *
 *  \param[in] codec  Pointer to the boxing_codec structure.
 *  \param[in] size   New capacity size.
 */

static DBOOL init_capacity(boxing_codec *codec, int size)
{
    int blocks = (size / codec->encoded_block_size);
    codec->decoded_data_size = codec->decoded_block_size * blocks;
    codec->encoded_data_size = codec->encoded_block_size * blocks;
    return DTRUE;
}


//----------------------------------------------------------------------------
/*!
 *  \brief Create a boxing codec instance.
 *
 *  Allocate memory for the boxing_codec type
 *  and initializes it according to input data.
 *  Return instance of allocated structure.
 *
 *  \param[in] codec_name  Codec name.
 *  \param[in] properties  Hash table with codec properties.
 *  \param[in] config      Pointer to the boxing_config structure.
 *  \return instance of allocated boxing_codec structure.
 */

boxing_codec * boxing_codec_create(const char *codec_name, GHashTable * properties, const boxing_config * config)
{
    if (strcmp(codec_name, "Interleaving") == 0)
    {
        return boxing_interleaving_create(properties, config);
    }
    else if (strcmp(codec_name, "Cipher") == 0)
    {
        return boxing_codec_cipher_create(properties, config);
    }
    else if (strcmp(codec_name, "ReedSolomon") == 0)
    {
        return boxing_codec_reedsolomon_create(properties, config);
    }
    else if (strcmp(codec_name, "Modulator") == 0)
    {
        return boxing_codec_modulator_create(properties, config);
    }
    else if (strcmp(codec_name, "SyncPointInserter") == 0)
    {
        return boxing_codec_syncpointinserter_create(properties, config);
    }
    else if (strcmp(codec_name, codec_crc32_name) == 0)
    {
        return boxing_crc32_create(properties, config);
    }
    else if (strcmp(codec_name, codec_crc64_name) == 0)
    {
        return boxing_crc64_create(properties, config);
    }
    else if (strcmp(codec_name, codec_ftf_interleaving_name) == 0)
    {
        return boxing_ftf_interleaving_create(properties, config);
    }
    else if (strcmp(codec_name, codec_packet_header_name) == 0)
    {
        return boxing_codec_packet_header_create(properties, config);
    }
    else if (strcmp(codec_name, codec_2dpam_name) == 0)
    {
        return boxing_codec_2dpam_create(properties, config);
    }
    else if (strcmp(codec_name, codec_symbol_converter_name) == 0)
    {
        return boxing_codec_symbol_converter_create(properties, config);
    }
    else if (strcmp(codec_name, codec_bch_name) == 0)
    {
        return boxing_codec_bch_create(properties, config);
    }
    else if (strcmp(codec_name, codec_ldpc_name) == 0)
    {
        return boxing_codec_ldpc_create(properties, config);
    }
    return NULL;
}


//----------------------------------------------------------------------------
/*!
 *  \brief Initializes all structure pointers with default values.
 *
 *  Initializes all structure pointers with default values.
 *
 *  \param[in] codec  Pointer to the boxing_codec structure.
 */

void boxing_codec_init_base(boxing_codec *codec)
{
    codec->is_error_correcting = DFALSE;
    codec->pre_zero_pad_data = DTRUE;
    codec->encoded_symbol_size = 8;
    codec->decoded_symbol_size = 8;
    codec->decode_cb = NULL;
    codec->reset = NULL;
    codec->reentrant = 1;
    codec->init_capacity = init_capacity;
    codec->decoded_data_size = 1;
    codec->encoded_data_size = 1;
    codec->decoded_block_size = 1;
    codec->encoded_block_size = 1;
}


//----------------------------------------------------------------------------
/*!
 *  \brief Base release boxing codec structure instance.
 *
 *  Base release boxing codec structure instance.
 *
 *  \param[in] codec  Pointer to the boxing_codec structure.
 */

void boxing_codec_release_base(boxing_codec *codec)
{
    BOXING_UNUSED_PARAMETER( codec );
}


//----------------------------------------------------------------------------
/*!
 *  \brief Release boxing codec structure instance.
 *
 *  Release boxing codec structure instance.
 *
 *  \param[in] codec  Pointer to the boxing_codec structure.
 */

void boxing_codec_release(boxing_codec *codec)
{
    codec->free(codec);
}


//----------------------------------------------------------------------------
/*!
  * \} end of codecs group
  */
