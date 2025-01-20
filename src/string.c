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
#include "boxing/platform/memory.h"
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

    if (*separator == '\0' || boxing_string_length(separator) > boxing_string_length(string))
    {
        char * return_string = boxing_string_clone(string);
        gvector * return_vector = gvector_create_pointers(1);
        GVECTORN(return_vector, char*, 0) = return_string;
        return return_vector;
    }

    size_t number_items = 1;
    size_t separator_size = boxing_string_length(separator);
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
            new_string = boxing_string_allocate(new_string_size);
            boxing_memory_copy(new_string, start, new_string_size);
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
    
    new_string = boxing_string_allocate( new_string_size );
    boxing_memory_copy(new_string, start, new_string_size);
    new_string[new_string_size] = '\0';
    GVECTORN(return_value, char*, number_items) = new_string;
    ++number_items;
    if (*separator_current == '\0')
    {
        new_string = boxing_string_allocate(0);
        GVECTORN(return_value, char*, number_items) = new_string;
    }
    return return_value;
}


//----------------------------------------------------------------------------
/*!
 *  \brief Create a string copy.
 *
 *  Function create a copy of input string and return it.
 *
 *  \param[in]  string  Input string.
 *  \return Copy of input string or NULL on error.
 */

char * boxing_string_clone(const char * string)
{
    if (string == NULL)
    {
        return NULL;
    }

    size_t string_size;
    char * return_value;
    string_size = boxing_string_length(string) + 1;
    return_value = BOXING_MEMORY_ALLOCATE_TYPE_ARRAY( char, string_size );
    boxing_memory_copy(return_value, string, string_size);
    return return_value;
}


//----------------------------------------------------------------------------
/*!
 *  \brief Allocate string
 *
 *  Allocate memory for the string with given string size.
 *  Return instance of allocated string.
 *
 *  \param[in]  length  String length.
 *  \return instance of allocated string.
 */

char * boxing_string_allocate( size_t length )
{
    char* string = BOXING_MEMORY_ALLOCATE_TYPE_ARRAY( char, (length + 1) );
    string[0] = '\0';
    return string;
}


//----------------------------------------------------------------------------
/*!
 *  \brief Frees string.
 *
 *  Frees string memory.
 *
 *  \param[in]  string  String.
 */

void boxing_string_free( char * string )
{
    boxing_memory_free(string);
}


//----------------------------------------------------------------------------
/*! \brief Copy string
 *
 *  The  boxing_string_copy() function copies the string pointed to by source, including the
 *  terminating null byte ('\\0'), to the buffer pointed to by destination. The strings
 *  may not overlap, and the destination string must be large enough to
 *  receive the copy.
 *
 *  \param[out]  destination   Destination string
 *  \param[in]   source        Source string
 */

void boxing_string_copy( char * destination, const char * source )
{
    if (destination == NULL || source == NULL)
    {
        return;
    }

    while ( *source != '\0' )
    {
        *destination = *source;
        destination++;
        source++;
    }
    *destination = '\0';
}


//----------------------------------------------------------------------------
/*!
 *  \brief Compare strings.
 *
 *  Compare two strings.
 *
 *  \param[in]   str1  First string.
 *  \param[in]   str2  Second string.
 *  \return sign of identity of the input strings.
 */

DBOOL boxing_string_equal(const char * str1, const char * str2)
{
    if ( str1 == NULL && str2 == NULL )
    {
        return DTRUE;
    }

    if ( str1 == NULL || str2 == NULL )
    {
        return DFALSE;
    }

    while (*str1 != '\0')
    {
        if (*str1 != *str2)
        {
            return DFALSE;
        }
        ++str1;
        ++str2;
    }
    return *str1 == *str2;
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
/*! \brief Get string length.
 *
 *  The boxing_string_length() function calculates the length of the string s, not
 *  including the terminating '\\0' character.
 *
 *  \param[in]  s  '\\0' terminated string
 *  \return Number of characters in s.
 */

size_t boxing_string_length(const char *s)
{
    if (s == NULL)
    {
        return 0;
    }

    size_t length = 0;
    while ( *s != '\0' )
    {
        s++;
        length++;
    }
    return length;
}


//----------------------------------------------------------------------------
/*! \brief Removes spaces at the beginning and end
 *
 *  Removes spaces at the beginning and end of the string.
 *
 *  \param[in,out]  string  A pointer to a string pointer.
 */

void boxing_string_trim(char** string)
{
    char* input_string_pointer = *string;

    if (input_string_pointer == NULL)
    {
        return;
    }

    if (boxing_string_length(input_string_pointer) == 0)
    {
        return;
    }

    size_t start_index = 0;
    while (input_string_pointer[start_index] == '\n' || input_string_pointer[start_index] == '\r' || input_string_pointer[start_index] == ' ')
    {
        start_index++;
    }

    size_t end_index = boxing_string_length(input_string_pointer);
    while ((input_string_pointer[end_index - 1] == '\n' || input_string_pointer[end_index - 1] == '\r' || input_string_pointer[end_index - 1] == ' ') && end_index > 0 && end_index > start_index)
    {
        end_index--;
    }

    boxing_string_cut(string, start_index, end_index);
}


//----------------------------------------------------------------------------
/*! \brief Get substring
 *
 *  Reallocates string to substring given by start_index and end_index.
 *
 *  \param[in,out]  string       A pointer to a string pointer.
 *  \param[in]      start_index  The index to which the function must be cut beginning of the string.
 *  \param[in]      end_index    The index after which the function needs to truncate the end of the string.
 */

void boxing_string_cut(char** string, size_t start_index, size_t end_index)
{
    if (*string == NULL || start_index > end_index)
    {
        return;
    }

    if (start_index == end_index)
    {
        char* new_string = boxing_string_allocate(1);
        new_string[0] = '\0';
        *string = new_string;
        return;
    }

    size_t string_size = boxing_string_length(*string);
    if (start_index >= string_size || end_index > string_size)
    {
        return;
    }

    if (start_index == 0 && end_index == string_size)
    {
        return;
    }

    size_t new_string_size = end_index - start_index + 1;
    char* new_string = boxing_string_allocate(new_string_size);
    char* input_string = *string;

    for (size_t i = 0; i < new_string_size - 1; i++)
    {
        new_string[i] = input_string[i + start_index];
    }

    new_string[new_string_size - 1] = '\0';

    boxing_string_free(input_string);
    *string = new_string;
}


//----------------------------------------------------------------------------
/*!
  * \} end of unbox group
  */
