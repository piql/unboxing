/*****************************************************************************
**
**  Implementation of the metadata interface
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
#include "boxing/metadata.h"
#include "boxing/math/bitutils.h"
#include "boxing/platform/platform.h"

//----------------------------------------------------------------------------
/*!
 *  \defgroup   fmetadata  Frame metadata.
 *  \brief      Structures and functions to read metadata.
 *  \ingroup    unbox
 *
 *  Structures and functions to read metadata from the metadata band
 *  located at the bottom of the frame, occupying the are between the 
 *  lower left and right corner marks.
 */


/*! 
  * \addtogroup fmetadata
  * \{
  */


//----------------------------------------------------------------------------
/*!
 *  \enum       boxing_metadata_content_types metadata.h
 *  \brief      The enumeration for specifying the content type.
 *  
 *  \param BOXING_METADATA_CONTENT_TYPES_UNKNOWN       (0) Unknown content type.
 *  \param BOXING_METADATA_CONTENT_TYPES_TOC           (1) TOC (Table of content) content type.
 *  \param BOXING_METADATA_CONTENT_TYPES_DATA          (2) Data content type.
 *  \param BOXING_METADATA_CONTENT_TYPES_VISUAL        (3) Visual content type.
 *  \param BOXING_METADATA_CONTENT_TYPES_CONTROLFRAME  (4) Control frame content type.
 *
 *  The enumeration for specifying the content type.
 */


//----------------------------------------------------------------------------
/*!
 *  \enum       boxing_metadata_type
 *  \brief      The enumeration for specifying the type of metadata.
 *  
 *  \param BOXING_METADATA_TYPE_UNKNOWN            (0xffff) Unknown metadata type.
 *  \param BOXING_METADATA_TYPE_ENDOFDATA          (0) End of data metadata type.
 *  \param BOXING_METADATA_TYPE_JOBID              (1) Job id metadata type.
 *  \param BOXING_METADATA_TYPE_FRAMENUMBER        (2) Frame number metadata type.
 *  \param BOXING_METADATA_TYPE_FILEID             (3) File id metadata type.
 *  \param BOXING_METADATA_TYPE_FILESIZE           (4) File size metadata type.
 *  \param BOXING_METADATA_TYPE_DATACRC            (5) Data CRC metadata type.
 *  \param BOXING_METADATA_TYPE_DATASIZE           (6) Data size metadata type.
 *  \param BOXING_METADATA_TYPE_SYMBOLSPERPIXEL    (7) Symbol per pixel metadata type.
 *  \param BOXING_METADATA_TYPE_CONTENTTYPE        (8) Content type metadata type.
 *  \param BOXING_METADATA_TYPE_CIPHERKEY          (9) Cipher key metadata type.
 *  \param BOXING_METADATA_TYPE_CONTENTSYMBOLSIZE  (10) Content symbol size metadata type.
 *  \param BOXING_METADATA_TYPE_LASTTYPE           (11) The type for specifying last metadata type.
 *
 *  The enumeration for specifying the metadata type.
 */


/*! 
 *  \var typedef boxing_metadata_item boxing_metadata_item_eof
 *  \brief Type definitions for specifying end of file metadata item.
 *   
 *  Type definitions for specifying end of file metadata item as boxing_metadata_item structure (base metadata item without additional value).
 */


/*! 
 *  \var typedef boxing_metadata_item_u32 boxing_metadata_item_job_id;
 *  \brief Type definitions for specifying fob id metadata item.
 *   
 *  Type definitions for specifying job id metadata item as boxing_metadata_item_u32 structure (base meta data item with uint32_t value).
 */


/*! 
 *  \var typedef boxing_metadata_item_u32 boxing_metadata_item_frame_number;
 *  \brief Type definitions for specifying frame number metadata item.
 *   
 *  Type definitions for specifying frame number metadata item as boxing_metadata_item_u32 structure (base meta data item with uint32_t value).
 */


/*! 
 *  \var typedef boxing_metadata_item_u32 boxing_metadata_item_file_id;
 *  \brief Type definitions for specifying file id metadata item.
 *   
 *  Type definitions for specifying file id metadata item as boxing_metadata_item_u32 structure (base meta data item with uint32_t value).
 */


/*! 
 *  \var typedef boxing_metadata_item_u64 boxing_metadata_item_file_size;
 *  \brief Type definitions for specifying file size metadata item.
 *   
 *  Type definitions for specifying file size metadata item as boxing_metadata_item_u64 structure (base meta data item with uint64_t value).
 */


/*! 
 *  \var typedef boxing_metadata_item_u64 boxing_metadata_item_data_crc;
 *  \brief Type definitions for specifying data CRC metadata item.
 *   
 *  Type definitions for specifying data CRC metadata item as boxing_metadata_item_u64 structure (base meta data item with uint64_t value).
 */


/*! 
 *  \var typedef boxing_metadata_item_u32 boxing_metadata_item_data_size;
 *  \brief Type definitions for specifying data size metadata item.
 *   
 *  Type definitions for specifying data size metadata item as boxing_metadata_item_u32 structure (base meta data item with uint32_t value).
 */


/*! 
 *  \var typedef boxing_metadata_item_u16 boxing_metadata_item_symbols_per_pixel;
 *  \brief Type definitions for specifying symbol per pixel metadata item.
 *   
 *  Type definitions for specifying symbol per pixel metadata item as boxing_metadata_item_u16 structure (base meta data item with uint16_t value).
 */


/*! 
 *  \var typedef boxing_metadata_item_u16 boxing_metadata_item_content_type;
 *  \brief Type definitions for specifying content type metadata item.
 *   
 *  Type definitions for specifying content type metadata item as boxing_metadata_item_u16 structure (base meta data item with uint16_t value).
 */


/*! 
 *  \var typedef boxing_metadata_item_u32 boxing_metadata_item_cipher_key;
 *  \brief Type definitions for specifying cipher key metadata item.
 *   
 *  Type definitions for specifying cipher key metadata item as boxing_metadata_item_u32 structure (base meta data item with uint32_t value).
 */


/*! 
 *  \var typedef boxing_metadata_item_u16 boxing_metadata_item_content_symbol_size;
 *  \brief Type definitions for specifying content symbol size metadata item.
 *   
 *  Type definitions for specifying content symbol size metadata item as boxing_metadata_item_u16 structure (base meta data item with uint16_t value).
 */


/*! 
 *  \var typedef GHashTable boxing_metadata_list
 *  \brief Type definitions for specifying boxing metadata list.
 *   
 *  Type definitions for specifying boxing metadata list as hash table. As a hash table is used GHashTable from the library GLIB.
 */


//----------------------------------------------------------------------------
/*!
 *  \struct     boxing_metadata_item_base_s  metadata.h
 *  \brief      Base struct for meta data item.
 *  
 *  \param type         Type of meta data item depends of enumeration variable boxing_metadata_type.
 *  \param size         The size of occupied memory (in bytes).
 *  \param serialize    Function to serialize data. (int serialize(char *data, int size, struct boxing_metadata_item_s * item))
 *  \param deserialize  Function to deserialize data. (int deserialize(struct boxing_metadata_item_s * item, const char *data, int size))
 *  \param free         Function to free the occupied memory. (free(struct boxing_metadata_item_s * item))
 *
 *  Structure describing a metadata item type.
 *  Type is set by enumeration constants boxing_metadata_type.
 *  The size is specified in bytes.
 */


//----------------------------------------------------------------------------
/*!
 *  \struct     boxing_metadata_item_s  metadata.h
 *  \brief      Struct for meta data item without additional value.
 *  
 *  \param base         Base struct for meta data item.
 *
 *  Structure storage of base meta data item without additional value.
 */


//----------------------------------------------------------------------------
/*!
 *  \struct     boxing_metadata_item_u16_s  metadata.h
 *  \brief      Struct for meta data item with uint16_t value.
 *  
 *  \param base   Base struct for meta data item.
 *  \param value  Value with type uint16_t
 *
 *  Structure storage of base meta data item with uint16_t value.
 */


//----------------------------------------------------------------------------
/*!
 *  \struct     boxing_metadata_item_u32_s  metadata.h
 *  \brief      Struct for meta data item with uint32_t value.
 *  
 *  \param base   Base struct for meta data item.
 *  \param value  Value with type uint32_t
 *
 *  Structure storage of base meta data item with uint32_t value.
 */


//----------------------------------------------------------------------------
/*!
 *  \struct     boxing_metadata_item_u64_s  metadata.h
 *  \brief      Struct for meta data item with uint64_t value.
 *  
 *  \param base   Base struct for meta data item.
 *  \param value  Value with type uint64_t
 *
 *  Structure storage of base meta data item with uint64_t value.
 */


//----------------------------------------------------------------------------
/*!
 *  \struct     boxing_metadata_item_unknown_s  metadata.h
 *  \brief      Struct for meta data item with a value of an unknown type.
 *  
 *  \param base   Base struct for meta data item.
 *  \param value  Value with unknown type.
 *
 *  Structure storage of base meta data item with value of an unknown type.
 */

//  DEFINES
//

#define METADATA_BASE_SIZE 4
#define METADATA_LIST_DEFAULT_SIZE 1024

typedef struct boxing_metadata_item_config_s
{
    const char *                name;
    int                         size;
    serialize_metadata_item     serialize;
    deserialize_metadata_item   deserialize;
    create_metadata_item        create;
    free_metadata_item          free;
} boxing_metadata_item_config;

//  PRIVATE INTERFACE
//

static boxing_metadata_item_config *   boxing_metadata_get_config(boxing_metadata_type type);

static int                             boxing_metadata_item_size(boxing_metadata_type type);
static boxing_metadata_item *          boxing_metadata_item_deserialize(const char *data, int size);

static int                             boxing_metadata_item_base_serialize(char *data, int size, boxing_metadata_item * item);
static int                             boxing_metadata_item_base_deserialize(boxing_metadata_item * item, const char *data, int size);
static boxing_metadata_item *          boxing_metadata_item_base_create();
static void                            boxing_metadata_item_base_free(boxing_metadata_item * item);

static int                             boxing_metadata_item_uint16_serialize(char *data, int size, boxing_metadata_item * item);
static int                             boxing_metadata_item_uint16_deserialize(boxing_metadata_item * item, const char *data, int size);
static boxing_metadata_item *          boxing_metadata_item_uint16_create();

static int                             boxing_metadata_item_uint32_serialize(char *data, int size, boxing_metadata_item * item);
static int                             boxing_metadata_item_uint32_deserialize(boxing_metadata_item * item, const char *data, int size);
static boxing_metadata_item *          boxing_metadata_item_uint32_create();

static int                             boxing_metadata_item_uint64_serialize(char *data, int size, boxing_metadata_item * item);
static int                             boxing_metadata_item_uint64_deserialize(boxing_metadata_item * item, const char *data, int size);
static boxing_metadata_item *          boxing_metadata_item_uint64_create();

static int                             boxing_metadata_item_unknown_serialize(char *data, int size, boxing_metadata_item * item);
static int                             boxing_metadata_item_unknown_deserialize(boxing_metadata_item * item, const char *data, int size);
static boxing_metadata_item *          boxing_metadata_item_unknown_create();
static void                            boxing_metadata_item_unknown_free(boxing_metadata_item * item);

static void                            boxing_metadata_ghash_value_free(gpointer data);
static gboolean                        boxing_metadata_ghash_uint16_equal(gconstpointer v1, gconstpointer v2);
static guint                           boxing_metadata_ghash_uint16_hash(gconstpointer v);

static boxing_metadata_item_config metadata_item_config[] =
{
    { "EndOfData", 0, boxing_metadata_item_base_serialize, boxing_metadata_item_base_deserialize, boxing_metadata_item_base_create, boxing_metadata_item_base_free },
    { "JobId", 4, boxing_metadata_item_uint32_serialize, boxing_metadata_item_uint32_deserialize, boxing_metadata_item_uint32_create, boxing_metadata_item_base_free },
    { "FrameNumber", 4, boxing_metadata_item_uint32_serialize, boxing_metadata_item_uint32_deserialize, boxing_metadata_item_uint32_create, boxing_metadata_item_base_free },
    { "FileId", 4, boxing_metadata_item_uint32_serialize, boxing_metadata_item_uint32_deserialize, boxing_metadata_item_uint32_create, boxing_metadata_item_base_free },
    { "FileSize", 8, boxing_metadata_item_uint64_serialize, boxing_metadata_item_uint64_deserialize, boxing_metadata_item_uint64_create, boxing_metadata_item_base_free },
    { "DataCrc", 8, boxing_metadata_item_uint64_serialize, boxing_metadata_item_uint64_deserialize, boxing_metadata_item_uint64_create, boxing_metadata_item_base_free },
    { "DataSize", 4, boxing_metadata_item_uint32_serialize, boxing_metadata_item_uint32_deserialize, boxing_metadata_item_uint32_create, boxing_metadata_item_base_free },
    { "SymbolsPerPixel", 2, boxing_metadata_item_uint16_serialize, boxing_metadata_item_uint16_deserialize, boxing_metadata_item_uint16_create, boxing_metadata_item_base_free },
    { "ContentType", 2, boxing_metadata_item_uint16_serialize, boxing_metadata_item_uint16_deserialize, boxing_metadata_item_uint16_create, boxing_metadata_item_base_free },
    { "CipherKey", 4, boxing_metadata_item_uint32_serialize, boxing_metadata_item_uint32_deserialize, boxing_metadata_item_uint32_create, boxing_metadata_item_base_free },
    { "ContentSymbolSize", 2, boxing_metadata_item_uint16_serialize, boxing_metadata_item_uint16_deserialize, boxing_metadata_item_uint16_create, boxing_metadata_item_base_free },
};

static boxing_metadata_item_config metadata_item_config_unknown =
{ "Unknown", 0, boxing_metadata_item_unknown_serialize, boxing_metadata_item_unknown_deserialize, boxing_metadata_item_unknown_create, boxing_metadata_item_unknown_free };

// PUBLIC METADATA FUNCTIONS
//

//----------------------------------------------------------------------------
/*!
 *  \brief Create an empty meta data list.
 *
 *  Create an empty meta data list,
 *  with specified hash, equal and free functions.
 *
 *  \return created empty meta data list.
 */

boxing_metadata_list * boxing_metadata_list_create()
{
    return g_hash_table_new_full(boxing_metadata_ghash_uint16_hash, boxing_metadata_ghash_uint16_equal, NULL, boxing_metadata_ghash_value_free);
}


//----------------------------------------------------------------------------
/*!
 *  \brief Frees occupied memory of meta data list
 *
 *  Frees occupied memory of meta data list instance.
 *
 *  \param[in]  list     The pointer to the meta data instance.
 */

void boxing_metadata_list_free(boxing_metadata_list * list)
{
    g_hash_table_destroy(list);
}


//----------------------------------------------------------------------------
/*!
 *  \brief Deserialize metadata.
 *
 *  Deserialize byte stream and create meta data list.
 *  If list or data is NULL, the function will return 0.
 *
 *  \param[out]  list     Pointer to meta data list.
 *  \param[in]   data     Pointer to input source data.
 *  \param[in]   size     Size of the input source data.
 *  \return count of metadata items deserialized.
 */

int boxing_metadata_list_deserialize(boxing_metadata_list * list, const char *data, size_t size)
{
    int item_count = 0;
    if (!list || !data)
    {
        return item_count;
    }

    boxing_metadata_list_clear(list);
    while (size > 0)
    {
        boxing_metadata_item * item = boxing_metadata_item_deserialize(data, (int)size);
        if (!item)
        {
            return item_count;
        }

        if (item->base.type == BOXING_METADATA_TYPE_ENDOFDATA)
        {
            // don't insert end of data
            boxing_metadata_item_free(item);
            return item_count;
        }

        boxing_metadata_list_append_item(list, item);

        data += METADATA_BASE_SIZE + item->base.size;
        size -= METADATA_BASE_SIZE + item->base.size;
        item_count++;
    }

    return item_count;
}


//----------------------------------------------------------------------------
/*!
 *  \brief Serialize metadata.
 *
 *  Serialize input meta data list to gvector byte stream.
 *  If pointer to meta data list equal to zero then return the empty vector of source data.
 *
 *  \param[in]  list  The pointer to meta data list instance.
 *  \return serialized list of meta data items (gvector).
 */

gvector * boxing_metadata_list_serialize(boxing_metadata_list * list)
{
    GHashTableIter iter;
    gpointer key, value;
    char buffer[256];
    gvector * vector = gvector_create(1, 0);

    g_hash_table_iter_init(&iter, list);
    while (g_hash_table_iter_next(&iter, &key, &value))
    {
        boxing_metadata_item * item = (boxing_metadata_item *)value;
        int byte_count = item->base.serialize(buffer, 256, item);
        gvector_append_data(vector, (unsigned int)byte_count, buffer);
    }

    boxing_metadata_item * item_eod = boxing_metadata_item_create(BOXING_METADATA_TYPE_ENDOFDATA);
    int byte_count = item_eod->base.serialize(buffer, 256, item_eod);
    gvector_append_data(vector, (unsigned int)byte_count, buffer);
    boxing_metadata_item_free(item_eod);
    return vector;
}


//----------------------------------------------------------------------------
/*!
 *  \brief Add new item to meta data list.
 *
 *  Add new item to meta data list.
 *  If pointer to meta data list or pointer to meta data item equal to zero then item does not adding to the meta data list.
 *
 *  \param[out]  list  The pointer to meta data list instance.
 *  \param[in]   item  The pointer to meta data item instance.
 */

void boxing_metadata_list_append_item(boxing_metadata_list * list, boxing_metadata_item *item)
{
    // Illegal to add two equal items to the list - test if item exists here...
    if (item)
    {
        g_hash_table_replace(list, &item->base.type, item);
    }
}


//----------------------------------------------------------------------------
/*!
 *  \brief Check if metadata list contains type
 *
 *  Return true if metadata list contains item of given type.
 *
 *  \param[in]  list  Meta data list instance.
 *  \param[in]  type  Type to look for.
 *  \return 0 if item does not exist
 */

int boxing_metadata_list_contains_item(boxing_metadata_list * list, boxing_metadata_type type)
{
    uint16_t key = type;
    return g_hash_table_contains(list, &key);
}


//----------------------------------------------------------------------------
/*!
 *  \brief Find the specified item an return it.
 *
 *  The function searches of specified item in the list. 
 *  Returns a pointer to the found item or Null if it was not found.
 *
 *  \param[in]  list  The pointer to meta data list instance.
 *  \param[in]  type  The type of meta data item.
 *  \return pointer to the found item.
 */

boxing_metadata_item * boxing_metadata_list_find_item(boxing_metadata_list * list, boxing_metadata_type type)
{
    uint16_t key = type;
    return g_hash_table_lookup(list, &key);
}


//----------------------------------------------------------------------------
/*!
 *  \brief Remove all items in the meta data list.
 *
 *  The function removes all items in the meta data list.
 *
 *  \param[in]  list  The pointer to meta data list instance.
 */

void boxing_metadata_list_clear(boxing_metadata_list * list)
{
    g_hash_table_remove_all(list);
}


//----------------------------------------------------------------------------
/*!
 *  \brief Creates the new instance of meta data item.
 *
 *  The function creates a new instance of an item of meta data with specified type.
 *  Returns a pointer to the created meta data item.
 *
 *  \param[in]  type  The type of meta data item.
 *  \return pointer to the created item.
 */

boxing_metadata_item * boxing_metadata_item_create(uint16_t type)
{
    uint16_t size = boxing_metadata_item_size(type);
    boxing_metadata_item_config *config = boxing_metadata_get_config(type);

    boxing_metadata_item *item = config->create();

    if (item)
    {
        item->base.type = type;
        item->base.size = size;
        item->base.serialize = config->serialize;
        item->base.deserialize = config->deserialize;
        item->base.free = config->free;
    }

    return item;
}


//----------------------------------------------------------------------------
/*!
 *  \brief Frees occupied memory of item
 *
 *  Frees occupied memory of meta data item.
 *
 *  \param[in]  item  The pointer to a meta data item.
 */

void boxing_metadata_item_free(boxing_metadata_item *item)
{
    if (item)
    {
        item->base.free(item);
    }
}


//----------------------------------------------------------------------------
/*!
 *  \brief Return name of specified meta data type
 *
 *  Function returns text representation of name of the specified meta data type.
 *
 *  \param[in]  type  The type of meta data item.
 *  \return string representation of type item.
 */

const char * boxing_metadata_type_name(boxing_metadata_type type)
{
    return  boxing_metadata_get_config(type)->name;
}


//----------------------------------------------------------------------------
/*!
  * \} end of metadata group
  */


// PRIVATE METADATA FUNCTIONS
//

static boxing_metadata_item_config * boxing_metadata_get_config(boxing_metadata_type type)
{
    if (type >= sizeof(metadata_item_config) / sizeof(metadata_item_config[0]))
        return &metadata_item_config_unknown;
    else
        return &metadata_item_config[type];

}

static int boxing_metadata_item_size(boxing_metadata_type type)
{
    return boxing_metadata_get_config(type)->size;
}


static boxing_metadata_item * boxing_metadata_item_deserialize(const char *data, int size)
{
    if (size < METADATA_BASE_SIZE)
    {
        return NULL;
    }
    uint16_t item_type = betohs(*(uint16_t*)data);
    boxing_metadata_item *item = boxing_metadata_item_create(item_type);

    if (item)
    {
        int byte_count = item->base.deserialize(item, data, size);
        if (!byte_count)
        {
            boxing_metadata_item_free(item);
            item = NULL;
        }
    }

    return item;
}


static int boxing_metadata_item_base_serialize(char *data, int size, boxing_metadata_item * item)
{
    if (size < (METADATA_BASE_SIZE))
        return 0;

    uint16_t * data_u16 = (uint16_t *)data;
    *data_u16++ = htobes(item->base.type);
    *data_u16++ = htobes(item->base.size);

    return METADATA_BASE_SIZE + item->base.size;
}

static int boxing_metadata_item_base_deserialize(boxing_metadata_item * item, const char *data, int size)
{
    if (size < (METADATA_BASE_SIZE))
        return 0;

    uint16_t * data_u16 = (uint16_t *)data;
    item->base.type = betohs(*data_u16++);
    item->base.size = betohs(*data_u16++);

    return METADATA_BASE_SIZE + item->base.size;
}

static boxing_metadata_item * boxing_metadata_item_base_create()
{
    return malloc(sizeof(boxing_metadata_item));
}

static void boxing_metadata_item_base_free(boxing_metadata_item * item)
{
    free(item);
}

static int boxing_metadata_item_uint16_serialize(char *data, int size, boxing_metadata_item * item)
{
    if (size < (METADATA_BASE_SIZE + 2))
        return 0;

    uint16_t * data_u16 = (uint16_t *)data;
    *data_u16++ = htobes(item->base.type);
    *data_u16++ = htobes(item->base.size);

    boxing_metadata_item_u16 *item_u16 = (boxing_metadata_item_u16 *)item;
    *data_u16 = htobes(item_u16->value);

    return METADATA_BASE_SIZE + item->base.size;
}

static int boxing_metadata_item_uint16_deserialize(boxing_metadata_item *item, const char *data, int size)
{
    if (size < (METADATA_BASE_SIZE + 2))
        return 0;

    uint16_t * data_u16 = (uint16_t *)data;
    item->base.type = betohs(*data_u16++);
    item->base.size = betohs(*data_u16++);

    boxing_metadata_item_u16 *item_u16 = (boxing_metadata_item_u16 *)item;
    item_u16->value = betohs(*data_u16);

    return METADATA_BASE_SIZE + item->base.size;
}

static boxing_metadata_item *boxing_metadata_item_uint16_create()
{
    return malloc(sizeof(boxing_metadata_item_u16));
}

static int boxing_metadata_item_uint32_serialize(char *data, int size, boxing_metadata_item *item)
{
    if (size < (METADATA_BASE_SIZE + 4))
        return 0;

    uint16_t * data_u16 = (uint16_t *)data;
    *data_u16++ = htobes(item->base.type);
    *data_u16++ = htobes(item->base.size);

    boxing_metadata_item_u32 *item_u32 = (boxing_metadata_item_u32 *)item;
    *(uint32_t *)data_u16 = htobel(item_u32->value);

    return METADATA_BASE_SIZE + item->base.size;
}

static int boxing_metadata_item_uint32_deserialize(boxing_metadata_item * item, const char *data, int size)
{
    if (size < (METADATA_BASE_SIZE + 4))
        return 0;

    uint16_t * data_u16 = (uint16_t *)data;
    item->base.type = betohs(*data_u16++);
    item->base.size = betohs(*data_u16++);

    boxing_metadata_item_u32 *item_u32 = (boxing_metadata_item_u32 *)item;
    uint32_t tmp;
    memcpy(&tmp, data_u16, sizeof(uint32_t));
    item_u32->value = betohl(tmp);

    return METADATA_BASE_SIZE + item->base.size;
}

static boxing_metadata_item *boxing_metadata_item_uint32_create()
{
    return malloc(sizeof(boxing_metadata_item_u32));
}

static int boxing_metadata_item_uint64_serialize(char *data, int size, boxing_metadata_item *item)
{
    if (size < (METADATA_BASE_SIZE + 8))
        return 0;

    uint16_t * data_u16 = (uint16_t *)data;
    *data_u16++ = htobes(item->base.type);
    *data_u16++ = htobes(item->base.size);

    boxing_metadata_item_u64 *item_u64 = (boxing_metadata_item_u64 *)item;
    *(uint64_t *)data_u16 = htobell(item_u64->value);

    return METADATA_BASE_SIZE + item->base.size;
}

static int boxing_metadata_item_uint64_deserialize(boxing_metadata_item *item, const char *data, int size)
{
    if (size < (METADATA_BASE_SIZE + 8))
        return 0;

    uint16_t * data_u16 = (uint16_t *)data;
    item->base.type = betohs(*data_u16++);
    item->base.size = betohs(*data_u16++);

    boxing_metadata_item_u64 *item_u64 = (boxing_metadata_item_u64 *)item;
    uint64_t tmp;
    memcpy(&tmp, data_u16, sizeof(uint64_t));
    item_u64->value = betohll(tmp);

    return METADATA_BASE_SIZE + item->base.size;
}

static boxing_metadata_item *boxing_metadata_item_uint64_create()
{
    return malloc(sizeof(boxing_metadata_item_u64));
}

static int boxing_metadata_item_unknown_serialize(char *data, int size, boxing_metadata_item *item)
{
    if (size < (METADATA_BASE_SIZE + item->base.size))
        return 0;

    uint16_t * data_u16 = (uint16_t *)data;
    *data_u16++ = htobes(item->base.type);
    *data_u16++ = htobes(item->base.size);

    boxing_metadata_item_unknown *item_unknown = (boxing_metadata_item_unknown *)item;
    memcpy((void *)data_u16, item_unknown->value, item->base.size);

    return METADATA_BASE_SIZE + item->base.size;
}

static int boxing_metadata_item_unknown_deserialize(boxing_metadata_item *item, const char *data, int size)
{
    if (size < (METADATA_BASE_SIZE))
        return 0;

    uint16_t * data_u16 = (uint16_t *)data;
    item->base.type = betohs(*data_u16++);
    item->base.size = betohs(*data_u16++);

    if (size < (item->base.size + METADATA_BASE_SIZE))
        return 0;

    if (item->base.size != 0)
    {
        boxing_metadata_item_unknown *item_unknown = (boxing_metadata_item_unknown *)item;
        item_unknown->value = malloc(item->base.size);
        if (!item_unknown->value)
        {
            return 0;
        }
        memcpy(item_unknown->value, (void *)data_u16, item->base.size);
    }
    return METADATA_BASE_SIZE + item->base.size;
}

static boxing_metadata_item *boxing_metadata_item_unknown_create()
{
    boxing_metadata_item_unknown *temp_item = malloc(sizeof(boxing_metadata_item_unknown));
    temp_item->value = NULL;
    return (boxing_metadata_item *)temp_item;
}

static void boxing_metadata_item_unknown_free(boxing_metadata_item *item)
{
    if (item)
    {
        boxing_metadata_item_unknown *item_unknown = (boxing_metadata_item_unknown *)item;
        if (item_unknown != NULL)
        {
            free(item_unknown->value);
        }
    }
    free(item);
}

static void boxing_metadata_ghash_value_free(gpointer data)
{
    boxing_metadata_item_free((boxing_metadata_item *)data);
}


static gboolean boxing_metadata_ghash_uint16_equal(gconstpointer v1, gconstpointer v2)
{
    return *((const uint16_t*)v1) == *((const uint16_t*)v2);
}

static guint boxing_metadata_ghash_uint16_hash(gconstpointer v)
{
    return *(const uint16_t*)v;
}

// ********************************** EOF **********************************************
