/*****************************************************************************
**
**  Implementation of the config interface
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
#include "boxing/config.h"
#include "boxing/globals.h"
#include "boxing/log.h"
#include "boxing/platform/memory.h"
#include "boxing/utils.h"
#include "g_variant.h"

//  SYSTEM INCLUDES
//
#include <stdio.h>


// PRIVATE INTERFACE
//

#if !defined(CONFIG_SOURCE_DEFINITIONS)
#define CONFIG_SOURCE_DEFINITIONS
#define CONFIG_ARRAY_SIZE(a)   sizeof(a) / sizeof(a[0])

typedef struct config_pair_s
{
    const char* key;
    const char* value;
} config_pair;

typedef struct config_class_s
{
    const char* name;
    const char* version;
    config_pair * pairs;
    unsigned int count;
} config_class;

typedef struct config_structure_s
{
    const char * version;
    config_class * classes;
    const unsigned int count;
} config_structure;

#endif // CONFIG_SOURCE_DEFINITIONS

static DBOOL         is_instance_initialized = DFALSE;
static boxing_config instance;
static const g_variant* boxing_config_property_g_variant(const boxing_config * config, const char * name, const char * key);

//----------------------------------------------------------------------------
/*!
 * Properties separator.
 */

const char * PROPERTIES_SEPARATOR              = ",";


/*! 
 *  \defgroup config Configuration
 *  \brief  Frame configuration.
 *  \ingroup unbox
 *
 *  The configuration describes the frame layout and the codecs
 *  used to code and decode the digital data. The configuration is a set 
 *  of key / value strings organized into groups.
 */


//----------------------------------------------------------------------------
/*! 
 * \addtogroup config
 * \{
 */


//----------------------------------------------------------------------------
/*! 
 * \struct boxing_config_s config.h
 * \brief  Frame configuration.
 *
 * \param  groups   Hash map of configuration groups.
 * \param  aliases  Group aliases.
 *
 * The config object has a list of property groups. Each property group has a
 * set of parameters (key / value pairs) where the key must be unique for the 
 * group.
 *
 * A config property value is accessed using the group and property name (key). 
 *
 * The alias consept allows to have alternative names for a group. The goal is 
 * to remove this feature in a future release.
 */


// PUBLIC CONFIG FUNCTIONS
//

//----------------------------------------------------------------------------
/*! 
 * \brief Create config object.
 *
 * Allocate memory for the boxing_config type and initializes internal hash 
 * tables.
 *
 * \return New config instance.
 */

boxing_config * boxing_config_create()
{
    boxing_config * return_value = BOXING_MEMORY_ALLOCATE_TYPE(boxing_config);
    return_value->groups =  g_hash_table_new_full(g_str_hash, g_str_equal, boxing_utils_g_hash_table_destroy_item_string, boxing_utils_g_hash_table_destroy_item_ghash);
    return_value->aliases =  g_hash_table_new_full(g_str_hash, g_str_equal, boxing_utils_g_hash_table_destroy_item_string, boxing_utils_g_hash_table_destroy_item_string);
    return return_value;
}


//---------------------------------------------------------------------------- 
/*!
 *  Convert config_structure to boxing_config.
 * 
 *  \param config               Configuration data
 *  \param source_config_data   Source.
 *  \return DTRUE on success
 */

boxing_config* boxing_create_from_structure(config_structure* source_config_data)
{
    // If the input data is incorrect, then exit with an error
    if ( source_config_data == NULL || source_config_data->count == 0)
    {
        return NULL;
    }

    boxing_config* config = boxing_config_create();
    
    config_class * classes = source_config_data->classes;

    // Pass on all classes
    for (unsigned int i = 0; i < source_config_data->count; i++)
    {
        if (classes->version != NULL && classes->version[0] != '\0')
        {
            boxing_config_set_property(config, classes->name, "version", classes->version);
        }

        config_pair * pairs = classes->pairs;

        // Pass on all properties
        for (unsigned int j = 0; j < classes->count; j++)
        {
            boxing_config_set_property(config, classes->name, pairs->key, pairs->value);
            pairs++;
        }

        classes++;
    }

    return config;
}


//----------------------------------------------------------------------------
/*!
 *  \brief Frees occupied memory of boxing_config structure.
 *
 *  Frees occupied memory of all internal hash tables.
 *
 *  \param[in]  config  boxing_config pointer.
 */

void boxing_config_free(boxing_config * config)
{
    if (config)
    {
        g_hash_table_destroy(config->aliases);
        g_hash_table_destroy(config->groups);
        if (is_instance_initialized && (&instance == config))
        {
            is_instance_initialized = DFALSE;
        }
        else 
        {
            boxing_memory_free(config);
        }
    }
}


//----------------------------------------------------------------------------
/*!
 *  \brief Clone boxing_config structure.
 *
 *  Creates a copy of input boxing_config structure and returns it.
 *  If boxing config pointer is NULL function returns NULL.
 *
 *  \param[in]  config  boxing_config pointer.
 *  \return new copy of boxing_config structure or NULL.
 */

boxing_config * boxing_config_clone(const boxing_config * config)
{
    if (config == NULL)
    {
        return NULL;
    }

    int i, j;
    boxing_config * copy = boxing_config_create();
    for (i = 0; i < config->groups->size; i++)
    {
        guint class_hash = config->groups->hashes[i];
        
        if (HASH_IS_REAL (class_hash) )
        {
            char * class_key = config->groups->keys[i];
            GHashTable * node_class = config->groups->values[i];
            GHashTable * copy_class = g_hash_table_new_full(g_str_hash, g_str_equal, boxing_utils_g_hash_table_destroy_item_string, boxing_utils_g_hash_table_destroy_item_g_variant);

            for (j = 0; j < node_class->size; j++)
            {
                guint node_hash = node_class->hashes[j];
                char * node_key = node_class->keys[j];
                g_variant * node_value = node_class->values[j];

                if (HASH_IS_REAL (node_hash) )
                {
                    g_hash_table_replace(copy_class, boxing_string_clone(node_key), g_variant_clone(node_value));
                }
            }
            g_hash_table_replace(copy->groups, boxing_string_clone(class_key), copy_class);
        }
    }

    for (i = 0; i < config->aliases->size; i++)
    {
        guint node_hash = config->aliases->hashes[i];
        char * node_key = config->aliases->keys[i];
        char * node_value = config->aliases->values[i];

        if (HASH_IS_REAL (node_hash) )
        {
            g_hash_table_replace(copy->aliases, boxing_string_clone(node_key), boxing_string_clone(node_value));
        }
    }
    return copy;
}


//----------------------------------------------------------------------------
/*!
 *  \brief Function checks two instances of the boxing_config structures on the identity.
 *
 *  Function checks two instances of the boxing_config structures on the identity.
 *  Return DTRUE if groups (data) and aliaseses (aliases) are equal.
 *
 *  \param[in]   a  Pointer to the first instance of the boxing_config structure.
 *  \param[in]   b  Pointer to the second instance of the boxing_config structure.
 *  \return sign of identity of the input structures.
 */

DBOOL boxing_config_is_equal(const boxing_config * a, const boxing_config * b)
{
    int i, j;
    if (a == NULL || b == NULL)
    {
        return a == b;
    }
    
    if (a->aliases->size != b->aliases->size || a->groups->size != b->groups->size)
    {
        return DFALSE;
    }

    // Check properties.
    for (i = 0; i < a->groups->size; i++)
    {
        if (!HASH_IS_REAL(a->groups->hashes[i]))
        {
            continue;
        }
        GHashTable * b_class = g_hash_table_lookup(b->groups, a->groups->keys[i]);
        if (b_class == NULL)
        {
            return DFALSE;
        }
        GHashTable * a_class = a->groups->values[i];

        if (a_class->size != b_class->size)
        {
            return DFALSE;
        }

        for (j = 0; j < a_class->size; j++)
        {
            if (!HASH_IS_REAL(a_class->hashes[j]))
            {
                continue;
            }
            g_variant * b_property = g_hash_table_lookup(b_class, a_class->keys[j]);
            if (b_property == NULL || !g_variant_equals(b_property, a_class->values[j]))
            {
                return DFALSE;
            }
        }
    }

    // Check aliases.
    for (i = 0; i < a->aliases->size; i++)
    {
        if (!HASH_IS_REAL(a->aliases->hashes[i]))
        {
            continue;
        }

        char * b_alias = g_hash_table_lookup(b->aliases, a->aliases->keys[i]);

        if (b_alias == NULL && a->aliases->values[i] == NULL)
        {
            continue;
        }

        if (b_alias == NULL || !boxing_string_equal(b_alias, a->aliases->values[i]))
        {
            return DFALSE;
        }
    }

    return DTRUE;
}


//----------------------------------------------------------------------------
/*! 
 * \brief Get global config instance.
 *
 *  Ig global configuration is not initialized,
 *  than initializes internal hash tables and
 *  set is_instance_initialized variable.
 *
 * \note Local config objects can be created using boxing_config_create.
 * \return pointer to the global config instance.
 */

boxing_config * boxing_config_instance()
{
    if (!is_instance_initialized)
    {
        is_instance_initialized = DTRUE;
        instance.groups =  g_hash_table_new_full(g_str_hash, g_str_equal, boxing_utils_g_hash_table_destroy_item_string, boxing_utils_g_hash_table_destroy_item_ghash);
        instance.aliases =  g_hash_table_new_full(g_str_hash, g_str_equal, boxing_utils_g_hash_table_destroy_item_string, boxing_utils_g_hash_table_destroy_item_string);
    }
    return &instance;
}


//----------------------------------------------------------------------------
/*!
 *  \brief Set string property.
 *
 *  The function sets a string property.
 *  If the property group does not exist, a new group is created. 
 *  If the property does not exist, a new property is added. 
 *  If the property exists, its value is replaced by the new value.
 *
 *  \param[in]   config   boxing_config pointer.
 *  \param[in]   group    Group name.
 *  \param[in]   key      Property key.
 *  \param[in]   value    New property value.
 */

void boxing_config_set_property(boxing_config * config, const char * group, const char * key, const char * value)
{
    GHashTable * class_hash;
    GHashTable * class_property;
    const char * alias_name;
    int i;
    if (!boxing_string_equal(key, CONFIG_XML_KEY_ALIAS)) 
    {
        class_hash = g_hash_table_lookup(config->groups, group);
        if (class_hash == NULL)
        {
            GHashTable *map_properties = g_hash_table_new_full(g_str_hash, g_str_equal, boxing_utils_g_hash_table_destroy_item_string, boxing_utils_g_hash_table_destroy_item_g_variant);
            g_hash_table_replace(map_properties, boxing_string_clone(key), g_variant_create(value));
            g_hash_table_replace(config->groups, boxing_string_clone(group), map_properties);
        }
        else
        {
            g_hash_table_replace(class_hash, boxing_string_clone(key), g_variant_create(value));
        }
        alias_name = g_hash_table_lookup(config->aliases, group);
        if(alias_name != NULL) 
        {
            boxing_config_set_property(config, alias_name, key, value);
        }
    }
    else
    {
        g_hash_table_replace(config->aliases, boxing_string_clone(value), boxing_string_clone(group));
        class_property = g_hash_table_lookup(config->groups, value);
        if (class_property != NULL)
        {
            class_hash = g_hash_table_lookup(config->groups, group);
            if (class_hash == NULL)
            {
                GHashTable *map_properties = g_hash_table_new_full(g_str_hash, g_str_equal, boxing_utils_g_hash_table_destroy_item_string, boxing_utils_g_hash_table_destroy_item_g_variant);
                g_hash_table_replace(config->groups, boxing_string_clone(group), map_properties);
                class_hash = map_properties;
            }
            
            for (i = 0; i < class_property->size; i++)
            {
                guint node_hash = class_property->hashes[i];
                gpointer node_key = class_property->keys[i];
                gpointer node_value = class_property->values[i];
                
                if (HASH_IS_REAL (node_hash) )
                {
                    g_hash_table_replace(class_hash, boxing_string_clone((char *)node_key), g_variant_create((char *)node_value));
                }
            }
        }
    }
}


//----------------------------------------------------------------------------
/*!
 *  \brief Set unsinged int property.
 *
 *  The function sets a new unsigned integer property.
 *  If this property does not exist, a new one is added. 
 *  If this property exists, it is replaced by a new value.
 *
 *  \param[in]   config   Pointer to the boxing_config structure.
 *  \param[in]   name     Property name.
 *  \param[in]   key      Property key.
 *  \param[in]   value    New unsigned integer property value.
 * 
 *  \todo consider optimizing
 */

void boxing_config_set_property_uint(boxing_config * config, const char * name, const char * key, unsigned int value)
{
    char buffer[64];
    sprintf(buffer, "%u", value);
    boxing_config_set_property(config, name, key, buffer);
}


//----------------------------------------------------------------------------
/*!
 *  \brief Get all group properties.
 *
 *  Sets properties pointer.
 *
 *  \param[in]   config      Pointer to the boxing_config structure.
 *  \param[in]   name        Property name.
 *  \param[out]  properties  Properties hash table.
 */

void boxing_config_properties(const boxing_config * config, const char * name, const GHashTable ** properties)
{
    *properties = g_hash_table_lookup(config->groups, name);
}


//----------------------------------------------------------------------------
/*!
 *  \brief Get string property.
 *
 *  Get string property.
 *
 *  \param[in]   config  Pointer to the boxing_config structure.
 *  \param[in]   name    Name.
 *  \param[in]   key     Key.
 *  \return required string property or NULL.
 */

const char* boxing_config_property(const boxing_config * config, const char * name, const char * key)
{
    GHashTable * class_hash;
    
    class_hash = g_hash_table_lookup(config->groups, name);
    if (class_hash != NULL)
    {
        gpointer item = g_hash_table_lookup(class_hash, key);
        if (item)
        {
            return (char*)((g_variant*)item)->data;
        }
    }
    return NULL;
}


//----------------------------------------------------------------------------
/*!
 *  \brief Get integer property.
 *
 *  Get integer property.
 *
 *  \param[in]   config  Pointer to the boxing_config structure.
 *  \param[in]   name    Name.
 *  \param[in]   key     Key.
 *  \return required integer property or NULL.
 */

int boxing_config_property_int(const boxing_config * config, const char * name, const char * key)
{
    return g_variant_to_int(boxing_config_property_g_variant(config, name, key));
}


//----------------------------------------------------------------------------
/*!
 *  \brief Get unsigned integer property.
 *
 *  Get unsigned integer property.
 *
 *  \param[in]   config  Pointer to the boxing_config structure.
 *  \param[in]   name    Name.
 *  \param[in]   key     Key.
 *  \return required unsigned integer property or NULL.
 */

unsigned int boxing_config_property_uint(const boxing_config * config, const char * name, const char * key)
{
    return g_variant_to_uint(boxing_config_property_g_variant(config, name, key));
}


//----------------------------------------------------------------------------
/*!
 *  \brief Get integer point property.
 *
 *  Get integer point property.
 *
 *  \param[in]   config  Pointer to the boxing_config structure.
 *  \param[in]   name    Name.
 *  \param[in]   key     Key.
 *  \param[out]  found   A sign of the success of the search for the required property.
 *  \return required integer point property or NULL.
 */

boxing_pointi boxing_config_property_pointi(const boxing_config * config, const char * name, const char * key, DBOOL * found)
{
    boxing_pointi return_value;
    char * property_hash;
    GHashTable * class_hash = g_hash_table_lookup(config->groups, name);
    
    return_value.x = return_value.y = 0;
    if (class_hash != NULL)
    {
        property_hash = g_hash_table_lookup(class_hash, key);
        if (property_hash != NULL)
        {
            gvector * list_coord = boxing_config_parse_list_properties(config, name, key);
            if (list_coord->size != 2)
            {
                DLOG_ERROR1("PointF unsupported format %s", property_hash);
                *found = DFALSE;
                return return_value;
            }
            else
            {
                return_value.x = atoi(GVECTORN(list_coord, char *, 0));
                return_value.y = atoi(GVECTORN(list_coord, char *, 1));
            }
            gvector_free(list_coord);
        }
        else
        {
            *found = DFALSE;
            return return_value;
        }
    }
    else
    {
        *found = DFALSE;
        return return_value;
    }
    *found = DTRUE;
    return return_value;
}


//----------------------------------------------------------------------------
/*!
 *  \brief Get float point property.
 *
 *  Get float point property.
 *
 *  \param[in]   config     Pointer to the boxing_config structure.
 *  \param[in]   name       Name.
 *  \param[in]   key        Key.
 *  \param[out]  was_found  A sign of the success of the search for the required property.
 *  \return required float point property or NULL.
 */

boxing_pointf boxing_config_property_pointf(const boxing_config * config, const char * name, const char * key, DBOOL * was_found)
{
    char * property_hash;
    GHashTable * class_hash = g_hash_table_lookup(config->groups, name);
    boxing_pointf return_value;
    return_value.x = return_value.y = 0;
    if (class_hash != NULL)
    {
        property_hash = g_hash_table_lookup(class_hash, key);
        if (property_hash != NULL)
        {
            gvector * list_coord = boxing_config_parse_list_properties(config, name, key);
            if (list_coord->size != 2)
            {
                DLOG_ERROR1("PointF unsupported format %s", property_hash);
                *was_found = DFALSE;
                return return_value;
            }
            else
            {
                return_value.x = (boxing_float)atof(GVECTORN(list_coord, char *, 0));
                return_value.y = (boxing_float)atof(GVECTORN(list_coord, char *, 1));
            }
            gvector_free(list_coord);
        }
        else
        {
            *was_found = DFALSE;
            return return_value;
        }
    }
    else
    {
        *was_found = DFALSE;
        return return_value;
    }
    *was_found = DTRUE;
    return return_value;
}


//----------------------------------------------------------------------------
/*!
 *  \brief Check if property exists.
 *
 *  Check if property exists. Return DTRUE if required property exist.
 *
 *  \param[in]   config     Pointer to the boxing_config structure.
 *  \param[in]   name       Name.
 *  \param[in]   key        Key.
 *  \return DTRUE if property exist.
 */

DBOOL boxing_config_is_set(const boxing_config * config, const char * name, const char * key)
{
    GHashTable * class_hash = g_hash_table_lookup(config->groups, name);
    if (class_hash != NULL)
    {
        return g_hash_table_lookup(class_hash, key) != NULL;
    }

    return DFALSE;
}


//----------------------------------------------------------------------------
/*!
 *  \brief Get comma separated property value.
 *
 *  Get comma separated property value
 *
 *  \param[in]   config     Pointer to the boxing_config structure.
 *  \param[in]   name       Name.
 *  \param[in]   key        Key.
 *  \return Vector of values.
 */

gvector * boxing_config_parse_list_properties(const boxing_config * config, const char * name, const char * key)
{
    const char * data = boxing_config_property(config, name, key);
    if (data != NULL)
    {
        gvector * list_separate = boxing_string_split(data, PROPERTIES_SEPARATOR);
        return list_separate;
    }
    else
    {
        return gvector_create(sizeof(char *), 0);
    }
}

//----------------------------------------------------------------------------
/*!
 * \} end of config group
 */


// PRIVATE CONFIG FUNCTIONS
//

//----------------------------------------------------------------------------
/*!
 * \brief Get g_variant property.
 */
static const g_variant* boxing_config_property_g_variant(const boxing_config * config, const char * name, const char * key)
{
    GHashTable * class_hash;
    
    class_hash = g_hash_table_lookup(config->groups, name);
    if (class_hash != NULL)
    {
        return g_hash_table_lookup(class_hash, key);
    }
    return NULL;
}
