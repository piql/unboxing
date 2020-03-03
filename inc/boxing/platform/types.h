#ifndef BOXING_TYPES_H
#define BOXING_TYPES_H

/*****************************************************************************
**
**  Definition of platform spesific data types
**
**  Creation date:  2016/06/22
**  Created by:     Ole Liabo
**
**
**  Copyright (c) 2016 Piql AS. All rights reserved.
**
**  This file is part of the boxing library
**
*****************************************************************************/

//  PLATFORM
//
#if defined( linux ) || defined( __linux ) || defined( __linux__ ) || defined( __APPLE_CC__ ) || defined( __CYGWIN__ ) 
#   define  D_OS_LINUX
#   define  BOXING_USE_C99_LIBRARIES

#elif defined( _WIN32 )
#   define  D_OS_WIN32
#   define  BOXING_USE_C89_LIBRARIES

#endif


//  PLATFORM DEPENDENT TYPES
//

#if defined BOXING_USE_C99_LIBRARIES
#   include <inttypes.h>
#   include <stddef.h>
#elif defined BOXING_USE_C89_LIBRARIES
#   include <stdint.h>
#   if !defined (__cplusplus)
#       define  inline __inline
#   endif
#else
    typedef unsigned int     size_t;
    typedef char             int8_t;
    typedef short int        int16_t;
    typedef int              int32_t;
    typedef long             int64_t;
    typedef unsigned int8_t  uint8_t;
    typedef unsigned int16_t uint16_t;
    typedef unsigned int32_t uint32_t;
    typedef unsigned int64_t uint64_t;
#endif


//  PLATFORM INDEPENDENT TYPES
//

typedef int8_t      boxing_int8;
typedef int16_t     boxing_int16;
typedef int32_t     boxing_int32;
typedef int64_t     boxing_int64;

typedef uint8_t     boxing_uint8;
typedef uint16_t    boxing_uint16;
typedef uint32_t    boxing_uint32;
typedef uint64_t    boxing_uint64;


//  CHAR
//
#define BITS_PER_CHAR  8


//  FLOATING POINT 
//

typedef float  boxing_float;
typedef double boxing_double;

#if !defined (BOXING_USE_C99_LIBRARIES) && !defined (BOXING_USE_C89_LIBRARIES)

#   define BOXING_FLOAT_MAX    3.402823e+38
#   define BOXING_MATH_EPSILON 1.192093e-07

#else
#   define _USE_MATH_DEFINES
#   include <math.h> 
#   include <float.h>

#   define BOXING_FLOAT_MAX   FLT_MAX
#   define BOXING_MATH_EPSILON DBL_EPSILON
#endif

#if !defined (M_PI)
#   define BOXING_MATH_PI 3.1415926535897932384626433832795028841968
#else
#   define BOXING_MATH_PI M_PI
#endif

#endif
