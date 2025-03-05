#ifndef BOXING_UNBOXER_H
#define BOXING_UNBOXER_H

/*****************************************************************************
**
**  Definition of unboxer C interface
**
**  Creation date:  2015/09/02
**  Created by:     Morgun Dmytro
**
**
**  Copyright (c) 2015 Piql AS. All rights reserved.
**
**  This file is part of the boxing library
**
*****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

//  PROJECT INCLUDES
//
#include "boxing/image8.h"
#include "boxing/metadata.h"
#include "boxing/filter.h"
#include "boxing/stats.h"
#include "boxing/unboxer/cornermark.h"
#include "boxing/codecs/codec_cb.h"
#include "boxing/codecs/codecdispatcher.h"
#include "boxing/frame/tracker.h"
#include "gvector.h"
    
//============================================================================
//   unboxer C interface

#define BOXINGLIB_CALLBACK

enum boxing_unboxer_result
{
    BOXING_UNBOXER_OK = 0,
    BOXING_UNBOXER_METADATA_ERROR,
    BOXING_UNBOXER_BORDER_TRACKING_ERROR,
    BOXING_UNBOXER_DATA_DECODE_ERROR,
    BOXING_UNBOXER_CRC_MISMATCH_ERROR,
    BOXING_UNBOXER_CONFIG_ERROR,
    BOXING_UNBOXER_PROCESS_CALLBACK_ABORT,
    BOXING_UNBOXER_INPUT_DATA_ERROR,
    BOXING_UNBOXER_SPLICING,
};

struct boxing_config_s;

typedef boxing_image8* (*boxing_sample_cb)(void* user, const boxing_image8 * frame, const boxing_matrixf* location_matrix, DBOOL* state);
typedef gvector*       (*boxing_quantize_cb)(void* user, const boxing_image8 * image, int block_width, int block_height, int bins);

#ifdef BOXINGLIB_CALLBACK

enum boxing_process_callback_result
{
    BOXING_PROCESS_CALLBACK_OK = 0,
    BOXING_PROCESS_CALLBACK_ABORT,
};

typedef int (*boxing_tracker_created_cb)(void * user, int* res, boxing_tracker * tracker);
typedef int (*boxing_reference_bar_complete_cb)(void * user, int* res, boxing_stats_mtf * stats);
typedef int (*boxing_metadata_complete_cb)(void * user, int* res, boxing_metadata_list * meta_data);
typedef int (*boxing_content_sampled_cb)(void * user, int* res, boxing_image8 * image, void * sampler_list);
typedef int (*boxing_content_quantized_cb)(void * user, int* res, char* data, int size);
typedef int (*boxing_training_complete_cb)(void * user, int* res, void * training_result);
typedef int (*boxing_decode_step_cb)(void * user, int* res, void * data, int data_size, int step,
                int is_codec_errorcorrecting, int parity_size, int block_size);
typedef int (*boxing_all_complete_cb)(void * user, int* res, boxing_stats_decode * stats);
#endif // BOXINGLIB_CALLBACK

typedef struct boxing_unboxer_parameters_s
{
    int                                 is_raw; 
    boxing_codec_cb *                   codec_cb; // 0 terminated array
    const struct boxing_config_s *      format; 
    int                                 training_mode;
    boxing_image8 *                     training_mode_reference;
    void *                              training_result;
    boxing_filter                       pre_filter;
    boxing_sample_cb                    sample_contents;
    boxing_quantize_cb                  quantize_contents;
#ifdef BOXINGLIB_CALLBACK
    boxing_tracker_created_cb           on_tracker_created;
    boxing_content_sampled_cb           on_content_sampled;
    boxing_content_quantized_cb         on_content_quantized;
    boxing_metadata_complete_cb         on_metadata_complete;
    boxing_reference_bar_complete_cb    on_reference_bar_complete;
    boxing_corner_mark_complete_cb      on_corner_mark_complete;
    boxing_training_complete_cb         on_training_complete;
    boxing_decode_step_cb               on_decode_step;
    boxing_all_complete_cb              on_all_complete;
    boxing_image8 *                     orig_image;
#endif
} boxing_unboxer_parameters;

typedef struct boxing_unboxer_s boxing_unboxer;

typedef struct boxing_unboxing_codec_info_s
{
    const char * name;
    int          reentrant;
} boxing_codec_info;

boxing_unboxer *   boxing_unboxer_create(boxing_unboxer_parameters * parameters);
void               boxing_unboxer_free(boxing_unboxer * unboxer);
void               boxing_unboxer_set_raw_input(boxing_unboxer * unboxer, int is_raw);
int                boxing_unboxer_is_raw_input(boxing_unboxer * unboxer);
void               boxing_unboxer_codec_info(const boxing_unboxer * unboxer, int step, boxing_codec_info *info);
size_t             boxing_unboxer_decoding_steps(const boxing_unboxer * unboxer);
void               boxing_unboxer_reset(const boxing_unboxer * unboxer);
enum boxing_unboxer_result boxing_unboxer_unbox_extract_container(
                    gvector * data,
                    boxing_metadata_list * metadata,
                    boxing_image8 * image, 
                    boxing_unboxer * unboxer,
                    void * user_data,
                    boxing_metadata_content_types fallback_metadata_content_type);
int                boxing_unboxer_decode(
                    boxing_unboxer * unboxer, 
                    gvector * data, 
                    boxing_metadata_list * metadata,  
                    boxing_stats_decode * decode_stats, 
                    unsigned int step,
                    void * user_data);
enum boxing_unboxer_result boxing_unboxer_unbox(
                    gvector * data,
                    boxing_metadata_list * metadata,
                    boxing_image8 * image, 
                    boxing_unboxer * unboxer,
                    int * extract_result, 
                    void *user_data,
                    boxing_metadata_content_types fallback_metadata_content_type);
boxing_codecdispatcher *  boxing_unboxer_dispatcher(boxing_unboxer * unboxer, const char * coding_scheme);
void                boxing_unboxer_parameters_init(boxing_unboxer_parameters * parameters);
void                boxing_unboxer_parameters_free(boxing_unboxer_parameters * parameters);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
