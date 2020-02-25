/*****************************************************************************
**
**  Implementation of the unboxer V1 interface
**
**  Creation date:  2014/01/03
**  Created by:     Ole Liabo
**
**
**  Copyright (c) 2014 Piql AS. All rights reserved.
**
**  This file is part of the boxing library
**
*****************************************************************************/

//  PROJECT INCLUDES
//
#include    "unboxerv1.h"
#include    "datapoints.h"
#include    "boxing/codecs/codecdispatcher.h"
#include    "boxing/codecs/cipher.h"
#include    "boxing/math/crc64.h"
#include    "boxing/math/crc32.h"
#include    "boxing/graphics/genericframe.h"
#include    "boxing/graphics/genericframefactory.h"
#include    "boxing/frame/tracker.h"
#include    "frameutil.h"
#include    "boxing/log.h"
#include    "boxing/platform/memory.h"
#include    "boxing/bool.h"

//  PRIVATE INTERFACE
//

static int          dunboxerv1_sharpness_filter(void* user_data, boxing_dunboxerv1 * unboxer, boxing_image8 * image, boxing_float mix);
static int          dunboxerv1_visual_sharpness_filter(void* user_data, boxing_dunboxerv1 * unboxer, boxing_image8 * image);
static DBOOL        dunboxerv1_calculate_lut(struct boxing_tracker_s * tracker, const boxing_image8 * image, boxing_image8_pixel * lut);
static void         dunboxerv1_apply_lut(boxing_image8 * image, const boxing_image8_pixel * lut);
static boxing_float dunboxerv1_calculate_mtf_max_min(struct boxing_tracker_s * tracker, const char * sampler_name,
                    const boxing_image8 * frame, boxing_float max, boxing_float min);
static int          dunboxerv1_calculate_mtf(boxing_dunboxerv1 * unboxer, const boxing_image8 * image,
                                                int symbols_per_pixel, struct boxing_tracker_s * tracker, void * user_data,
                                                boxing_float *horizontal_mtf, boxing_float *vertical_mtf);
static int      extract_digital_content( void* user, boxing_dunboxerv1 * unboxer, boxing_image8 * sampled_image, int symbols_per_pixel, gvector * the_data_array, DBOOL quantize_data );
static int      extract_analog_content(boxing_image8 * sampled_image, gvector *the_data_array, void * user_data);
static int      dunboxerv1_load_data_from_image(boxing_dunboxerv1 * unboxer, boxing_image8 * image,
                                                       gvector * the_data_array, boxing_metadata_list * metadata_list, int horizontal_border_tracking, 
                                                       struct boxing_tracker_s * tracker, void * user_data, DBOOL quantize_data);
static void     pack_data( gvector * data );
static int      dunboxerv1_decode_step(boxing_dunboxerv1 * unboxer, gvector * data, boxing_metadata_list * metadata,
                boxing_stats_decode * decode_stats, unsigned int step, void * user_data);


/*! 
  * \addtogroup unboxer
  * \{
  */


//----------------------------------------------------------------------------
/*!
 *  \struct  boxing_dunboxerv1_s  unboxerv1.h 
 *  \brief   Structure representing V1 of the unboxer.
 *
 *  \param parameters             Unboxer parameters.
 *  \param frame                  Frame.
 *  \param error_image            Error image.
 *  \param frame_util             Frame utility.
 *  \param codec                  Codec.
 *  \param metadata_codec         Metadata codec.
 *  \param preload_frames         Preload frames.
 *  \param quantize_data_on_load  Quantize data on load.
 *
 *  The unboxer is responsible for decoding an image into digital data and
 *  metadata.
 */


// PUBLIC UNBOXER V1 FUNCTIONS
//

//---------------------------------------------------------------------------- 
/*! \brief Create dunboxer v1.
 * 
 *  Create dunboxer v1 instance with default parameters.
 *
 *  \return instance of allocated boxing_dunboxerv1 structure.
 */

boxing_dunboxerv1 * boxing_dunboxerv1_create()
{
    boxing_dunboxerv1 * unboxer = BOXING_MEMORY_ALLOCATE_TYPE(boxing_dunboxerv1);
    unboxer->frame = NULL;
    boxing_unboxer_parameters_init( &unboxer->parameters );
    unboxer->frame_util = (boxing_abstract_frame_util*)boxing_frame_util_create();
    return unboxer;
}


//---------------------------------------------------------------------------- 
/*! \brief Free dunboxer v1 data
 * 
 *  Free data owned by dunboxer v1 structure and structure itself.
 *
 *  \param unboxer Pointer to the boxing_dunboxerv1 instance.
 */

void boxing_dunboxerv1_destroy(boxing_dunboxerv1 * unboxer)
{
    boxing_generic_frame_factory_free(unboxer->frame);
    boxing_frame_util_destroy(unboxer->frame_util);
    boxing_memory_free(unboxer);
}


//---------------------------------------------------------------------------- 
/*! \brief Setup configuration.
 * 
 *  Setup configuration function description.
 *
 *  \param unboxer Pointer to the boxing_dunboxerv1 instance.
 *  \return Setup configuration result.
 */

int boxing_dunboxerv1_setup_config(boxing_dunboxerv1 * unboxer)
{

    boxing_generic_frame_factory_free(unboxer->frame);
    unboxer->frame = boxing_generic_frame_factory_create(unboxer->parameters.format);

    if (!unboxer->frame)
    {
        DLOG_ERROR( "unboxing_set_config:  Failed to inititalize frame");
        return BOXING_UNBOXER_CONFIG_ERROR;
    }

    const char * frame_format_version = boxing_generic_frame_get_format(unboxer->frame);
    if ((boxing_string_equal("GPFv1.0", frame_format_version) == DTRUE) || (boxing_string_equal("GPFv1.1", frame_format_version) == DTRUE))
    {
        unboxer->quantize_data_on_load = DTRUE;
    }
    else
    {
        unboxer->quantize_data_on_load = DFALSE;
    }
    return BOXING_UNBOXER_OK;
}


//----------------------------------------------------------------------------
/*!
 *  \brief Unbox data and metadata.
 *
 *  The function tries to decode the data using best effort, this means it will
 *  continue the unboxing process even if extraction phase fails. If extraction
 *  fails a data container with content set to 0 will be generated and sent to 
 *  the decoding step. This to ensure that multi frame boxing formats 
 *  (striped data) can be decoded even if one of the frames fails.
 *  To detect that an error was ignored in the extraction phase, the user must 
 *  look at the extract_result parameter.
 *
 *  \param[in]      unboxer         Unboxer structure.
 *  \param[out]     data            Decoded digital data.
 *  \param[out]     metadata_list   Decoded metadata.
 *  \param[in]      image           Image to be decoded.
 *  \param[out]     extract_result  Result from data extraction phase of unboxing.
 *  \param[in,out]  user_data       User data.
 *  \return Unboxing decoding result status code
 */

int boxing_dunboxerv1_process(
    boxing_dunboxerv1 * unboxer,
    gvector * data,
    boxing_metadata_list * metadata_list,
    boxing_image8 * image,
    int * extract_result,
    void * user_data)
{
    boxing_stats_decode decode_stats = { 0, 0, 0.0f, 0.0f };
    boxing_image8 * frame = image;

    *extract_result = boxing_dunboxerv1_extract_container(unboxer, data, metadata_list, frame, user_data);
    if (*extract_result != BOXING_UNBOXER_OK)
    {
        boxing_codecdispatcher * dispatcher = boxing_dunboxerv1_dispatcher((boxing_unboxer *)unboxer, CODEC_DISPATCHER_DATA_CODING_SCHEME);

        if (!dispatcher)
        {
            return *extract_result;
        }

        int encoded_size = (int)boxing_codecdispatcher_get_encoded_packet_size(dispatcher);
        gvector_resize(data, encoded_size);
        boxing_memory_clear(data->buffer, encoded_size);
    }

    int decode_result = *extract_result;
    for(unsigned int step = 0; step < unboxer->codec->decode_codecs.size; step++ )
    {
        decode_result = boxing_dunboxerv1_decode(unboxer, data, metadata_list, &decode_stats, step, user_data);
        if (decode_result != BOXING_UNBOXER_OK)
        {
            break;
        }
    }

    return decode_result;
}

//----------------------------------------------------------------------------
/*!
 *  \brief Unbox metadata and extract a raw quantized copy of the data container.
 *
 *  Description.
 *
 *  \param[in]      unboxer        Unboxer structure
 *  \param[out]     data           Quantized data container.
 *  \param[out]     metadata_list  Decoded metadata
 *  \param[in]      frame          Image to be decoded.
 *  \param[in,out]  user_data      User data.
 *  \return Unboxing result status code.
 */

int boxing_dunboxerv1_extract_container(
    boxing_dunboxerv1 * unboxer, 
    gvector * data,
    boxing_metadata_list * metadata_list, 
    boxing_image8 * frame,
    void * user_data)
{
    if (!unboxer->frame)
    {
        DLOG_ERROR( "dunboxer_process:  Failed to inititalize frame");
        return BOXING_UNBOXER_CONFIG_ERROR;
    }

    int retval = BOXING_UNBOXER_OK;

    boxing_tracker * tracker = unboxer->frame->create_frame_tracker(unboxer->frame);

#ifdef BOXINGLIB_CALLBACK
    tracker->on_corner_mark_complete = unboxer->parameters.on_corner_mark_complete;
    tracker->user_data = user_data;

    if (unboxer->parameters.on_tracker_created)
    {
        if (unboxer->parameters.on_tracker_created(user_data, &retval, tracker) != BOXING_PROCESS_CALLBACK_OK)
        {
            return BOXING_UNBOXER_PROCESS_CALLBACK_ABORT;
        }
    }
#endif
    retval = dunboxerv1_load_data_from_image(unboxer, frame, data, metadata_list, DTRUE, tracker, user_data, unboxer->quantize_data_on_load);
    boxing_tracker_destroy(tracker);

#ifdef BOXINGLIB_CALLBACK
    if (unboxer->parameters.on_content_quantized)
    {
        if (unboxer->parameters.on_content_quantized(user_data, &retval, (char *)data->buffer, (int)data->size) != BOXING_PROCESS_CALLBACK_OK)
        {
            return BOXING_UNBOXER_PROCESS_CALLBACK_ABORT;
        }
    }
#endif
    
    return retval; 
}


//----------------------------------------------------------------------------
/*!
 *  \brief Decode data container.
 *
 *  Description.
 *
 *  \param[in]      unboxer       Unboxer structure
 *  \param[out]     data          Quantized data container.
 *  \param[out]     metadata      Decoded metadata
 *  \param[in]      decode_stats  Decode statistics.
 *  \param[in]      step          Current step.
 *  \param[in,out]  user_data     User data.
 *  \return Decode result status code.
 */

int boxing_dunboxerv1_decode(boxing_dunboxerv1 * unboxer, gvector * data, boxing_metadata_list * metadata, boxing_stats_decode * decode_stats, unsigned int step, void * user_data)
{
    boxing_metadata_item_content_type * item = (boxing_metadata_item_content_type *)boxing_metadata_list_find_item(metadata, BOXING_METADATA_TYPE_CONTENTTYPE);
    if (item && item->value == BOXING_METADATA_CONTENT_TYPES_VISUAL)
    {
        DLOG_INFO( "boxing_dunboxerv1_decode:  Skipping decode, visual content" );
        return BOXING_UNBOXER_OK;
    }

    int retval = dunboxerv1_decode_step(unboxer, data, metadata, decode_stats, step, user_data);
#ifdef BOXINGLIB_CALLBACK
    if (unboxer->parameters.on_all_complete && ((retval != BOXING_UNBOXER_OK) || (step == (unboxer->codec->encode_codecs.size - 1))))
    {
        int res = retval;
        if (unboxer->parameters.on_all_complete(user_data, &res, decode_stats) != BOXING_PROCESS_CALLBACK_OK)
        {
            return BOXING_UNBOXER_PROCESS_CALLBACK_ABORT;
        }
    }
#endif
    return retval;
}


//----------------------------------------------------------------------------
/*!
 *  \brief Dispatcher function.
 *
 *  Dispatcher function description.
 *
 *  \param[in]      unboxer        Unboxer structure.
 *  \param[out]     coding_scheme  Coding scheme.
 *  \return boxing_codecdispatcher instance.
 */

boxing_codecdispatcher * boxing_dunboxerv1_dispatcher(const boxing_unboxer * unboxer, const char * coding_scheme)
{
    boxing_codecdispatcher * dispatcher = NULL;

    if(!unboxer)
    {
        return NULL;    
    }

   if(boxing_string_equal(coding_scheme, CODEC_DISPATCHER_DATA_CODING_SCHEME) == DTRUE)
    {
        dispatcher = ((boxing_dunboxerv1 *)unboxer)->codec;
    } 
    else if(boxing_string_equal(coding_scheme, CODEC_DISPATCHER_METADATA_CODING_SCHEME) == DTRUE)
    {
        dispatcher = ((boxing_dunboxerv1 *)unboxer)->metadata_codec;
    }
    
    return dispatcher;
}


//----------------------------------------------------------------------------
/*!
  * \} end of unboxer group
  */


// PRIVATE UNBOXER V1 FUNCTIONS
//

//----------------------------------------------------------------------------
/*!
 *  Decode data using the 
 *
 *  \param[in]  unboxer    Unboxer structure
 *  \param[in/out] data    Quantized data container
 *  \param[out] decode_stats Statistics
 *  \param[in] step        Decoding step
 *  \param[in] user_data   User data 
 *  \return Unboxing result status code
 */

static int dunboxerv1_decode_step(boxing_dunboxerv1 * unboxer, gvector * data, boxing_metadata_list * metadata, boxing_stats_decode * decode_stats, unsigned int step, void * user_data)
{
#ifndef BOXINGLIB_CALLBACK
	BOXING_UNUSED_PARAMETER(user_data);
#endif

    int retval = BOXING_UNBOXER_OK;

    if (step == 0)
    {        
        if (boxing_metadata_list_contains_item(metadata, BOXING_METADATA_TYPE_CONTENTTYPE))
        {
            int content_type = ((boxing_metadata_item_content_type *)boxing_metadata_list_find_item(metadata, BOXING_METADATA_TYPE_CONTENTTYPE))->value;

            if ((content_type == BOXING_METADATA_CONTENT_TYPES_DATA) ||
                (content_type == BOXING_METADATA_CONTENT_TYPES_TOC) ||
                (content_type == BOXING_METADATA_CONTENT_TYPES_CONTROLFRAME))
            {
                boxing_codecdispatcher * the_dispatcher = unboxer->codec;

                // For backward compatibility
                if (the_dispatcher->symbol_alignment == BOXING_CODEC_SYMBOL_ALIGNMENT_BIT)
                {
                    int symbols_per_pixel = 2;
                    if (boxing_metadata_list_contains_item(metadata, BOXING_METADATA_TYPE_SYMBOLSPERPIXEL))
                    {
                        symbols_per_pixel = ((boxing_metadata_item_symbols_per_pixel *)boxing_metadata_list_find_item(metadata, BOXING_METADATA_TYPE_SYMBOLSPERPIXEL))->value;
                    }

                    if (symbols_per_pixel != 2)
                    {
                        DLOG_ERROR1("dunboxer_process:  Unsupported number of symbols per pixel %i", symbols_per_pixel);
                        retval = BOXING_UNBOXER_DATA_DECODE_ERROR;
                        return retval;
                    }
                    // convert data oldschool style
                    pack_data(data);
                }
            }
        }

        decode_stats->fec_accumulated_amount = 0;
        decode_stats->fec_accumulated_weight = 0;
        decode_stats->resolved_errors = 0;
        decode_stats->unresolved_errors = 0;
    }

    if ( data->size > (4028 * 2092) )
    {
        DLOG_WARNING2( "Strange data size(%ld) while processing step %d - Suspecting corrupt memory", data->size, step );
    }

    boxing_codecdispatcher * the_dispatcher = unboxer->codec;

    if( step < the_dispatcher->encode_codecs.size)
    {
        boxing_codec_cipher codec_cipher_copy;
        // make a copy of the codec to ensure reentrancy
        boxing_codec * codec = boxing_codecdispatcher_get_decode_codec(the_dispatcher, step);
        if( boxing_string_equal(codec->name, "Cipher") )
        {
            memcpy(&codec_cipher_copy, codec, sizeof(boxing_codec_cipher));
            codec = (boxing_codec *)&codec_cipher_copy;
            
            if (boxing_metadata_list_contains_item(metadata, BOXING_METADATA_TYPE_CIPHERKEY))
            {
                ((boxing_codec_cipher *)codec)->key = ((boxing_metadata_item_cipher_key *)boxing_metadata_list_find_item(metadata, BOXING_METADATA_TYPE_CIPHERKEY))->value;
            }
        }

#ifdef BOXINGLIB_CALLBACK
        if (unboxer->parameters.on_decode_step != NULL)
        {
            boxing_codec * current_codec = boxing_codecdispatcher_get_decode_codec(the_dispatcher, step);
            if (unboxer->parameters.on_decode_step(user_data, &retval, data->buffer, (int)data->size, step,
                    current_codec->is_error_correcting, current_codec->encoded_block_size - current_codec->decoded_block_size, current_codec->encoded_block_size) != BOXING_PROCESS_CALLBACK_OK)
            {
                return BOXING_UNBOXER_PROCESS_CALLBACK_ABORT;
            }
        }
#endif

        // old school codec does not calculate CRC, but last decoder will fail
        // if any errors are detected
        retval = boxing_codecdispatcher_decode_step_codec(codec, data, NULL, decode_stats, user_data) ? BOXING_UNBOXER_OK : BOXING_UNBOXER_DATA_DECODE_ERROR;

        if (retval != BOXING_UNBOXER_OK)
        {
            decode_stats->fec_accumulated_amount = decode_stats->fec_accumulated_weight;
            return retval;
        }
 
    }

    if( step == (the_dispatcher->encode_codecs.size - 1) )
    {
        if(data->size == 0)
        {
            return retval;
        }

        if(boxing_metadata_list_contains_item(metadata, BOXING_METADATA_TYPE_DATASIZE))
        {
            int data_size = ((boxing_metadata_item_data_size *)boxing_metadata_list_find_item(metadata, BOXING_METADATA_TYPE_DATASIZE))->value;
            gvector_resize(data, data_size);
        }
        // validate data
        if (boxing_metadata_list_contains_item(metadata, BOXING_METADATA_TYPE_DATACRC))
        {
            uint64_t data_checksum = ((boxing_metadata_item_data_crc *)boxing_metadata_list_find_item(metadata, BOXING_METADATA_TYPE_DATACRC))->value;
            dcrc64 * calc_crc = boxing_math_crc64_create_def();
            uint64_t checksum = boxing_math_crc64_calc_crc(calc_crc, (char *)data->buffer, (unsigned int)data->size);
            boxing_math_crc64_free(calc_crc);
            if (data_checksum != checksum )
            {
                DLOG_ERROR2( "dunboxer_process:  Data checksum differs; expected 0x%016.16llX but found 0x%016.16llX", data_checksum,  checksum );
                DLOG_ERROR2( "dunboxer_process:  Resolved errors: %u unresolved errors: ", decode_stats->resolved_errors, decode_stats->unresolved_errors);
                retval = BOXING_UNBOXER_CRC_MISMATCH_ERROR;
                decode_stats->fec_accumulated_amount = decode_stats->fec_accumulated_weight;
            }
        }
    
        if (retval != BOXING_UNBOXER_OK)
        {
            DLOG_ERROR( "dunboxer_process:  Fatal error restoring data.");
        }


    }
    return retval;
}

static int dunboxerv1_sharpness_filter(void* user_data, boxing_dunboxerv1 * unboxer, boxing_image8 * image, boxing_float mix)
{
    if ( !unboxer->parameters.pre_filter.coeff )
    {
        return BOXING_FILTER_CALLBACK_ERROR;
    }

    const int width = image->width;
    const int height = image->height;
    boxing_filter_coeff_2d filter_coeff;
    filter_coeff.rows = unboxer->parameters.pre_filter.coeff->rows;
    filter_coeff.cols = unboxer->parameters.pre_filter.coeff->cols;
    int coeff_count = filter_coeff.rows*filter_coeff.cols;
    filter_coeff.coeff = BOXING_STACK_ALLOCATE_TYPE_ARRAY(boxing_float, coeff_count);
    for(int i = 0; i < coeff_count; i++)
    {
        filter_coeff.coeff[i] = unboxer->parameters.pre_filter.coeff->coeff[i] * mix;
    }
    filter_coeff.coeff[coeff_count/2] = unboxer->parameters.pre_filter.coeff->coeff[coeff_count/2] * mix + (1 - mix);

    if (unboxer->parameters.pre_filter.process)
    {
        int pre_res = unboxer->parameters.pre_filter.process(user_data, image, filter_coeff.coeff, coeff_count);
        BOXING_STACK_FREE( filter_coeff.coeff );
        return pre_res;
    }
    else
    {
        boxing_image8 destination;
        boxing_image8_init_in_place(&destination, width, height);
        if (destination.data == NULL)
        {
            DLOG_ERROR( "unboxer_sharpness_filter:  Can't allocate temporary image data! Filter is not applied!");
            BOXING_STACK_FREE( filter_coeff.coeff );
            return BOXING_FILTER_CALLBACK_ERROR;
        }
        boxing_image8_pixel * destination_pixel = destination.data;
        const int rows = filter_coeff.rows;
        const int cols = filter_coeff.cols;
        const int filter_y_radius = rows / 2;
        const int filter_x_radius = cols / 2;
        boxing_float * coeff = filter_coeff.coeff;

        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                int Y = (y - filter_y_radius + height) % height;

                boxing_float value = 0.0f;

                for (int i = 0; i < rows; i++)
                {
                    int X = x - filter_x_radius + width;
                    const boxing_image8_pixel * pixel = image->data + Y * width;
                    for (int j = 0; j < cols; j++)
                    {
                        value += *(coeff + i*cols + j) * *(pixel + (X++) % width);
                    }

                    Y = ((Y + 1) % height);
                }

                value = BOXING_MATH_CLAMP(BOXING_PIXEL_MIN, BOXING_PIXEL_MAX, value);
                *destination_pixel++ = (boxing_image8_pixel)value;
            }
        }
        memcpy(image->data, destination.data, width * height);
        boxing_memory_free(destination.data);
    }

    BOXING_STACK_FREE( filter_coeff.coeff );
    return BOXING_FILTER_CALLBACK_OK;
}

static int dunboxerv1_visual_sharpness_filter(void* user_data, boxing_dunboxerv1 * unboxer, boxing_image8 * image)
{
    if (!unboxer->parameters.pre_filter.coeff)
    {
        return BOXING_FILTER_CALLBACK_ERROR;
    }
    const int width = image->width;
    const int height = image->height;

    boxing_filter_coeff_2d filter_coeff;
    filter_coeff.rows = 5;
    filter_coeff.cols = 5;
    const int coeff_count = filter_coeff.rows * filter_coeff.cols;

    float coefficients[] =
    {
        0.00048091f, 0.00501119f, 0.01094545f, 0.00501119f, 0.00048091f,
        0.00501119f, 0.05221780f, 0.11405416f, 0.05221780f, 0.00501119f,
        0.01094545f, 0.11405416f, 0.24911720f, 0.11405416f, 0.01094545f,
        0.00501119f, 0.05221780f, 0.11405416f, 0.05221780f, 0.00501119f,
        0.00048091f, 0.00501119f, 0.01094545f, 0.00501119f, 0.00048091f
    };
    filter_coeff.coeff = coefficients;


    if (unboxer->parameters.pre_filter.process != NULL)
    {
        return unboxer->parameters.pre_filter.process(user_data, image, filter_coeff.coeff, coeff_count);
    }
    else
    {
        boxing_image8 destination;
        boxing_image8_init_in_place(&destination, width, height);
        if (destination.data == NULL)
        {
            DLOG_ERROR( "unboxer_visual_sharpness_filter:  Can't allocate temporary image data! Filter is not applied!");
            return BOXING_FILTER_CALLBACK_ERROR;
        }

        boxing_image8_pixel * destination_pixel = destination.data;
        const int rows = filter_coeff.rows;
        const int cols = filter_coeff.cols;
        const int filter_y_radius = rows / 2;
        const int filter_x_radius = cols / 2;
        boxing_float * coeff = filter_coeff.coeff;

        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                int Y = (y - filter_y_radius + height) % height;

                boxing_float value = 0.0f;

                for (int i = 0; i < rows; i++)
                {
                    int X = x - filter_x_radius + width;
                    const boxing_image8_pixel * pixel = image->data + Y * width;
                    for (int j = 0; j < cols; j++)
                    {
                        value += *(coeff + i*cols + j) * *(pixel + (X++) % width);
                    }

                    Y = ((Y + 1) % height);
                }

                value = BOXING_MATH_CLAMP(BOXING_PIXEL_MIN, BOXING_PIXEL_MAX, value);
                *destination_pixel++ = (boxing_image8_pixel)value;
            }
        }
        memcpy(image->data, destination.data, width * height);
        boxing_memory_free(destination.data);
    }

    return BOXING_FILTER_CALLBACK_OK;
}

static DBOOL dunboxerv1_calculate_lut(struct boxing_tracker_s * tracker, const boxing_image8 * image, boxing_image8_pixel* lut)
{
    if(!(tracker->mode & BOXING_TRACK_CALIBRATION_BAR))
    {
        return DFALSE;
    }

    boxing_sampler * content_sampler = (boxing_sampler *)g_hash_table_lookup(tracker->container_sampler_list, BOXING_SAMPLE_BAR_CALIBRATION);
    if(!content_sampler || !content_sampler->state)
    {
        return DFALSE;
    }

    boxing_image8 * calibration_bar_image = content_sampler->sample(content_sampler, image);

    boxing_image8_pixel *sampled_lut = calibration_bar_image->data;

    int j = 0;
    for(int i = 0; i < 256; i++)
    {
        while(j < 255)
        {
            if(sampled_lut[j] > (boxing_image8_pixel)i)
            {
                break;
            }
            j++;
        }
        lut[i] = (boxing_image8_pixel)j;
    }

    boxing_image8_free(calibration_bar_image);

    return DTRUE;
}

static void dunboxerv1_apply_lut(boxing_image8 * image, const boxing_image8_pixel * lut)
{
    const size_t image_size = image->width * image->height;
    for (size_t i = 0; i < image_size; ++i)
    {
        image->data[i] = lut[image->data[i]];
    }
}

/**
 * \brief This function byte interleaves data.
 *
 * \param[out] dst       Interleaved data
 * \param[in]  src       Source data
 * \param[in]  distance  Interleave distance
 */
static void byte_interleaving(gvector * dst, const gvector * src, uint32_t distance)
{
    const char * src_end = (const char *)src->buffer + src->size;
    char * dst_pointer = (char *)dst->buffer;

    for (uint32_t i = 0; i < distance; i++)
    {
        const char * src_pointer = (const char *)src->buffer + i;

        while (src_pointer < src_end)
        {
            *dst_pointer = *src_pointer;

            src_pointer += distance;
            dst_pointer++;
        }
    }
}


/**
 * \brief This function extracts the raw metadata bytes from a samped metadata container.
 *
 * \param[in] samples     Sampled metadata container
 *  \return Raw metadata byte stream
 */
static gvector * extract_metadata_bytes(boxing_image8 * samples)
{
    gvector * metadata_bytes = boxing_datapoints_quantize(samples, samples->width, samples->height, 2);
    gvector * metadata_bytes_interleaved = gvector_create_char(metadata_bytes->size, 0);

    byte_interleaving(metadata_bytes_interleaved, metadata_bytes, samples->width);

    gvector_free(metadata_bytes);

    return metadata_bytes_interleaved;
}


//----------------------------------------------------------------------------
/*! 
 *  Decode metadata in image  
 *
 *  \return DBOOL
 */

static DBOOL decode_metadata(
    boxing_metadata_list * metadata_list,
    const boxing_image8 * frame, 
    boxing_sampler * sampler, 
    boxing_codecdispatcher * codec, 
    void* user_data)
{
    gvector* metadata_bytes = NULL;

    // Sample and convert samples to bytearray
    {
        boxing_image8 * samples = sampler->sample( sampler, frame );
        int capacity = samples->width * samples->height;

        metadata_bytes = extract_metadata_bytes( samples );
        gvector_resize( metadata_bytes, capacity );
        boxing_image8_free( samples );
    }

    // packed
    if(codec->symbol_alignment == BOXING_CODEC_SYMBOL_ALIGNMENT_BIT)
    {
        gvector * packed_data = gvector_create(1, (metadata_bytes->size + 7) / 8);
        unsigned int counter = 0;
        const char * metadata_end = (char *)metadata_bytes->buffer + metadata_bytes->size;
        for(char * it_metadata = (char *)metadata_bytes->buffer; it_metadata != metadata_end; )
        {
            char ch = 0;
            for(int bit = 0; (bit < 8) && (it_metadata != metadata_end); bit++, it_metadata++)
            {
                ch <<= 1;
                ch |= (*it_metadata) & 1;
            }
            GVECTORN8(packed_data, counter++) = ch;
        }
        gvector_swap(metadata_bytes, packed_data);
        gvector_free(packed_data);
    }

    boxing_stats_decode decode_stats = {0, 0, 0.0f, 0.0f};
    
    if (!boxing_codecdispatcher_decode(codec, metadata_bytes, &decode_stats, user_data))
    {
        DLOG_ERROR( "unboxing_decode_metadata:  Unable to decode metadata");
        DLOG_ERROR2( "Resolved errors: %u unresolved errors: %u", decode_stats.resolved_errors, decode_stats.unresolved_errors);

        gvector_free(metadata_bytes);
        return DFALSE;
    }

    if (boxing_codecdispatcher_version_cmp(&codec->version, &BOXING_CODEC_DISPATCHER_1_0) < 0)
    {
        // validate checksum
        dcrc32 * calc_crc = boxing_math_crc32_create_def();
        boxing_uint32 checksum = boxing_math_crc32_calc_crc(calc_crc, (char *)metadata_bytes->buffer, (unsigned int)metadata_bytes->size);
        boxing_math_crc32_free(calc_crc);
        if (checksum != 0)
        {
            DLOG_ERROR( "unboxing_decode_metadata:  Metadata does not contain a valid checksum");
            gvector_free( metadata_bytes );
            return DFALSE;
        }
    }

    // extract meta data
    boxing_metadata_list_deserialize(metadata_list, (char *)metadata_bytes->buffer, metadata_bytes->item_size * metadata_bytes->size);
    gvector_free(metadata_bytes);
    return DTRUE;
}

// If mtf calculation is not possible, mtf is set to -1
static boxing_float dunboxerv1_calculate_mtf_max_min(
    struct boxing_tracker_s * tracker, 
    const char * sampler_name,
    const boxing_image8 * frame, 
    boxing_float max,
    boxing_float min)
{
    boxing_sampler * content_sampler = (boxing_sampler *)g_hash_table_lookup(tracker->container_sampler_list, sampler_name);
    if(!content_sampler || !content_sampler->state)
    {
        return -1.0f;
    }
    
    boxing_image8 * sample_image = content_sampler->sample(content_sampler, frame);
    int data_size = sample_image->width * sample_image->height;

    boxing_image8_pixel * ptr = sample_image->data;
    boxing_float average_black = 0.0f;
    boxing_float average_white = 0.0f;
    int samples = 0;

    if(data_size < 1)
    {
        return -1.0f;
    }

    while(samples < data_size)
    {
        average_white += *ptr++;
        average_black += *ptr++;
        samples  += 2;
    }

    boxing_image8_free(sample_image);

    // normalize values
    average_white /= samples/2;
    average_black /= samples/2;

    // calc MTF
    return (average_white - average_black)/(max - min);
}

// If mtf calculation is not possible, mtf is set to -1
static int dunboxerv1_calculate_mtf(
    boxing_dunboxerv1 * unboxer,
    const boxing_image8 * image, 
    int symbols_per_pixel, 
    struct boxing_tracker_s * tracker,
    void * user_data,
    boxing_float *horizontal_mtf,
    boxing_float *vertical_mtf)
{
#ifndef BOXINGLIB_CALLBACK
	BOXING_UNUSED_PARAMETER(user_data);
	BOXING_UNUSED_PARAMETER(unboxer);
#endif

    boxing_stats_mtf reference_bar_stats = { -1.0, -1.0, -1.0, -1.0 };
    
    if(!(tracker->mode & BOXING_TRACK_CALIBRATION_BAR))
    {
        return BOXING_UNBOXER_OK;
    }

    boxing_sampler * content_sampler = (boxing_sampler *)g_hash_table_lookup(tracker->container_sampler_list, BOXING_SAMPLE_BAR_CALIBRATION);
    if(!content_sampler || !content_sampler->state)
    {
        return BOXING_UNBOXER_OK;
    }

    boxing_image8 * calibration_bar_image = content_sampler->sample(content_sampler, image);
    boxing_image8 * lut = calibration_bar_image;
    boxing_float reference_black = 0;
    boxing_float reference_white = 0;
    int lut_size = calibration_bar_image->width * calibration_bar_image->height;
    boxing_image8_pixel *lut_data = lut->data;

    int lut_samples_per_symbol = lut_size/symbols_per_pixel;
    if(lut_samples_per_symbol < 1)
    {
        boxing_image8_free(calibration_bar_image);
        return BOXING_UNBOXER_OK;
    }

    for(int i = 0; i < lut_samples_per_symbol; i++)
    {
        reference_black += lut_data[i];
        reference_white += lut_data[lut_size - 1 - i];
    }

    reference_black /= lut_samples_per_symbol;
    reference_white /= lut_samples_per_symbol;

    char buffer[100];
    size_t samplebar_str_length = strlen(BOXING_SAMPLE_BAR_REFERENCE);
    memcpy(buffer, BOXING_SAMPLE_BAR_REFERENCE, samplebar_str_length);
    memcpy(buffer + samplebar_str_length, ".left", strlen(".left") + 1);
    reference_bar_stats.left_vertical_mtf = (boxing_float)dunboxerv1_calculate_mtf_max_min(tracker, buffer,
            image, reference_white, reference_black);
    memcpy(buffer + samplebar_str_length, ".right", strlen(".right") + 1);
    reference_bar_stats.right_vertical_mtf = (boxing_float)dunboxerv1_calculate_mtf_max_min(tracker, buffer, image,
            reference_white, reference_black);
    memcpy(buffer + samplebar_str_length, ".top", strlen(".top") + 1);
    reference_bar_stats.top_horizontal_mtf = (boxing_float)dunboxerv1_calculate_mtf_max_min(tracker, buffer, image,
            reference_white, reference_black);
    memcpy(buffer + samplebar_str_length, ".bottom", strlen(".bottom") + 1);
    reference_bar_stats.bottom_horizontal_mtf = (boxing_float)dunboxerv1_calculate_mtf_max_min(tracker, buffer, image,
            reference_white, reference_black);
    *horizontal_mtf = (reference_bar_stats.top_horizontal_mtf + reference_bar_stats.bottom_horizontal_mtf) / 2;
    *vertical_mtf = (reference_bar_stats.left_vertical_mtf + reference_bar_stats.right_vertical_mtf) / 2;
    DLOG_INFO1( "dunboxer_calc_mtf:  Left MTF       : %f", reference_bar_stats.left_vertical_mtf);
    DLOG_INFO1( "dunboxer_calc_mtf:  Right MTF      : %f", reference_bar_stats.right_vertical_mtf);
    DLOG_INFO1( "dunboxer_calc_mtf:  Top MTF        : %f", reference_bar_stats.top_horizontal_mtf);
    DLOG_INFO1( "dunboxer_calc_mtf:  Bottom MTF     : %f", reference_bar_stats.bottom_horizontal_mtf);
    DLOG_INFO1( "dunboxer_calc_mtf:  Horizontal MTF : %f", horizontal_mtf);
    DLOG_INFO1( "dunboxer_calc_mtf:  Vertical MTF   : %f", vertical_mtf);
    DLOG_INFO1( "dunboxer_calc_mtf:  MTF            : %f", ((*horizontal_mtf) * (*vertical_mtf)));

    boxing_image8_free(calibration_bar_image);
	int retval = BOXING_UNBOXER_OK;
#ifdef BOXINGLIB_CALLBACK
    if (unboxer->parameters.on_reference_bar_complete && 
        unboxer->parameters.on_reference_bar_complete(user_data, &retval, &reference_bar_stats) != BOXING_PROCESS_CALLBACK_OK)
    {
        return BOXING_UNBOXER_PROCESS_CALLBACK_ABORT;
    }
#endif
    return retval;
}

 static int extract_digital_content(void* user, boxing_dunboxerv1 * unboxer, boxing_image8 * sampled_image, int symbols_per_pixel, gvector * the_data_array, DBOOL quantize_data)
{
    if (!quantize_data)
    {
        gvector_resize(the_data_array, sampled_image->width * sampled_image->height);
        memcpy(the_data_array->buffer, sampled_image->data, the_data_array->size);
        return BOXING_UNBOXER_OK;
    }

    if ( unboxer->parameters.quantize_contents )
    {
        gvector_replace( the_data_array, unboxer->parameters.quantize_contents( user, sampled_image, 32, 32, symbols_per_pixel ) );
    }
    else
    {
        gvector_replace(the_data_array, boxing_datapoints_quantize(sampled_image, 32, 32, symbols_per_pixel));
    }
    return  BOXING_UNBOXER_OK;
}

static int extract_analog_content(boxing_image8 * sampled_image, gvector *the_data_array, void * user_data)
{
    BOXING_UNUSED_PARAMETER(user_data);
    gvector_resize(the_data_array, sampled_image->width * sampled_image->height);
    memcpy(the_data_array->buffer, sampled_image->data, sampled_image->width * sampled_image->height);
    
    DLOG_INFO2("unboxing_load_data_from_image: Image height %d, width %d", sampled_image->height, sampled_image->width);
    
    gvector_append_data(the_data_array, 4, &sampled_image->width);
    gvector_append_data(the_data_array, 4, &sampled_image->height);
    return  BOXING_UNBOXER_OK;
}

static boxing_float dunboxerv1_calculate_filter_mix(boxing_float sampling_rate, boxing_float mtf, boxing_filter_coeff_2d * coeff)
{
    if(mtf <= 0.0f)
    {
        DLOG_WARNING( "Negative MTF");
        return 1.0f;
    }
    boxing_float filter_gain = 0.0f;
    int rows = coeff->rows;
    int cols = coeff->cols;

    for(int row = 0; row < rows; row++)
    {
        for(int col = 0; col < cols; col++)
        {
            filter_gain += coeff->coeff[row*rows + col] * (boxing_float)cos( (col-(cols/2))*(boxing_float)BOXING_MATH_PI/sampling_rate );
        }
    }

    if(filter_gain <= 0.0f)
    {
        DLOG_WARNING( "Negative filter gain");
        return 1.0f;
    }

    boxing_float gain_required = 1.0f / mtf;
    boxing_float mix = 1.0f;
    if(gain_required < filter_gain)
    {
        mix = gain_required/filter_gain;
        mix = (mix > 1.0f) ? 1.0f: mix;
    }
    return mix;
}

static int dunboxerv1_load_data_from_image(
    boxing_dunboxerv1 * unboxer,
    boxing_image8 * image, 
    gvector * the_data_array, 
    boxing_metadata_list * metadata_list,
    int horizontal_border_tracking,
    struct boxing_tracker_s * tracker,
    void * user_data,
    DBOOL quantize_data)
{
    int retval = BOXING_UNBOXER_OK;

    if (!unboxer->frame)
    {
        return BOXING_UNBOXER_CONFIG_ERROR; /// Should be assert?
    }

    int tracker_mode = tracker->mode;
    
    if (unboxer->parameters.is_raw)
    {
        tracker_mode |= BOXING_TRACK_SIMULATED;
    }
    else
    {
        tracker_mode &= ~BOXING_TRACK_SIMULATED;
    }

    if (horizontal_border_tracking)
    {
        tracker_mode |= BOXING_TRACK_HORIZONTAL_SHIFT;
    }
    else
    {
        tracker_mode &= ~BOXING_TRACK_HORIZONTAL_SHIFT;
    }

    tracker_mode |= BOXING_TRACK_REFERENCE_MARKS;

    tracker_mode |= BOXING_TRACK_SYNC_POINTS;

    tracker->mode = tracker_mode;

    if (tracker->track_frame(tracker, image))
    {
        DLOG_ERROR( "unboxing_load_data_from_image:  Tracking frame failed");
        return BOXING_UNBOXER_BORDER_TRACKING_ERROR;
    }

    // Extract metadata
    boxing_sampler * metadata_sampler = boxing_tracker_get_container_sampler(tracker, BOXING_SAMPLE_CONTAINER_METADATA);
    if (!metadata_sampler || !metadata_sampler->state)
    {
        DLOG_ERROR( "unboxing_load_data_from_image:  Meta data locations not vaild");
        return BOXING_UNBOXER_METADATA_ERROR;
    }

    
    if (!decode_metadata(metadata_list, image, metadata_sampler, unboxer->metadata_codec, user_data))
    {
        DLOG_WARNING( "unboxing_load_data_from_image:  Decoding meta data failed");
        retval = BOXING_UNBOXER_METADATA_ERROR;
    }

#ifdef BOXINGLIB_CALLBACK
    if (unboxer->parameters.on_metadata_complete)
    {
        // clone meta data ??
        int res = unboxer->parameters.on_metadata_complete(user_data, &retval, metadata_list);
        if (res != BOXING_PROCESS_CALLBACK_OK)
        {
            return BOXING_UNBOXER_PROCESS_CALLBACK_ABORT;
        }
    }
#endif

    if (retval != BOXING_UNBOXER_OK)
    {
        return retval;
    }

    boxing_codecdispatcher * codecdispatcher = boxing_unboxer_dispatcher((boxing_unboxer *)unboxer, CODEC_DISPATCHER_DATA_CODING_SCHEME);
    if (!codecdispatcher)
    {
        return BOXING_UNBOXER_CONFIG_ERROR;
    }

    int symbols_per_pixel = (1 << codecdispatcher->color_depth);

    if (boxing_metadata_list_contains_item(metadata_list,  BOXING_METADATA_TYPE_SYMBOLSPERPIXEL))
    {
        symbols_per_pixel = ((boxing_metadata_item_symbols_per_pixel *)boxing_metadata_list_find_item(metadata_list, BOXING_METADATA_TYPE_SYMBOLSPERPIXEL))->value;
    }
    else
    {
        DLOG_WARNING1("unboxing_load_data_from_image:  Meta data doesn't contain \"SymbolsPerPixel\", using default instead: %d", symbols_per_pixel);
    }
    

    /// \todo warning: variable 'content_symbol_size' set but not used
    int content_symbol_size = 1;
    if (boxing_metadata_list_contains_item(metadata_list, BOXING_METADATA_TYPE_CONTENTSYMBOLSIZE))
    {
        content_symbol_size = ((boxing_metadata_item_content_symbol_size *)boxing_metadata_list_find_item(metadata_list, BOXING_METADATA_TYPE_CONTENTSYMBOLSIZE))->value;
    }
    (void)content_symbol_size;
    
    if (!boxing_metadata_list_contains_item(metadata_list, BOXING_METADATA_TYPE_CONTENTTYPE))
    {
        // append missing metadata
        boxing_metadata_item_content_type * content_type = (boxing_metadata_item_content_type *)boxing_metadata_item_create(BOXING_METADATA_TYPE_CONTENTTYPE);
        content_type->value = BOXING_METADATA_CONTENT_TYPES_DATA;
        boxing_metadata_list_append_item(metadata_list, (boxing_metadata_item *)content_type);

        // should this be handled with a switch ??
        DLOG_WARNING("unboxing_load_data_from_image: metadat does not contain content type, defaulting to 'BOXING_METADATA_CONTENT_TYPES_DATA'");
    }
    
    // Analogue data?
    DBOOL is_analogue_data = DFALSE;
    if (boxing_metadata_list_contains_item(metadata_list, BOXING_METADATA_TYPE_CONTENTTYPE))
    {
        int content_type = ((boxing_metadata_item_content_type *)boxing_metadata_list_find_item(metadata_list, BOXING_METADATA_TYPE_CONTENTTYPE))->value;
        if (content_type == BOXING_METADATA_CONTENT_TYPES_VISUAL)
        {
            is_analogue_data = DTRUE;
            DLOG_INFO("unboxing_load_data_from_image: data is analogue");
        }
    }

    // Sharpen image
    if (!unboxer->parameters.is_raw)
    {
        // Measure MTF, used by adaptive sharpening
        boxing_float horizontal_mtf;
        boxing_float vertical_mtf;
        int result = dunboxerv1_calculate_mtf(unboxer, image, symbols_per_pixel, tracker, user_data, &horizontal_mtf, &vertical_mtf);
        if (result != BOXING_UNBOXER_OK)
        {
            return result;
        }

        int sharpen_result;
        if (is_analogue_data)
        {
            sharpen_result = dunboxerv1_visual_sharpness_filter(user_data, unboxer, image);
        }
        else
        {
            boxing_float mix = dunboxerv1_calculate_filter_mix((tracker->x_sampling_rate + tracker->y_sampling_rate)/2.0f,
                                                                      BOXING_MATH_MAX(horizontal_mtf, vertical_mtf),
                                                                      unboxer->parameters.pre_filter.coeff);
            sharpen_result = dunboxerv1_sharpness_filter(user_data, unboxer, image, mix);
        }

        if ( sharpen_result == BOXING_FILTER_CALLBACK_ERROR )
        {
            return BOXING_UNBOXER_CONFIG_ERROR;
        }
        else if ( sharpen_result == BOXING_FILTER_CALLBACK_ABORT )
        {
            return BOXING_UNBOXER_PROCESS_CALLBACK_ABORT;
        }
    }

    // Extract content
    boxing_sampler * content_sampler = boxing_tracker_get_container_sampler(tracker, BOXING_SAMPLE_CONTAINER_CONTENT);
    if(!content_sampler || !content_sampler->state)
    {
        DLOG_ERROR( "unboxing_load_data_from_image:  container data locations not vaild");
        return BOXING_UNBOXER_DATA_DECODE_ERROR;
    }

    boxing_image8 * sampled_image = NULL;
    if ( unboxer->parameters.sample_contents )
    {
        // Hijack the content sampler
        /// \todo Move this replacement closer to where the sampler is created
        sampled_image = unboxer->parameters.sample_contents( user_data, image, &content_sampler->location_matrix, &content_sampler->state );
    }
    else
    {
        sampled_image = content_sampler->sample( content_sampler, image );
    }

    // Apply LUT on sampled image
    if (is_analogue_data)
    {
        boxing_image8_pixel lut[256];
        // Calculate LUT for mapping analogue output image
        if(dunboxerv1_calculate_lut(tracker, image, lut))
        {        
            dunboxerv1_apply_lut(sampled_image, lut);
        }
    }

#ifdef BOXINGLIB_CALLBACK
    if (unboxer->parameters.on_content_sampled)
    {
        if (unboxer->parameters.on_content_sampled(user_data, &retval, sampled_image, tracker->container_sampler_list) != BOXING_PROCESS_CALLBACK_OK)
        {
            boxing_image8_free(sampled_image);
            return BOXING_UNBOXER_PROCESS_CALLBACK_ABORT;
        }
    }

    if (retval != BOXING_UNBOXER_OK)
    {
        boxing_image8_free(sampled_image);
        return retval;
    }
#endif

    if( is_analogue_data )
    {
        retval = extract_analog_content(sampled_image, the_data_array, user_data);
    } 
    else 
    {
        retval = extract_digital_content(user_data, unboxer, sampled_image, symbols_per_pixel, the_data_array, quantize_data);
    }

    boxing_image8_free(sampled_image);
    return retval;
}

// convert data oldschool style 
static void pack_data(gvector * data)
{                    
    gvector packed_data;
    gvector_create_inplace(&packed_data, 1, (data->size + 7) / 8);
    unsigned int packed_data_counter = 0;
    char * data_end = (char *)data->buffer + data->size;
    for(char * it = (char *)data->buffer; it != data_end; )
    {
        char ch = 0;
        for(int bit = 0; (bit < 8) && (it != data_end); it++, bit++)
        {
            ch <<= 1;
            ch |= (*it) & 1;
        }
        GVECTORN8(&packed_data, packed_data_counter++) = ch;
    }
    boxing_memory_free(data->buffer);
    data->buffer = packed_data.buffer;
    data->size = packed_data.size;
}
