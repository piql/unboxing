/*****************************************************************************
**
**  Definition of the memory interface
**
**  Creation date:  2016/06/15
**  Created by:     Ole Liabo
**
**
**  Copyright (c) 2016 Piql AS. All rights reserved.
**
**  This file is part of the boxing library
**
*****************************************************************************/

//  PROJECT INCLUDES
//
#include "boxing/platform/memory.h"


/*! 
  * \addtogroup platform
  * \{
  */


//----------------------------------------------------------------------------
/*!
 *  \def BOXING_MEMORY_ALLOCATE_TYPE(type) memory.h
 *  \brief Allocates memory for the specified type.
 *
 *  Allocates memory for the specified type.
 *
 *  \param[in]  type  Type.
 */


//----------------------------------------------------------------------------
/*!
 *  \def BOXING_MEMORY_ALLOCATE_TYPE_ARRAY(type, count) memory.h
 *  \brief Allocates memory for the array with specified type.
 *
 *  Allocates memory for the array with specified type.
 *
 *  \param[in]  type   Type.
 *  \param[in]  count  Number of array elements.
 */


//----------------------------------------------------------------------------
/*!
 *  \def BOXING_MEMORY_ALLOCATE_TYPE_ARRAY_CLEAR(type, count) memory.h
 *  \brief Allocates memory for the array with specified type and clears the allocated memory.
 *
 *  Allocates memory for the array with specified type and clears the allocated memory.
 *
 *  \param[in]  type   Type.
 *  \param[in]  count  Number of array elements.
 */


//----------------------------------------------------------------------------
/*!
 *  \def BOXING_STACK_ALLOCATE_TYPE_ARRAY(type, count) memory.h
 *  \brief Allocates memory in the stack for the array with specified type.
 *
 *  Allocates memory in the stack for the array with specified type.
 *
 *  \param[in]  type   Type.
 *  \param[in]  count  Number of array elements.
 */


//----------------------------------------------------------------------------
/*!
 *  \def BOXING_NULL_POINTER memory.h
 *  \brief Define null pointer value.
 *
 *  Define null pointer value.
 */


// PUBLIC MEMORY FUNCTIONS
//

//----------------------------------------------------------------------------
/*!
 *  \brief Allocates a buffer of continious memory.
 *
 *  Allocates a buffer of continious memory.
 *
 *  \param[in]  size  Size of memory buffer to allocate in bytes.
 */

void * boxing_memory_allocate(size_t size)
{
    return malloc(size);
}


//----------------------------------------------------------------------------
/*!
 *  \brief Allocates and clears a buffer of continious memory.
 *
 *  Allocates and clears (set all bytes to 0) a buffer of continious memory.
 *
 *  \param[in]  size  Size of memory buffer to allocate in bytes.
 */

void * boxing_memory_allocate_and_clear( size_t size )
{
    void* buffer = malloc( size );
    if ( buffer != NULL )
    {
        memset( buffer, 0, size );
    }
    return buffer;
}


//----------------------------------------------------------------------------
/*!
 *  \brief Free memory.
 *
 *  Frees the memory for a given pointer.
 *
 *  \param[in]  pointer_to_memory  A pointer to freed memory.
 */

void boxing_memory_free(void * pointer_to_memory)
{
    if (pointer_to_memory != NULL)
    {
        free(pointer_to_memory);
    }
}


//----------------------------------------------------------------------------
/*!
 *  \brief Clears continious memory for a given pointer.
 *
 *  Clears (set all bytes to 0) continious memory for a given pointer.
 *
 *  \param[in]  pointer_to_memory   A pointer to cleared memory.
 *  \param[in]  size_in_bytes       Size of cleared memory.
 */

void boxing_memory_clear(void * pointer_to_memory, size_t size_in_bytes)
{
    if (pointer_to_memory != NULL && size_in_bytes != 0)
    {
        memset(pointer_to_memory, 0, size_in_bytes);
    }
}


//----------------------------------------------------------------------------
/*!
 *  \brief Copied data from source memory area to destination memory area with a given size.
 *
 *  Copied data from source memory area to destination memory area with a given size.
 *
 *  \param[in]  pointer_to_memory_destination  A pointer to a destination memory area.
 *  \param[in]  pointer_to_memory_source       A pointer to a source memory area.
 *  \param[in]  size_in_bytes                  Size of copied memory.
 */

void boxing_memory_copy(void * pointer_to_memory_destination, const void * pointer_to_memory_source, size_t size_in_bytes)
{
    if (pointer_to_memory_destination != NULL && pointer_to_memory_source != NULL && size_in_bytes != 0)
    {
        memcpy(pointer_to_memory_destination, pointer_to_memory_source, size_in_bytes);
    }
}

//----------------------------------------------------------------------------
/*!
  * \} end of platform group
  */
