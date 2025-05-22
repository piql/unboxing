#ifndef BOXING_STRING_H
#define BOXING_STRING_H

/*****************************************************************************
**
**  Definition of the string interface
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
#include "gvector.h"
#include "boxing/bool.h"

char    *boxing_string_clone(const char *string);
gvector *boxing_string_split(const char *string, const char *separator);
DBOOL    boxing_string_to_integer(int *value, const char *string);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
