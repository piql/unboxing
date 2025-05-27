/* GLIB - Library of useful routines for C programming
 * Copyright (C) 1995-1997  Peter Mattis, Spencer Kimball and Josh MacDonald
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>.
 */

/*
 * Modified by the GLib Team and others 1997-2000.  See the AUTHORS
 * file for a list of people on the GLib Team.  See the ChangeLog
 * files for a list of changes.  These files are distributed with
 * GLib at ftp://ftp.gtk.org/pub/gtk/.
 */

/*
 * Modified by Piql: Added custom types.
 */

//  PROJECT INCLUDES
//

#include "g_variant.h"
#include "boxing/string.h"
#include "boxing/math/math.h"

//  SYSTEM INCLUDES
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#if defined(_MSC_VER)
#define strcasecmp _stricmp
#endif

//  PRIVATE INTERFACE
//

static boxing_pointi string_to_pointi(const char * str, DBOOL * success);

// PUBLIC G_VARIANT FUNCTIONS
//

g_variant *g_variant_create_pointer(void *value)
{
    g_variant *newVariant = malloc(sizeof(g_variant));
    newVariant->data = malloc(sizeof(void *));

    newVariant->type = G_VARIANT_POINTER;
    *(void **)newVariant->data = value;

    return newVariant;
}


g_variant *g_variant_create_int(int value)
{
    g_variant *newVariant = malloc(sizeof(g_variant));
    newVariant->data = malloc(sizeof(int));

    newVariant->type = G_VARIANT_INT;
    *((int *)newVariant->data) = value;

    return newVariant;
}


g_variant *g_variant_create_uint(unsigned int value)
{
    g_variant *newVariant = malloc(sizeof(g_variant));
    newVariant->data = malloc(sizeof(unsigned int));

    newVariant->type = G_VARIANT_UINT;
    *((unsigned int *)newVariant->data) = value;

    return newVariant;
}

g_variant *g_variant_create_longlong(long long value)
{
    g_variant *newVariant = malloc(sizeof(g_variant));
    newVariant->data = malloc(sizeof(long long));

    newVariant->type = G_VARIANT_LONGLONG;
    *((long long *)newVariant->data) = value;

    return newVariant;
}


g_variant *g_variant_create_ulonglong(unsigned long long value)
{
    g_variant *newVariant = malloc(sizeof(g_variant));
    newVariant->data = malloc(sizeof(unsigned long long));

    newVariant->type = G_VARIANT_ULONGLONG;
    *((unsigned long long *)newVariant->data) = value;

    return newVariant;
}


g_variant *g_variant_create_float(float value)
{
    g_variant *newVariant = malloc(sizeof(g_variant));
    newVariant->data = malloc(sizeof(float));

    newVariant->type = G_VARIANT_FLOAT;
    *((float *)newVariant->data) = value;

    return newVariant;
}


g_variant *g_variant_create_double(double value)
{
    g_variant *newVariant = malloc(sizeof(g_variant));
    newVariant->data = malloc(sizeof(double));

    newVariant->type = G_VARIANT_DOUBLE;
    *((double *)newVariant->data) = value;

    return newVariant;
}


g_variant *g_variant_create_string(const char * value)
{
    g_variant *newVariant = malloc(sizeof(g_variant));
    newVariant->data = boxing_string_clone(value);
    newVariant->type = G_VARIANT_STRING;

    return newVariant;
}

g_variant *g_variant_create_pointi(boxing_pointi value)
{
    g_variant *newVariant = malloc(sizeof(g_variant));
    newVariant->data = malloc(sizeof(boxing_pointi));

    newVariant->type = G_VARIANT_POINTI;
    *((boxing_pointi *)newVariant->data) = value;

    return newVariant;
}

g_variant *g_variant_create(const char *value)
{
    return g_variant_create_string(value);
}



void g_variant_free(g_variant *variant)
{
    if (variant)
    {
        free(variant->data);
        free(variant);
    }
}


int g_variant_to_int(const g_variant *variant)
{
    if (variant == NULL)
    {
        return 0;
    }
    switch (variant->type)
    {
        case G_VARIANT_POINTER:
            return (int)(intptr_t)variant->data;
        case G_VARIANT_INT:
            return *(int *)variant->data;
        case G_VARIANT_UINT:
            return (int)*(unsigned int *)variant->data;
        case G_VARIANT_LONGLONG:
            return (int)*(long long *)variant->data;
        case G_VARIANT_ULONGLONG:
            return (int)*(unsigned long long *)variant->data;
        case G_VARIANT_FLOAT:
            return (int)*((float *)variant->data);
        case G_VARIANT_DOUBLE:
            return (int)*((double *)variant->data);
        case G_VARIANT_STRING:
            return atoi((char *)variant->data);
        case G_VARIANT_POINTI:
            return ((boxing_pointi *)variant->data)->x;

    }
    return -1;
}

unsigned int g_variant_to_uint(const g_variant *variant)
{
    if (variant == NULL)
    {
        return 0;
    }

    switch (variant->type)
    {
        case G_VARIANT_POINTER:
            return (unsigned int)(uintptr_t)variant->data;
        case G_VARIANT_INT:
            return *(int *)variant->data;
        case G_VARIANT_UINT:
            return (unsigned int)*(unsigned int *)variant->data;
        case G_VARIANT_LONGLONG:
            return (unsigned int)*(long long *)variant->data;
        case G_VARIANT_ULONGLONG:
            return (unsigned int)*(unsigned long long *)variant->data;
        case G_VARIANT_FLOAT:
            return (unsigned int)*((float *)variant->data);
        case G_VARIANT_DOUBLE:
            return (unsigned int)*((double *)variant->data);
        case G_VARIANT_STRING:
            return (unsigned int)strtol((char *)variant->data, NULL, 0); // warning may not be fully compatible
        case G_VARIANT_POINTI:
            return (unsigned int)((boxing_pointi *)variant->data)->x;

    }
    return (unsigned int)-1;
}

long long g_variant_to_longlong(const g_variant *variant)
{
    switch (variant->type)
    {
    case G_VARIANT_POINTER:
        return (long long)(intptr_t)variant->data;
    case G_VARIANT_INT:
        return (long long)*(int *)variant->data;
    case G_VARIANT_UINT:
        return (long long)*(unsigned int *)variant->data;
    case G_VARIANT_LONGLONG:
        return (long long)*(long long *)variant->data;
    case G_VARIANT_ULONGLONG:
        return (long long)*(unsigned long long *)variant->data;
    case G_VARIANT_FLOAT:
        return (long long)*((float *)variant->data);
    case G_VARIANT_DOUBLE:
        return (long long)*((double *)variant->data);
    case G_VARIANT_STRING:
        return (long long)strtoll((char *)variant->data, NULL, 0); // warning may not be fully compatible
    case G_VARIANT_POINTI:
        return (long long)((boxing_pointi *)variant->data)->x;

    }
    return (unsigned int)-1;
}

unsigned long long g_variant_to_ulonglong(const g_variant * variant)
{
    switch (variant->type)
    {
    case G_VARIANT_POINTER:
        return (unsigned long long)(uintptr_t)variant->data;
    case G_VARIANT_INT:
        return (unsigned long long)*(int *)variant->data;
    case G_VARIANT_UINT:
        return (unsigned long long)*(unsigned int *)variant->data;
    case G_VARIANT_LONGLONG:
        return (unsigned long long)*(long long *)variant->data;
    case G_VARIANT_ULONGLONG:
        return (unsigned long long)*(unsigned long long *)variant->data;
    case G_VARIANT_FLOAT:
        return (unsigned long long)*((float *)variant->data);
    case G_VARIANT_DOUBLE:
        return (unsigned long long)*((double *)variant->data);
    case G_VARIANT_STRING:
        return (unsigned long long)strtoull((char *)variant->data, NULL, 0); // warning may not be fully compatible
    case G_VARIANT_POINTI:
        return (unsigned long long)((boxing_pointi *)variant->data)->x;

    }
    return (unsigned int)-1;
}

char *g_variant_to_string(const g_variant *variant)
{
    char numberBuffer[100];
    char *return_value = NULL;
    switch (variant->type)
    {
        case G_VARIANT_POINTER:
            sprintf(numberBuffer, "%u", (unsigned int)(uintptr_t)variant->data);
            return_value = numberBuffer;
            break;
        case G_VARIANT_INT:
            sprintf(numberBuffer, "%i", *(int *)variant->data);
            return_value = numberBuffer;
            break;
        case G_VARIANT_UINT:
            sprintf(numberBuffer, "%u", *(unsigned int *)variant->data);
            return_value = numberBuffer;
            break;
        case G_VARIANT_LONGLONG:
            sprintf(numberBuffer, "%lld", *((long long *)variant->data));
            return_value = numberBuffer;
            break;
        case G_VARIANT_ULONGLONG:
            sprintf(numberBuffer, "%llu", *(unsigned long long *)variant->data);
            return_value = numberBuffer;
            break;
        case G_VARIANT_FLOAT:
            sprintf(numberBuffer, "%f", *(float *)variant->data);
            return_value = numberBuffer;
            break;
        case G_VARIANT_DOUBLE:
            sprintf(numberBuffer, "%fl", *(double *)variant->data);
            return_value = numberBuffer;
            break;
        case G_VARIANT_STRING:
            return_value = (char *)variant->data;
            break;
        case G_VARIANT_POINTI:
            sprintf(numberBuffer, "%i,%i", ((boxing_pointi *)variant->data)->x, ((boxing_pointi *)variant->data)->y);
            return_value = numberBuffer;
            break;

    }
    return boxing_string_clone(return_value);
}

#define IS_DIGIT(c) ((c) <= '9' && (c) >= '0')
#define IS_SIGN(c) (c == '-' || c == '+')


boxing_pointi g_variant_to_pointi(const g_variant * variant, DBOOL * success)
{
    if (success)
        *success = DFALSE;
    boxing_pointi return_value;
    return_value.x = return_value.y = 0;
    switch (variant->type)
    {
        case G_VARIANT_INT:
            return_value.x = *(int *)variant->data;
            break;
        case G_VARIANT_UINT:
            return_value.x = (int)*(unsigned int *)variant->data;
            break;
        case G_VARIANT_LONGLONG:
            return_value.x = (int)*(long long *)variant->data;
            break;
        case G_VARIANT_ULONGLONG:
            return_value.x = (int)*(unsigned long long *)variant->data;
            break;
        case G_VARIANT_FLOAT:
            return_value.x = (int)*((float *)variant->data);
            break;
        case G_VARIANT_DOUBLE:
            return_value.x = (int)*((double *)variant->data);
            break;
        case G_VARIANT_STRING:
            return_value = string_to_pointi((char *)variant->data, success);
            break;
        case G_VARIANT_POINTI:
            if (success)
                *success = DTRUE;
            return_value = *((boxing_pointi *)variant->data);
            break;
        case G_VARIANT_POINTER: // one less warning
            break;
    }
    return return_value;
}

char * g_variant_if_string(const g_variant * variant)
{
    return variant->type == G_VARIANT_STRING ? (char *)variant->data : NULL;
}

void g_variant_set_uint(g_variant * variant, unsigned int value)
{
    if (variant->data == NULL)
    {
        variant->data = malloc(sizeof(unsigned int));
    }
    else
    {
        if (variant->type != G_VARIANT_UINT)
        {
            free(variant->data);
            variant->data = malloc(sizeof(unsigned int));
        }
    }
    variant->type = G_VARIANT_UINT;
    *((unsigned int *)variant->data) = value;
}

int g_variant_to_bool(const g_variant * variant)
{
    switch (variant->type)
    {
        case G_VARIANT_POINTER:
            return variant->data != 0;
        case G_VARIANT_INT:
            return *(int *)variant->data != 0;
        case G_VARIANT_UINT:
            return *(int *)variant->data != 0;
        case G_VARIANT_LONGLONG:
            return *(long long *)variant->data != 0;
        case G_VARIANT_ULONGLONG:
            return *(unsigned long long *)variant->data != 0;
        case G_VARIANT_FLOAT:
            return *(float *)variant->data != 0;
        case G_VARIANT_DOUBLE:
            return *(double *)variant->data != 0;
            break;
        case G_VARIANT_STRING: {
          return (((char *)variant->data)[0] == 'T' ||
                  ((char *)variant->data)[0] == 't') &&
                 (((char *)variant->data)[1] == 'R' ||
                  ((char *)variant->data)[1] == 'r') &&
                 (((char *)variant->data)[2] == 'U' ||
                  ((char *)variant->data)[2] == 'u') &&
                 (((char *)variant->data)[3] == 'E' ||
                  ((char *)variant->data)[3] == 'e') &&
                 ((char *)variant->data)[4] == '\0';
        }
        case G_VARIANT_POINTI:
            return DFALSE;
    }
    return 0;
}

g_variant * g_variant_clone(const g_variant * variant)
{
    switch (variant->type)
    {
        case G_VARIANT_POINTER:
            return g_variant_create_pointer(variant->data);
        case G_VARIANT_INT:
            return g_variant_create_int(*(int *)variant->data);
        case G_VARIANT_UINT:
            return g_variant_create_uint(*(int *)variant->data);
        case G_VARIANT_LONGLONG:
            return g_variant_create_longlong(*(long long *)variant->data);
        case G_VARIANT_ULONGLONG:
            return g_variant_create_ulonglong(*(unsigned long long *)variant->data);
        case G_VARIANT_FLOAT:
            return g_variant_create_float(*(float *)variant->data);
        case G_VARIANT_DOUBLE:
            return g_variant_create_double(*(double *)variant->data);
            break;
        case G_VARIANT_STRING:
            return g_variant_create_string((char *)variant->data);
        case G_VARIANT_POINTI:
            return g_variant_create_pointi(*(boxing_pointi *)variant->data);
    }
    return NULL;
}

DBOOL g_variant_equals(const g_variant *a, const g_variant *b)
{
    if (a == NULL || b == NULL)
    {
        return a == b;
    }
    if (a->type != b->type)
    {
        char * stra = g_variant_to_string(a);
        char * strb = g_variant_to_string(b);
        DBOOL returnValue = strcmp(stra, strb) == 0;
        free(stra);
        free(strb);
        return returnValue;
    }
    else
    {
        switch (a->type)
        {
        case G_VARIANT_POINTER:
            return *((void **)(a->data)) == *((void **)(b->data));
        case G_VARIANT_UINT:
        case G_VARIANT_INT:
            return *((int*)(a->data)) == *((int*)(b->data));
        case G_VARIANT_ULONGLONG:
        case G_VARIANT_LONGLONG:
            return *((long long*)(a->data)) == *((long long*)(b->data));
        case G_VARIANT_FLOAT:
            return *((float*)(a->data)) == *((float*)(b->data));
        case G_VARIANT_DOUBLE:
            return *((double*)(a->data)) == *((double*)(b->data));
            break;
        case G_VARIANT_STRING:
            return strcmp(a->data, b->data) == 0;
        case G_VARIANT_POINTI:
            return (memcmp(a->data, b->data, sizeof(boxing_pointi)) == 0);
        }
        return DFALSE;
    }
}


// PRIVATE G_VARIANT FUNCTIONS
//

static boxing_pointi string_to_pointi(const char * str, DBOOL * success)
{
    if (success)
        *success = DFALSE;
    boxing_pointi return_value;
    return_value.x = return_value.y = 0;
    if (str[0] == '\0' || !IS_DIGIT(str[0]) || !IS_SIGN(str[0]))
    {
        return return_value;
    }
    int i = 0;
    if (IS_SIGN(str[0]))
    {
        ++i;
    }
    if (!IS_DIGIT(str[1]))
    {
        return return_value;
    }
    while (IS_DIGIT(str[i]))
    {
        ++i;
    }
    if (str[i] == '\0')
    {
        return return_value;
    }
    char *buffer = malloc(i + 1);
    memcpy(buffer, str, i - 1);
    buffer[i] = 0;
    return_value.x = atoi(buffer);
    free(buffer);
    while (str[i] && !IS_DIGIT(str[i]) && !IS_SIGN(str[i]))
    {
        ++i;
    }
    if (!str[i])
    {
        return return_value;
    }
    if (success)
        *success = DTRUE;
    return_value.y = atoi(str + i);
    return return_value;
}
