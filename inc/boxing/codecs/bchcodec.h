#ifndef BOXING_BCHCODEC_H
#define BOXING_BCHCODEC_H

/*****************************************************************************
**
**  Definition of BCH codec functions
**
**  Creation date:  2014/12/16
**  Created by:     Piql AS
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

#include "boxing/codecs/codecbase.h"
#include "boxing/platform/types.h"


static const char codec_bch_name[] = "BCH";
struct bch_control;
    
typedef struct boxing_codec_bch_s
{
    boxing_codec         base;
    struct bch_control * bch;
    unsigned int         message_size;
    unsigned int         parity_size;
    unsigned int         m;
    unsigned int         t;
    unsigned int         poly;

} boxing_codec_bch;

boxing_codec * boxing_codec_bch_create(GHashTable * properties, const boxing_config * config);
void           boxing_codec_bch_free(boxing_codec *codec);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
