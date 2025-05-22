/*****************************************************************************
**
**  Implementation of the string interface
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
#include "boxing/string.h"
#include "boxing/platform/platform.h"
#include "g_variant.h"


/*! 
  * \addtogroup unbox
  * \{
  */


// PUBLIC STRING FUNCTIONS
//

//----------------------------------------------------------------------------
/*!
 *  \brief Split string into substrings.
 *
 *  Split input string into substrings according to the separator string.
 *
 *  \param[in]  string     Input string.
 *  \param[in]  separator  Separator string.
 *  \return vector of substrings or NULL on error.
 */

gvector * boxing_string_split(const char * string, const char * separator)
{
    if (string == NULL || separator == NULL)
    {
        return NULL;
    }

    if (*separator == '\0' || strlen(separator) > strlen(string))
    {
        char * return_string = boxing_string_clone(string);
        gvector * return_vector = gvector_create_pointers(1);
        GVECTORN(return_vector, char*, 0) = return_string;
        return return_vector;
    }

    size_t number_items = 1;
    size_t separator_size = strlen(separator);
    size_t new_string_size;
    const char * current = string;
    const char * start = string;
    char * new_string;
    const char * separator_current = separator;

    while (*current != '\0')
    {
        if (*separator_current == '\0')
        {
            ++number_items;
            separator_current = separator;
        }
        if (*current == *separator_current)
        {
            ++separator_current;
        }
        else
        {
            separator_current = separator;
        }
        ++current;
    }

    if (*separator_current == '\0')
    {
        ++number_items;
    }

    current = string;
    separator_current = separator;
    gvector * return_value = gvector_create_pointers(number_items);
    number_items = 0;
    while (*current != '\0')
    {
        if (*separator_current == '\0')
        {
            new_string_size = current - start - separator_size;
            new_string = malloc(new_string_size + 1);
            memcpy(new_string, start, new_string_size);
            new_string[new_string_size] = '\0';
            GVECTORN(return_value, char*, number_items) = new_string;
            ++number_items;
            separator_current = separator;
            start = current;
        }
        if (*current == *separator_current)
        {
            ++separator_current;
        }
        else
        {
            separator_current = separator;
        }
        ++current;
    }
    if (*separator_current == '\0' && *current == '\0')
    {
        new_string_size = current - start - separator_size;
    }
    else
    {
        new_string_size = current - start;
    }
    
    new_string = malloc( new_string_size + 1 );
    memcpy(new_string, start, new_string_size);
    new_string[new_string_size] = '\0';
    GVECTORN(return_value, char*, number_items) = new_string;
    ++number_items;
    if (*separator_current == '\0')
    {
        new_string = boxing_string_clone("");
        GVECTORN(return_value, char*, number_items) = new_string;
    }
    return return_value;
}


//----------------------------------------------------------------------------
/*!
 *  \brief Create a string copy.
 *
 *  Function to allocate a copy of an input string and return it.
 *
 *  \param[in]  string  Input string.
 *  \return Copy of input string or NULL on error.
 */

char *boxing_string_clone(const char *string) {
    if (string == NULL) return NULL;
    size_t len = strlen(string);
    char *copy = malloc(len + 1);
    if (copy == NULL) return NULL;
    memcpy(copy, string, len + 1);
    return copy;
}


//----------------------------------------------------------------------------
/*! \brief Converts string to integer.
 *
 *  \param[out]  value   Output integer value.
 *  \param[in]   string  Input string.
 *  \return DTRUE if success.
 */

DBOOL boxing_string_to_integer(int * value, const char * string)
{
    if ( string == NULL || value == NULL )
    {
        return DFALSE;
    }

    int digits[128];
    int count = 0;

    while ( *string )
    {
        switch (*string)
        {
        case '0': digits[count] = 0; break;
        case '1': digits[count] = 1; break;
        case '2': digits[count] = 2; break;
        case '3': digits[count] = 3; break;
        case '4': digits[count] = 4; break;
        case '5': digits[count] = 5; break;
        case '6': digits[count] = 6; break;
        case '7': digits[count] = 7; break;
        case '8': digits[count] = 8; break;
        case '9': digits[count] = 9; break;
        default: return DFALSE;
        }

        count++;
        string++;
    }

    if ( count == 0 )
    {
        return DFALSE;
    }

    *value = 0;
    int multiplier = 1;
    for ( int i = count - 1; i >= 0; i-- )
    {
        *value += digits[i] * multiplier;
        multiplier *= 10;
    }

    return DTRUE;
}

//----------------------------------------------------------------------------
/*!
  * \} end of unbox group
  */
