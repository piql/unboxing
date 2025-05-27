#ifndef BOXING_UNBOXERV1_H
#define BOXING_UNBOXERV1_H

/*****************************************************************************
**
**  Definition of the unboxer interface class
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

#ifdef __cplusplus
extern "C" {
#endif

//  PROJECT INCLUDES
//
#include "boxing/unboxer.h"
#include "boxing/metadata.h"
#include "boxing/codecs/codecdispatcher.h"
#include "gvector.h"


struct boxing_abstract_frame_util_s;
struct boxing_frame_s;
struct boxing_tracker_s;

//============================================================================
//  STRUCT:  boxing_dunboxerv1

typedef struct boxing_dunboxerv1_s
{
    boxing_unboxer_parameters             parameters;
    struct boxing_frame_s *               frame;
    boxing_image8 *                       error_image;
    struct boxing_abstract_frame_util_s * frame_util;
    boxing_codecdispatcher *              codec;
    boxing_codecdispatcher *              metadata_codec;
    int                                   preload_frames;
    DBOOL                                 quantize_data_on_load;
} boxing_dunboxerv1;

boxing_dunboxerv1 *     boxing_dunboxerv1_create(void);
void                    boxing_dunboxerv1_destroy(boxing_dunboxerv1 * unboxer);
int                     boxing_dunboxerv1_setup_config(boxing_dunboxerv1 * unboxer);
int                     boxing_dunboxerv1_process(boxing_dunboxerv1 * unboxer, gvector * data,
                                                  boxing_metadata_list * metadata_list, boxing_image8 * image,
												  int * extract_result, void *user_data, boxing_metadata_content_types fallback_metadata_content_type);
int                     boxing_dunboxerv1_extract_container(boxing_dunboxerv1 * unboxer, gvector * data,
                            boxing_metadata_list * metadata_list, boxing_image8 * image, void *user_data, boxing_metadata_content_types fallback_metadata_content_type);
int                     boxing_dunboxerv1_decode(boxing_dunboxerv1 * unboxer, gvector * data, 
                            boxing_metadata_list * metadata, boxing_stats_decode * decode_stats, unsigned int step, void *user_data);
boxing_codecdispatcher *  boxing_dunboxerv1_dispatcher(const boxing_unboxer * unboxer, const char * coding_scheme);

//============================================================================

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
