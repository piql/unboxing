/*****************************************************************************
**
**  Implementation of the unboxing interface
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
#include    "boxing/unboxer.h"
#include    "boxing/log.h"
#include    "boxing/graphics/genericframe.h"
#include    "boxing/unboxer/abstractframeutil.h"
#include    "unboxerv1.h"
#include    "boxing/bool.h"
#include    "boxing/utils.h"

//  SYSTEM INCLUDES
//

//---------------------------------------------------------------------------- 
/*! \mainpage Unboxing Library
 *  \brief Functions for decoding analog and digital data. 
 *
 *  The unboxing library has functions for decoding (\ref unboxing) 
 *  digital and analog data.
 *
 *  The \ref unboxer (boxing/unboxer.h) takes sampled input images, and decodes 
 *  in two steps: extract and decode. The extract step locates the frame within the image, 
 *  decodes the metadata in the bottom border of the frame, then tracks the 
 *  pixels within the frame. The tracked pixels are then quantified for 
 *  digital data. The decode step is different for analog and digital data. 
 *  Digital decode is applying the codec defined by the \ref config. Analoge
 *  decode is applying the LUT created from the frame top border calibration 
 *  bar.
 * 
 *  \section Definitions
 *
 *  \subsection sampled-image 
 *  2 dimensional digitized version of image stored on analog storage medium. Must 
 *  be sampled with higher resoultion than original resolution used when writing the image.
 *  \subsection  raw-image 
 *  Two dimensional digital image to be written on analog storage  medium. The image 
 *  represents a rendered 2D barcode image with a frame and a data container. 
 *  The resoultion of the raw image per printed pixel is from1 to 8 bits.
 *  \subsection metadata 
 *  Generic information stored in the border of the frame of a raw image. 
 *  Examples can be frame number and checksums.
 *  \subsection boxing 
 *  Coding analog and digital data into raw images.
 *  \subsection unboxing 
 *  Decoding sampled images and restoring the original content written to the raw image.
 *  \subsection boxing-format 
 *  Parameters describing the raw image geometry and the methods used for 
 *  coding the digital data into the frame.
 *  \subsection extract-step 
 *  First step in unboxing a sampled-image. The process consists of locating the 
 *  frame within the image, then decoding the metadata 
 *  in the bottom border of the frame, then tracking the pixels within 
 *  the frame. The tracked pixels are then quantified for digital data.
 *  \subsection decode-step 
 *  Second step in unboxing a sampled-image. The step is different 
 *  for analog and digital data. Digital decode is applying the codec 
 *  defined by the boxing_format. Analoge decode is applying the LUT 
 *  created from the frame top border calibration bar.
 *
 *  \section Samples Sample Applications
 *
 *  \subsection Boxer
 *  Command line application for coding digital data, see tests\boxer\main.c
 *
 *  \subsection Unboxer 
 *  Command line application for decoding digital data, see tests\unboxer\main.c
 * 
 *  \defgroup unbox Unboxing
 */


/// \cond PRIVATE_GROUPS
//---------------------------------------------------------------------------- 
/*! \defgroup codecs Codecs
 *  \brief The codecs reformat the input and output bitstreams.
 *  \ingroup unbox
 *
 *  A codec has processing functions for coding and decoding a digital 
 *  bitstream. 
 */


//---------------------------------------------------------------------------- 
/*! \defgroup frame Frame
 *  \brief Frame tracking.
 *  \ingroup unbox
 *
 *  Functions for sampling and tracking frame and content.
 */


//---------------------------------------------------------------------------- 
/*! \defgroup graphics Graphics
 *  \brief Frame graphics.
 *  \ingroup unbox
 *
 *  Functions for generating frames.
 */


//---------------------------------------------------------------------------- 
/*! \defgroup math Math
 *  \brief Math functions.
 *  \ingroup unbox
 *
 *  Math functions.
 */

//---------------------------------------------------------------------------- 
/*! \defgroup platform Platform
 *  \brief Platform dependent functions and structures.
 *  \ingroup unbox
 *
 *  The platform is defined as the computing environment responsible for 
 *  executing the unboxing software and providing the resources the 
 *  unboxing software needs. This includes reading the frames from the film
 *  reel and converting them into 8bit 2D images, and software library 
 *  functions like dynamic memory allocation, math, floating point and other 
 *  support functions. All platform dependant function used directly by 
 *  the unboxing library is part of the C99 version of the C programming
 *  language. All the C language routines used are documented here. The 
 *  C99 function documentation are from the GNU Linux man pages 
 *  project (C).
 */
/// \endcond PRIVATE_GROUPS


//---------------------------------------------------------------------------- 
/*! \defgroup unboxer Unboxer
 *  \brief Unbox sampled frames.
 *  \ingroup unbox
 *
 *  The unboxer is responsible for decoding digital and analoge data from 
 *  sampled frames. This provides the top level API for decoding frames.
 */


/*! 
  * \addtogroup unboxer
  * \{
  */


//----------------------------------------------------------------------------
/*!
 *  \enum   boxing_unboxer_result unboxer.h
 *  \brief  Unboxing result.
 *  
 *  \param BOXING_UNBOXER_OK                      (0) Unboxing OK.
 *  \param BOXING_UNBOXER_METADATA_ERROR          (1) Metadata error.
 *  \param BOXING_UNBOXER_BORDER_TRACKING_ERROR   (2) Border tracking error.
 *  \param BOXING_UNBOXER_DATA_DECODE_ERROR       (3) Data decode error.
 *  \param BOXING_UNBOXER_CRC_MISMATCH_ERROR      (4) CRC mismatch error.
 *  \param BOXING_UNBOXER_CONFIG_ERROR            (5) Configuration error.
 *  \param BOXING_UNBOXER_PROCESS_CALLBACK_ABORT  (6) Process callback abort.
 *  \param BOXING_UNBOXER_INPUT_DATA_ERROR        (7) Input data error.
 *
 *  Unboxer result codes.
 */


//----------------------------------------------------------------------------
/*!
 *  \enum   boxing_process_callback_result unboxer.h
 *  \brief  Processing callback result.
 *  
 *  \param BOXING_PROCESS_CALLBACK_OK     (0) Process callback OK.
 *  \param BOXING_PROCESS_CALLBACK_ABORT  (1) Process callback abort.
 *
 *  A callback should return BOXING_PROCESS_CALLBACK_ABORT if the unbox process
 *  should be aborted or BOXING_PROCESS_CALLBACK_OK is the unbox process 
 *  should continue.
 */


//----------------------------------------------------------------------------
/*! 
 *  \typedef _GHashTable GHashTable
 *  \brief Hash table
 *   
 *  A hash table provides fast key lookup for the given value.
 *
 *  For details, see the GLIB library documentation.
 *  https://developer.gnome.org/glib/2.54/glib-Hash-Tables.html
 */

//----------------------------------------------------------------------------
/*! 
 *  \struct boxing_unboxer_s unboxer.h
 *  \brief Boxer instance
 *   
 *  Handle to boxer instance.
 */

/*! 
 *  \typedef boxing_image8 * (*boxing_sample_cb)(void* user, const boxing_image8 * frame, const boxing_matrixf * location_matrix, DBOOL * state)
 *  \brief Boxing sample callback function.
 *
 *  \param[in,out]  user             User data.
 *  \param[in]      frame            Frame.
 *  \param[in]      location_matrix  Location matrix.
 *  \param[in]      state            State.
 *   
 *  Called when image sampling is complete..
 */


/*! 
 *  \typedef gvector * (*boxing_quantize_cb)(void * user, const boxing_image8 * image, int block_width, int block_height, int bins)
 *  \brief Boxing quantize callback function.
 *
 *  \param[in,out]  user          User data.
 *  \param[in]      image         Image.
 *  \param[in]      block_width   Block width.
 *  \param[in]      block_height  Block height.
 *  \param[in]      bins          Bins.
 *
 *  Called before quantizing starts.   
 */


/*! 
 *  \typedef int (*boxing_tracker_created_cb)(void * user, int * res, boxing_tracker * tracker)
 *  \brief Boxing tracker created callback function.
 *
 *  \param[in,out]  user     User data.
 *  \param[in]      res      Result.
 *  \param[in]      tracker  Tracker.
 *   
 *  Called after the frame tracker object is created. Allows for implementation
 *  spesific modifications to the tracker.
 */


/*! 
 *  \typedef int (*boxing_reference_bar_complete_cb)(void * user, int * res, boxing_stats_mtf * stats)
 *  \brief Boxing reference bar complete callback function.
 *
 *  \param[in,out]  user   User data.
 *  \param[in]      res    Result.
 *  \param[in]      stats  MTF statistics.
 *   
 *  Called when reference bar tracking is complete.
 */


/*! 
 *  \typedef int (*boxing_metadata_complete_cb)(void * user, int * res, boxing_metadata_list * meta_data)
 *  \brief Boxing metadata complete callback function.
 *
 *  \param[in,out]  user       User data.
 *  \param[in]      res        Result.
 *  \param[in]      meta_data  Decoded metadata items.
 *   
 *  Called when metadata decoding is complete.
 */


/*! 
 *  \typedef int (*boxing_content_sampled_cb)(void * user, int * res, boxing_image8 * image, void * sampler_list)
 *  \brief Boxing content sampled callback function.
 *
 *  \param[in,out]  user          User data.
 *  \param[in]      res           Result.
 *  \param[in]      image         Image.
 *  \param[in]      sampler_list  Sampler list.
 *   
 *  Called when image content is sampled (location of each content pixel is found).
 */


/*! 
 *  \typedef int (*boxing_content_quantized_cb)(void * user, int * res, char * data, int size)
 *  \brief Boxing content quantized callback function.
 *
 *  \param[in,out]  user  User data.
 *  \param[in]      res   Result.
 *  \param[in]      data  Data.
 *  \param[in]      size  Size.
 *   
 *
 *  Called when iamge quantize (conversion from sampled pixels to symbols) is complete.
 */


/*! 
 *  \typedef int (*boxing_training_complete_cb)(void * user, int * res, void * training_result)
 *  \brief Boxing training complete callback function.
 *
 *  \param[in,out]  user             User data.
 *  \param[in]      res              Result.
 *  \param[in]      training_result  Training result.
 *   
 *  Called when boxing training is complete.
 */


/*! 
 *  \typedef int (*boxing_decode_step_cb)(void * user, int * res, void * data, int data_size, int step, int is_codec_errorcorrecting, int parity_size, int block_size)
 *  \brief Boxing decode step callback function.
 *
 *  \param[in,out]  user                      User data.
 *  \param[in]      res                       Result.
 *  \param[in]      data                      Data.
 *  \param[in]      data_size                 Data size.
 *  \param[in]      step                      Current step.
 *  \param[in]      is_codec_errorcorrecting  Codec error correcting sign.
 *  \param[in]      parity_size               Parity size.
 *  \param[in]      block_size                Block size.
 *   
 *  Codec decode step complete callback.
 */


/*! 
 *  \typedef int (*boxing_all_complete_cb)(void * user, int* res, boxing_stats_decode * stats)
 *  \brief Unoxing complete callback function.
 *
 *  \param[in,out]  user   User data.
 *  \param[in]      res    Result.
 *  \param[in]      stats  Decode statistic.
 *   
 *  Last callback from unboxing.
 */


/*! 
 *  \struct gvector
 *  \brief Dynamic array.
 *   
 *  Arrays are similar to standard C arrays, except that they grow automatically 
 *  as elements are added.
 */


//---------------------------------------------------------------------------- 
/*! 
 *  \struct  boxing_unboxer_parameters_s unboxer.h
 *  \brief   Unboxer configuration.
 * 
 *  \param is_raw                     True if input data is in RAW format.
 *  \param codec_cb                   Codec callback function.
 *  \param format                     Boxing format, defines frame geometry and codecs.
 *  \param training_mode              True if unboxer should be run in training mode.
 *  \param training_mode_reference    Original RAW frame.
 *  \param training_result            Output from training mode.
 *  \param pre_filter                 5x5 sharpness filter that will be executed on 
 *                                    images to be unboxed. Default filter function is used 
 *                                    if set to NULL. Provide a direct assignemement function 
 *                                    (i.e. destination = source) if you want to skip 
 *                                    filtering.
 *  \param decoding_filters           Codec decoding functions.
 *  \param sample_contents            Boxing sample function.
 *  \param quantize_contents          Boxing quantize function.
 *  \param on_tracker_created         Boxing tracker created callback function.
 *  \param on_content_sampled         Boxing content sampled callback function.
 *  \param on_content_quantized       Boxing content quantized callback function.
 *  \param on_metadata_complete       Boxing metadata complete callback function.
 *  \param on_reference_bar_complete  Boxing reference bar complete callback function.
 *  \param on_corner_mark_complete    Boxing corner mark complete callback function.
 *  \param on_training_complete       Boxing training complete callback function.
 *  \param on_decode_step             Boxing decode step callback function.
 *  \param on_all_complete            Boxing all complete callback function.
 *  \param orig_image                 Original image.
 *
 *  Configure unboxer. Note that the callback interface is only avaliable if  
 *  BOXINGLIB_CALLBACK is defined.
 *
 *  Note 1: Training mode is an experimental feature for improving the unboxer
 *          by unboxing a known frame and using it's characteristics when decoding
 *          the rest of the frames. Could be removed in future versions in the
 *          library.
 *  Note 2: Pre-filtering of the image is really the responsibility of the reading
 *          device. This function will be removed in future versions of the library.
 *
 */ 


//----------------------------------------------------------------------------
/*!
 *  \struct  boxing_unboxing_codec_info_s  unboxer.h 
 *  \brief   Codec info.
 *
 *  \param name       Name of the codec.
 *  \param reentrant  Is the codec reentrant.
 *
 *  Codec information.
 */

// PUBLIC UNBOXER FUNCTIONS
//

//---------------------------------------------------------------------------- 
/*! \brief Create unboxer
 * 
 *  Create unboxer with given parameters.
 *
 *  \param parameters  Unboxing configuration.
 *  \return Unboxer instance or NULL on error.
 */

boxing_unboxer * boxing_unboxer_create(boxing_unboxer_parameters * parameters)
{
    if (!parameters)
    {
        return NULL;
    }

    boxing_dunboxerv1 * unboxer = boxing_dunboxerv1_create();
    if (!unboxer)
    {
        return NULL;
    }

    // Free dynamically allocated memory before copy parameters
    boxing_unboxer_parameters_free(&unboxer->parameters);
    
    unboxer->parameters = *parameters;

    if (parameters->training_result)
    {
        boxing_abstract_frame_util * frame_util = unboxer->frame_util;
        if (frame_util)
        {
            frame_util->deserialize(frame_util, (char *)parameters->training_result);
        }
        else
        {
            DLOG_WARNING( "(boxing_unboxer_create) No frame util set: Loading training result file aborted" );
        }
    }

    if (boxing_dunboxerv1_setup_config(unboxer) != BOXING_UNBOXER_OK)
    {
        boxing_dunboxerv1_destroy(unboxer);
        return NULL;
    }

    unboxer->metadata_codec = boxing_codecdispatcher_create(BOXING_VIRTUAL2(unboxer->frame, metadata_container, capasity),
                                        BOXING_CODEC_MODULATION_PAM2, unboxer->parameters.format, "MetadataCodingScheme");
    boxing_codecdispatcher_callback_setup(unboxer->metadata_codec, unboxer->parameters.codec_cb);

    int levels_per_symbol = unboxer->frame->levels_per_symbol(unboxer->frame);

    unboxer->codec = boxing_codecdispatcher_create(BOXING_VIRTUAL2(unboxer->frame, container, capasity), levels_per_symbol, unboxer->parameters.format, "DataCodingScheme");
    boxing_codecdispatcher_callback_setup(unboxer->codec, unboxer->parameters.codec_cb);
    boxing_codecdispatcher_reset(unboxer->codec);
    return (boxing_unboxer *)unboxer;
}


//---------------------------------------------------------------------------- 
/*! \brief Free unboxer data
 * 
 *  Free data owned by unboxer structure, but not structure itself.
 *
 *  \param unboxer Unboxer instance.
 */

void boxing_unboxer_free(boxing_unboxer * unboxer)
{
    boxing_dunboxerv1 * ub = (boxing_dunboxerv1 *)unboxer;
    if (ub)
    {
        boxing_codecdispatcher_free(ub->codec);
        boxing_codecdispatcher_free(ub->metadata_codec);
        boxing_dunboxerv1_destroy(ub);
    }
}


//---------------------------------------------------------------------------- 
/*! \brief Enable/disable RAW input images.
 * 
 *  Set if input images should be interpreted as RAW images.
 *
 *  \param[in]  unboxer      Unboxer instance.
 *  \param[in]  is_raw       false if 0, else true
 */

void boxing_unboxer_set_raw_input(boxing_unboxer * unboxer, int is_raw)
{
    ((boxing_dunboxerv1 *)unboxer)->parameters.is_raw = (is_raw == 0 ? 0 : 1);
}


//---------------------------------------------------------------------------- 
/*! \brief Get RAW mode.
 * 
 *  Return is_raw parameter.
 *
 *  \param[in]  unboxer      Unboxer instance.
 *  \return is_raw value.
 */

int boxing_unboxer_is_raw_input(boxing_unboxer * unboxer)
{
    return ((boxing_dunboxerv1 *)unboxer)->parameters.is_raw;
}


//----------------------------------------------------------------------------
/*!
 *  \brief   Get the codec information.
 *
 *  Get the information of the current codec from the unboxer instance.
 *
 *  \param[in]      unboxer  Unboxer instance.
 *  \param[in]      step     Step value.
 *  \param[out]     info     Codec information.
 */

void boxing_unboxer_codec_info(const boxing_unboxer * unboxer, int step, boxing_codec_info *info)
{
    boxing_dunboxerv1 * ub = (boxing_dunboxerv1 *)unboxer;
    if(ub && (step > -1) && (step < (int)ub->codec->decode_codecs.size))
    {
        boxing_codec * codec = GVECTORN(&ub->codec->decode_codecs, boxing_codec *, step);
        info->name      = codec->name;
        info->reentrant = codec->reentrant;
    }
}


//----------------------------------------------------------------------------
/*!
 *  \brief   Get the number of decoding steps.
 *
 *  Get the number of unboxer decoding steps.
 *
 *  \param[in]  unboxer  Unboxer instance.
 *  \return     number of decoding steps or 0 on error.
 */

size_t boxing_unboxer_decoding_steps(const boxing_unboxer * unboxer)
{
    if (!unboxer)
    {
        return 0;
    }

    boxing_dunboxerv1 * ub = (boxing_dunboxerv1 *)unboxer;
    return ub->codec->decode_codecs.size;
}


//----------------------------------------------------------------------------
/*!
 *  \brief   Reset the unboxer.
 *
 *  Reset unboxer codec and metadata codec.
 *
 *  \param[in]  unboxer  Unboxer instance.
 */

void boxing_unboxer_reset(const boxing_unboxer * unboxer)
{
    if (unboxer)
    {
        boxing_dunboxerv1 * ub = (boxing_dunboxerv1 *)unboxer;
        boxing_codecdispatcher_reset(ub->codec);
        boxing_codecdispatcher_reset(ub->metadata_codec);
    }
}


//----------------------------------------------------------------------------
/*!
 *  \brief   Extract data container.
 *
 *  Unbox metadata and extract a raw quantized copy of the data container.
 *
 *  \param[out] data      Quantized data container.
 *  \param[out] metadata  Decoded metadata.
 *  \param[in]  image     Image to be decoded.
 *  \param[in]  unboxer   Unboxer structure.
 *  \param[in]  user_data User data.
 *  \param[in] fallback_metadata_content_type Fallback metadata content type if metadata decoding fails
 *  \return Unboxing status code
 */

enum boxing_unboxer_result boxing_unboxer_unbox_extract_container(
    gvector * data,
    boxing_metadata_list * metadata,
    boxing_image8 * image, 
    boxing_unboxer * unboxer,
    void *user_data,
    boxing_metadata_content_types fallback_metadata_content_type)
{
    return (enum boxing_unboxer_result)boxing_dunboxerv1_extract_container((boxing_dunboxerv1 *)unboxer, data, metadata, image, user_data, fallback_metadata_content_type);
}


//----------------------------------------------------------------------------
/*!
 *  \brief   Decode data.
 *
 *  Unbox metadata and data from quantized data container.
 *
 *  \param[in]  unboxer       Unboxer structure
 *  \param[in,out] data       In: Quantized data container. Out: Decoded data.
 *  \param[in] metadata       Decoded metadata
 *  \param[out] decode_stats  Statistic of the decode process
 *  \param[in] step           Decoding step
 *  \param[in] user_data      User data.
 *  \return Unboxing status code
 */

int boxing_unboxer_decode(boxing_unboxer * unboxer, gvector * data, boxing_metadata_list * metadata,  boxing_stats_decode * decode_stats, unsigned int step, void *user_data)
{
    return boxing_dunboxerv1_decode((boxing_dunboxerv1 *)unboxer, data, metadata, decode_stats, step, user_data);
}


//---------------------------------------------------------------------------- 
/*! \brief Decode image
 * 
 *  Decode image and return data and metadata on success.
 *
 *  \param[out] data            Decoded data.
 *  \param[out] metadata        Decoded metadata.
 *  \param[in]  image           Input image.
 *  \param[in]  unboxer         Unboxer structure.
 *  \param[out] extract_result  Result from data extraction phase of unboxing.
 *  \param[in]  user_data       User data.
 *  \param[in] fallback_metadata_content_type Fallback metadata content type if metadata decoding fails
 *  \return     Unboxing status code.
 */

enum boxing_unboxer_result boxing_unboxer_unbox(gvector * data, boxing_metadata_list * metadata, boxing_image8 * image, boxing_unboxer * unboxer, int * extract_result, void *user_data, boxing_metadata_content_types fallback_metadata_content_type)
{
    return (enum boxing_unboxer_result)boxing_dunboxerv1_process((boxing_dunboxerv1 *)unboxer, data, metadata, image, extract_result, user_data, fallback_metadata_content_type);
}


//---------------------------------------------------------------------------- 
/*! \brief Dispatcher function.
 * 
 *  Get the unboxer dispatcher. The dispatcher is responsible for calling the
 *  codecs in the codec chain.
 *
 *  \param[in] unboxer        Unboxer.
 *  \param[in] coding_scheme  Coding scheme.
 *  \return  boxing_codecdispatcher instance.
 */

boxing_codecdispatcher *  boxing_unboxer_dispatcher(boxing_unboxer * unboxer, const char * coding_scheme)
{
    return boxing_dunboxerv1_dispatcher(unboxer, coding_scheme);
}


//---------------------------------------------------------------------------- 
/*! \brief Initialize unboxer parameters
 * 
 *  Initialize unboxer parameters to default values.
 *
 *  \param[in]  parameters      Parameters to be initialized.
 */

void boxing_unboxer_parameters_init(boxing_unboxer_parameters * parameters)
{
#ifdef BOXINGLIB_CALLBACK
    parameters->on_tracker_created = NULL;
    parameters->on_content_sampled = NULL;
    parameters->on_content_quantized = NULL;
    parameters->on_metadata_complete = NULL;
    parameters->on_reference_bar_complete = NULL;
    parameters->on_corner_mark_complete = NULL;
    parameters->on_training_complete = NULL;
    parameters->on_decode_step = NULL;
    parameters->on_all_complete = NULL;
    parameters->orig_image = NULL;
#endif
    parameters->is_raw = DFALSE;
    parameters->codec_cb = NULL;
    parameters->format = NULL;
    parameters->training_mode = DFALSE;
    parameters->training_mode_reference = NULL;
    parameters->training_result = NULL;
    boxing_filter_init( &parameters->pre_filter );
    parameters->sample_contents = NULL;
    parameters->quantize_contents = NULL;
}


//---------------------------------------------------------------------------- 
/*! \brief Free unboxing parameters data.
 * 
 *  Free data owned by the parameters, but not the parameter instance itself.
 *
 *  \param[in]  parameters      Parameters to be freed.
 */

void boxing_unboxer_parameters_free(boxing_unboxer_parameters * parameters)
{
    boxing_filter_free( &parameters->pre_filter );
}


//----------------------------------------------------------------------------
/*!
  * \} end of unboxer group
  */
