#ifndef BOXING_METADATA_H
#define BOXING_METADATA_H

/*****************************************************************************
**
**  Definition of the metadata C interface
**
**  Creation date:  2014/12/16
**  Created by:     Haakon Larsson
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
#include "ghash.h"
#include "gvector.h"
#include "boxing/platform/types.h"


typedef enum
{
    BOXING_METADATA_CONTENT_TYPES_UNKNOWN      = 0,
    BOXING_METADATA_CONTENT_TYPES_TOC          = 1,
    BOXING_METADATA_CONTENT_TYPES_DATA         = 2,
    BOXING_METADATA_CONTENT_TYPES_VISUAL       = 3,
    BOXING_METADATA_CONTENT_TYPES_CONTROLFRAME = 4,
} boxing_metadata_content_types;

#define BOXING_METADATA_CONTENT_TYPE_STR(bmct)                                 \
  ((bmct) == BOXING_METADATA_CONTENT_TYPES_TOC            ? "TOC"              \
   : (bmct) == BOXING_METADATA_CONTENT_TYPES_DATA         ? "DATA"             \
   : (bmct) == BOXING_METADATA_CONTENT_TYPES_VISUAL       ? "VISUAL"           \
   : (bmct) == BOXING_METADATA_CONTENT_TYPES_CONTROLFRAME ? "CONTROLFRAME"     \
                                                          : "UNKNOWN")

typedef enum
{
    BOXING_METADATA_TYPE_UNKNOWN           = 0xffff,
    BOXING_METADATA_TYPE_ENDOFDATA         = 0,
    BOXING_METADATA_TYPE_JOBID             = 1,
    BOXING_METADATA_TYPE_FRAMENUMBER       = 2,
    BOXING_METADATA_TYPE_FILEID            = 3,
    BOXING_METADATA_TYPE_FILESIZE          = 4,
    BOXING_METADATA_TYPE_DATACRC           = 5,
    BOXING_METADATA_TYPE_DATASIZE          = 6,
    BOXING_METADATA_TYPE_SYMBOLSPERPIXEL   = 7,
    BOXING_METADATA_TYPE_CONTENTTYPE       = 8,
    BOXING_METADATA_TYPE_CIPHERKEY         = 9,
    BOXING_METADATA_TYPE_CONTENTSYMBOLSIZE = 10,
    BOXING_METADATA_TYPE_LASTTYPE          = 11,
} boxing_metadata_type;

struct boxing_metadata_item_s;

typedef int(*deserialize_metadata_item)(struct boxing_metadata_item_s * item, const char *data, int size);
typedef int(*serialize_metadata_item)(char *data, int size, struct boxing_metadata_item_s * item);
typedef struct boxing_metadata_item_s* (*create_metadata_item)(void);
typedef void(*free_metadata_item)(struct boxing_metadata_item_s * item);

typedef struct boxing_metadata_item_base_s
{
    uint16_t type;
    uint16_t size;
    serialize_metadata_item serialize;
    deserialize_metadata_item deserialize;
    free_metadata_item free;
} boxing_metadata_item_base;

typedef struct boxing_metadata_item_s
{
    boxing_metadata_item_base base;
} boxing_metadata_item;

typedef struct boxing_metadata_item_u16_s
{
    boxing_metadata_item_base base;
    uint16_t value;
} boxing_metadata_item_u16;

typedef struct boxing_metadata_item_u32_s
{
    boxing_metadata_item_base base;
    uint32_t value;
} boxing_metadata_item_u32;

typedef struct boxing_metadata_item_u64_s
{
    boxing_metadata_item_base base;
    uint64_t value;
} boxing_metadata_item_u64;

typedef struct boxing_metadata_item_unknown_s
{
    boxing_metadata_item_base base;
    void * value;
} boxing_metadata_item_unknown;


typedef boxing_metadata_item boxing_metadata_item_eof;

typedef boxing_metadata_item_u32 boxing_metadata_item_job_id;

typedef boxing_metadata_item_u32 boxing_metadata_item_frame_number;

typedef boxing_metadata_item_u32 boxing_metadata_item_file_id;

typedef boxing_metadata_item_u64 boxing_metadata_item_file_size;

typedef boxing_metadata_item_u64 boxing_metadata_item_data_crc;

typedef boxing_metadata_item_u32 boxing_metadata_item_data_size;

typedef boxing_metadata_item_u16 boxing_metadata_item_symbols_per_pixel;

typedef boxing_metadata_item_u16 boxing_metadata_item_content_type;

typedef boxing_metadata_item_u32 boxing_metadata_item_cipher_key;

typedef boxing_metadata_item_u16 boxing_metadata_item_content_symbol_size;

typedef GHashTable boxing_metadata_list;

boxing_metadata_list *   boxing_metadata_list_create(void);
void                     boxing_metadata_list_free(boxing_metadata_list * list);
int                      boxing_metadata_list_deserialize(boxing_metadata_list * list, const char *data, size_t size);
gvector *                boxing_metadata_list_serialize(boxing_metadata_list * list);
void                     boxing_metadata_list_append_item(boxing_metadata_list * list, boxing_metadata_item *item);
int                      boxing_metadata_list_contains_item(boxing_metadata_list * list, boxing_metadata_type type);
boxing_metadata_item *   boxing_metadata_list_find_item(boxing_metadata_list * list, boxing_metadata_type type);
void                     boxing_metadata_list_clear(boxing_metadata_list * list);

boxing_metadata_item *   boxing_metadata_item_create(uint16_t type);
void                     boxing_metadata_item_free(boxing_metadata_item * item);

const char *             boxing_metadata_type_name(boxing_metadata_type type);

#ifdef __cplusplus
}
#endif

#endif
