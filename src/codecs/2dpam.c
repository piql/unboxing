/*****************************************************************************
**
**  Implementation of the modulator codec interface
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
#include "boxing/codecs/2dpam.h"
#include "boxing/log.h"
#include "boxing/platform/memory.h"
#include "boxing/image8.h"
#include "horizontalmeasures.h"

// SYSTEM INCLUDES
#include <stddef.h> /* ptrdiff_t */

//  DEFINES
//

#define CODEC_MEMBER(name) (((boxing_codec_modulator *)codec)->name)
#define CODEC_BASE_MEMBER(name) (((boxing_codec_modulator *)codec)->base.name)

//  CONSTANTS
//

typedef unsigned char pam_symboli;
typedef unsigned char pam_symbolf;

typedef struct pam2d_symboli_s
{
    pam_symboli s0;
    pam_symboli s1;
}pam2d_symboli;

char bit_map[5][2][6][6] = {
    {
        {
            { 1, 1, 1, 1, 1, 1 },
            { 1, 1, 1, 1, 1, 1 },
            { 1, 1, 1, 1, 1, 1 },
            { 0, 0, 0, 0, 0, 0 },
            { 0, 0, 0, 0, 0, 0 },
            { 0, 0, 0, 0, 0, 0 },
        },
        {
            { 0, 0, 0, 0, 0, 0 },
            { 0, 0, 0, 0, 0, 0 },
            { 0, 0, 0, 0, 0, 0 },
            { 1, 1, 1, 1, 1, 1 },
            { 1, 1, 1, 1, 1, 1 },
            { 1, 1, 1, 1, 1, 1 },
        }
    },
    {
        {
            { 1, 1, 1, 0, 0, 0 },
            { 1, 1, 1, 0, 0, 0 },
            { 1, 1, 1, 0, 0, 0 },
            { 1, 1, 1, 0, 0, 0 },
            { 1, 1, 1, 0, 0, 0 },
            { 1, 1, 1, 0, 0, 0 },
        },
        {
            { 0, 0, 0, 1, 1, 1 },
            { 0, 0, 0, 1, 1, 1 },
            { 0, 0, 0, 1, 1, 1 },
            { 0, 0, 0, 1, 1, 1 },
            { 0, 0, 0, 1, 1, 1 },
            { 0, 0, 0, 1, 1, 1 },
        }
    },
    {
        {
            { 1, 1, 1, 1, 1, 1 },
            { 0, 0, 1, 1, 0, 0 },
            { 0, 0, 0, 0, 0, 0 },
            { 0, 0, 0, 0, 0, 0 },
            { 0, 0, 1, 1, 0, 0 },
            { 1, 1, 1, 1, 1, 1 },
        },
        {
            { 0, 0, 0, 0, 0, 0 },
            { 1, 0, 0, 0, 0, 1 },
            { 1, 1, 1, 1, 1, 1 },
            { 1, 1, 1, 1, 1, 1 },
            { 1, 0, 0, 0, 0, 1 },
            { 0, 0, 0, 0, 0, 0 },
        }
    },
    {
        {
            { 1, 1, 0, 0, 1, 1 },
            { 1, 0, 0, 0, 0, 1 },
            { 1, 0, 0, 0, 0, 1 },
            { 1, 0, 0, 0, 0, 1 },
            { 1, 0, 0, 0, 0, 1 },
            { 1, 1, 0, 0, 1, 1 },
        },
        {
            { 0, 0, 1, 1, 0, 0 },
            { 0, 0, 1, 1, 0, 0 },
            { 0, 1, 1, 1, 1, 0 },
            { 0, 1, 1, 1, 1, 0 },
            { 0, 0, 1, 1, 0, 0 },
            { 0, 0, 1, 1, 0, 0 },
        }
    },
    {
        {
            { 1, 0, 0, 0, 0, 1 },
            { 1, 0, 1, 1, 0, 1 },
            { 0, 0, 1, 1, 0, 0 },
            { 0, 0, 1, 1, 0, 0 },
            { 1, 0, 1, 1, 0, 1 },
            { 1, 0, 0, 0, 0, 1 },
        },
        {
            { 0, 1, 1, 1, 1, 0 },
            { 0, 0, 0, 0, 0, 0 },
            { 1, 1, 0, 0, 1, 1 },
            { 1, 1, 0, 0, 1, 1 },
            { 0, 0, 0, 0, 0, 0 },
            { 0, 1, 1, 1, 1, 0 },
        }
    }
};


/*  32 2D-PAM mapping
 * 
 *   s1 (y)
 *   ^
 *   |
 * 5 +    10000  10001  10011  11011  11001  11000
 *   |
 * 4 +    10100         10010  11010         11100
 *   |
 * 3 +    10101  10111  10110  11110  11111  11101
 *   |
 * 2 +    00101  00111  00110  01110  01111  01101
 *   |                                            
 * 1 +    00100         00010  01010         01100
 *   |                                            
 * 0 +    00000  00001  00011  01011  01001  01000
 *   |                                            
 *    ------|------|------|------|------|------|-----> s0 (x)
 *          0      1      2      3      4      5    
 */

static const pam2d_symboli map_32_2dPam[32] =
{
    { 0, 0 }, { 1, 0 }, { 2, 1 }, { 2, 0 }, { 0, 1 }, { 0, 2 }, { 2, 2 }, { 1, 2 },

    { 5, 0 }, { 4, 0 }, { 3, 1 }, { 3, 0 }, { 5, 1 }, { 5, 2 }, { 3, 2 }, { 4, 2 },

    { 0, 5 }, { 1, 5 }, { 2, 4 }, { 2, 5 }, { 0, 4 }, { 0, 3 }, { 2, 3 }, { 1, 3 },

    { 5, 5 }, { 4, 5 }, { 3, 4 }, { 3, 5 }, { 5, 4 }, { 5, 3 }, { 3, 3 }, { 4, 3 }
};


//  PRIVATE INTERFACE
//

static DBOOL codec_decode(void * codec, gvector * data, gvector * erasures, boxing_stats_decode * stats, void* user_data);
static DBOOL codec_encode(void * codec, gvector * data);
static boxing_codec_syncpointinserter * create_syncpointinserter(GHashTable * properties, const boxing_config * config);


/*! 
  * \addtogroup codecs
  * \{
  */


// PUBLIC MODULATOR FUNCTIONS
//

//----------------------------------------------------------------------------
/*!
 *  \struct     boxing_codec_2dpam_s  2dpam.h
 *  \brief      Codec 2dpam data storage.
 *
 *  \param base                       Pointer to the boxing_codec structure.
 *  \param num_data_pixels_per_frame  Number of data pixels per frame.
 *  \param syncpointinserter          Pointer to the boxing_codec_syncpointinserter structure.
 *
 *  Structure for storing codec 2dpam data.
 */


//----------------------------------------------------------------------------
/*!
 *  \brief Create an boxing_codec_2dpam instance.
 *
 *  Allocate memory for the boxing_codec_2dpam type
 *  and initializes all structure data according to the input values.
 *  Return instance of allocated structure.
 *
 *  \param[in]  properties  Hash table with codec properties.
 *  \param[in]  config      Pointer to the boxing_config structure.
 *  \return instance of allocated boxing_codec_2dpam structure reduced to boxing_codec structure.
 */

boxing_codec * boxing_codec_2dpam_create(GHashTable * properties, const boxing_config * config)
{
    BOXING_UNUSED_PARAMETER( config );
    boxing_codec_2dpam * codec = BOXING_MEMORY_ALLOCATE_TYPE(boxing_codec_2dpam);
    boxing_codec_init_base((boxing_codec *)codec);

    codec->syncpointinserter = create_syncpointinserter(properties, config);
    codec->syncpointinserter->property_value_sync_point_foreground_m = 5;

    codec->base.free = boxing_codec_2dpam_free;
    codec->base.is_error_correcting = DFALSE;
    codec->base.name = codec_2dpam_name;
    codec->base.decode = codec_decode;
    codec->base.encode = codec_encode;

    codec->base.encoded_symbol_size = codec->syncpointinserter->base.encoded_symbol_size;
    codec->base.decoded_symbol_size = 1;

    codec->base.encoded_block_size = codec->syncpointinserter->base.encoded_block_size;
    codec->base.decoded_block_size = codec->syncpointinserter->base.decoded_block_size / 2 * 5;

    return (boxing_codec *)codec;
}


//----------------------------------------------------------------------------
/*!
 *  \brief Frees occupied memory of boxing_codec_2dpam structure.
 *
 *  Frees occupied memory of all internal structure pointers and structure pointer.
 *
 *  \param[in]  codec  Pointer to the boxing_codec structure.
 */

void boxing_codec_2dpam_free(boxing_codec * codec)
{
    boxing_codec_syncpointinserter_free((boxing_codec *)(((boxing_codec_2dpam *)codec)->syncpointinserter));
    boxing_codec_release_base(codec);
    boxing_memory_free(codec);
}


//----------------------------------------------------------------------------
/*!
  * \} end of codecs group
  */


// PRIVATE MODULATOR FUNCTIONS
//

static boxing_codec_syncpointinserter * create_syncpointinserter(GHashTable * properties, const boxing_config * config)
{
    const char * value;
    value = boxing_config_property(config, "FrameFormat", "syncPointHDistance");
    if (value)
        g_hash_table_replace(properties, boxing_string_clone("SyncPointHDistancePixel"), g_variant_create_string(value));

    value = boxing_config_property(config, "FrameFormat", "syncPointVDistance");
    if (value)
        g_hash_table_replace(properties, boxing_string_clone("SyncPointVDistancePixel"), g_variant_create_string(value));

    value = boxing_config_property(config, "FrameFormat", "syncPointRadius");
    if (value)
        g_hash_table_replace(properties, boxing_string_clone("SyncPointRadiusPixel"), g_variant_create_string(value));

    value = boxing_config_property(config, "FrameFormat", "maxLevelsPerSymbol");
    if (value)
    {
        unsigned int levels_per_symbol = atoi(value);
        unsigned int bits_per_symbol = 0;
        while (levels_per_symbol)
        {
            bits_per_symbol++;
            levels_per_symbol >>= 1;
        }

        g_hash_table_replace(properties, boxing_string_clone("NumBitsPerPixel"), g_variant_create_uint(bits_per_symbol));
    }

    g_hash_table_replace(properties, boxing_string_clone("DataOrientation"), g_variant_create_uint(1));

    return (boxing_codec_syncpointinserter *)boxing_codec_syncpointinserter_create(properties, config);

}


struct symbol_tracker
{
    int   width;
    int   height;
    int   y;
    int   x;
    char *img_cur;
    char *map_bg;
    char *map_bg_cur;
    char *map_bg_end;
    char *map_fg_cur;
};

static DBOOL get_next_symbol(struct symbol_tracker * tracker)
{
    while (tracker->map_bg_cur != tracker->map_bg_end)
    {
        tracker->map_bg_cur++, tracker->img_cur++, tracker->map_fg_cur++;

        if (!(*tracker->map_bg_cur || *tracker->map_fg_cur))
        {
            ptrdiff_t pos = tracker->map_bg_cur - tracker->map_bg;
            tracker->y = (int)(pos / tracker->width);
            tracker->x = (int)(pos - (tracker->y * tracker->width));
            return DTRUE;
        }
    }

    return DFALSE;
}


static void init_tracker(struct symbol_tracker *tracker, boxing_codec_syncpointinserter * codec, char *img)
{
    tracker->map_bg = codec->bitarray_sync_point_background;
    tracker->map_bg_cur = codec->bitarray_sync_point_background-1;
    tracker->map_fg_cur = codec->bitarray_sync_point_foreground-1;
    tracker->width = codec->property_image_size_m.x;
    tracker->height = codec->property_image_size_m.y;

    tracker->map_bg_end = tracker->map_bg + (tracker->width * tracker->height);
    tracker->x = -1;
    tracker->y = -1;
    tracker->img_cur = img-1;
}

static char calc_llr(char b[2][6][6], double v[6][6])
{
    double p0 = 0;
    double p1 = 0;

    for (int row = 0; row < 6; row++)
    {
        for (int col = 0; col < 6; col++)
        {
            p0 += v[row][col] * b[0][row][col];
            p1 += v[row][col] * b[1][row][col];
        }
    }
    double llr = log1p(p1 / p0 - 1) * 10; // a sigmoid function may be a good clamping alternative
    
    if (llr > 127)
        llr = 127;
    else if (llr < -128)
        llr = -128;

    return (char)llr;
}


static DBOOL codec_decode(void * codec, gvector * data, gvector * erasures, boxing_stats_decode * stats, void* user_data)
{
    BOXING_UNUSED_PARAMETER(stats);
    BOXING_UNUSED_PARAMETER(erasures);
    BOXING_UNUSED_PARAMETER(user_data);
    boxing_image8 image;
    
    image.width = ((boxing_codec_2dpam *)codec)->syncpointinserter->property_image_size_m.x;
    image.height = ((boxing_codec_2dpam *)codec)->syncpointinserter->property_image_size_m.y;
    image.is_owning_data = DFALSE;
    image.data = data->buffer;
    unsigned int h_block_size_scale = 1;
    unsigned int v_block_size_scale = 4;
    unsigned int block_size_base = 16;
    boxing_pointi block_size = { block_size_base * h_block_size_scale, block_size_base * v_block_size_scale };
    boxing_matrix_float * means = boxing_calculate_means(&image, block_size.x, block_size.y, 6);

    // demodulate
    struct symbol_tracker tracker;

    init_tracker(&tracker, ((boxing_codec_2dpam *)codec)->syncpointinserter, data->buffer);

    gvector * decoded_data = gvector_create(data->item_size, ((boxing_codec_2dpam *)codec)->base.decoded_block_size);

    unsigned char * dst = (unsigned char *)decoded_data->buffer;

    while (1)
    {
        if (DTRUE != get_next_symbol(&tracker))
            break;
        int s0 = *(unsigned char*)tracker.img_cur;
        boxing_float *mean0 = MATRIX_MULTIPAGE_ROW_PTR(means, tracker.x/block_size.x, tracker.y/block_size.y);
        boxing_float *var0 = mean0 + 6;

        if (DTRUE != get_next_symbol(&tracker))
            break;
        int s1 = *(unsigned char*)tracker.img_cur;
        boxing_float *mean1 = MATRIX_MULTIPAGE_ROW_PTR(means, tracker.x / block_size.x, tracker.y / block_size.y);
        boxing_float *var1 = mean1 + 6;

        //double std_dev = 6.9;
        double r[6][6];
        for (int row = 0; row < 6; row++)
        {
            for (int col = 0; col < 6; col++)
            {
                //double std_dev_pow = 2 * std_dev*std_dev;
                double dS0 = s0 - mean0[col];
                double dS1 = s1 - mean1[row];
                //r[row][col] = exp(-(dS0 * dS0) / (std_dev_pow)) * exp(-(dS1 * dS1) / (std_dev_pow));
                r[row][col] = exp(-(dS0 * dS0) / (var0[col])) * exp(-(dS1 * dS1) / (var1[row]));
            }
        }

        *dst++ = calc_llr(bit_map[0], r);
        *dst++ = calc_llr(bit_map[1], r);
        *dst++ = calc_llr(bit_map[2], r);
        *dst++ = calc_llr(bit_map[3], r);
        *dst++ = calc_llr(bit_map[4], r);
    }

    gvector_swap(data, decoded_data);
    gvector_free(decoded_data);
    return DTRUE;
}

static DBOOL codec_encode(void * codec, gvector * data)
{
    gvector * encoded_data = gvector_create(data->item_size, ((boxing_codec*)codec)->encoded_block_size);

    //
    unsigned char * src = (unsigned char *)data->buffer;
    unsigned char * src_end = src + data->size;
    unsigned char * dst = (unsigned char *)encoded_data->buffer;
    for (; src < src_end; dst++, src += 5)
    {
        char value = 
            ((1 & src[0]) << 4) |
            ((1 & src[1]) << 3) |
            ((1 & src[2]) << 2) |
            ((1 & src[3]) << 1) |
            ((1 & src[4]));
        pam2d_symboli sympols = map_32_2dPam[(int)value];

        (*dst) = (unsigned char)sympols.s0;
        dst++;
        (*dst) = (unsigned char)sympols.s1;
    }

    // insert sync

    gvector_swap(data, encoded_data);
    gvector_free(encoded_data);


    boxing_codec_syncpointinserter *syncpointinserter = ((boxing_codec_2dpam *)codec)->syncpointinserter;
    return ((boxing_codec *)syncpointinserter)->encode(syncpointinserter, data);
}
