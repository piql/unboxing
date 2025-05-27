#ifndef BOXING_CONFIG_H
#define BOXING_CONFIG_H

/*****************************************************************************
**
**  Definition of the config interface
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

#include <stdlib.h> // We want the stdlib definition of NULL so glib doesn't override it
#include "ghash.h"
#include "gvector.h"
#include "boxing/string.h"
#include "boxing/bool.h"
#include "boxing/math/math.h"


typedef struct boxing_config_s
{
    GHashTable * groups;
    GHashTable * aliases;
} boxing_config;

struct config_structure_s;  
  
extern const char * PROPERTIES_SEPARATOR;

boxing_config *   boxing_config_create(void);
boxing_config *   boxing_config_create_from_structure(struct config_structure_s* structure);    
void              boxing_config_free(boxing_config * config);
boxing_config *   boxing_config_clone(const boxing_config * config);
DBOOL             boxing_config_is_equal(const boxing_config *a, const boxing_config *b);
boxing_config *   boxing_config_instance(void);
void              boxing_config_set_property(boxing_config * config, const char * group, const char * key, const char * value);
void              boxing_config_set_property_uint(boxing_config * config, const char * group, const char * key, unsigned int value);

void              boxing_config_properties(const boxing_config * config, const char * group, const GHashTable ** properties);
const char *      boxing_config_property(const boxing_config * config, const char * group, const char * key);
int               boxing_config_property_int(const boxing_config * config, const char * group, const char * key);
unsigned int      boxing_config_property_uint(const boxing_config * config, const char * group, const char * key);
boxing_pointi     boxing_config_property_pointi(const boxing_config * config, const char * group, const char * key, DBOOL * found);
boxing_pointf     boxing_config_property_pointf(const boxing_config * config, const char * group, const char * key, DBOOL * found);
DBOOL             boxing_config_is_set(const boxing_config * config, const char * group, const char * key);
gvector *         boxing_config_parse_list_properties(const boxing_config * config, const char * group, const char * key);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
