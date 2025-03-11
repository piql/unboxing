/*****************************************************************************
**
**  Implementation of the utility interface
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
#include "boxing/utils.h"
#include "ghash.h"
#include "g_variant.h"


/*! 
  * \addtogroup unbox
  * \{
  */


//----------------------------------------------------------------------------
/*!
 *  \def BOXING_VIRTUAL(p, method) utils.h
 *  \brief Calls a function without parameters in the specified structure.
 *
 *  Calls a function without parameters "method" in the structure "p".
 *
 *  \param[in]  p       Structure name.
 *  \param[in]  method  Function name.
 */


//----------------------------------------------------------------------------
/*!
 *  \def BOXING_VIRTUALP(p, method, param) utils.h
 *  \brief Calls a function with one parameter in the specified structure.
 *
 *  Calls a function "method" with one parameter "param" in the structure "p".
 *
 *  \param[in]  p       Structure name.
 *  \param[in]  method  Function name.
 *  \param[in]  param   Function parameter.
 */


//----------------------------------------------------------------------------
/*!
 *  \def BOXING_VIRTUALP2(p, method, param1, param2) utils.h
 *  \brief Calls a function with two parameters in the specified structure.
 *
 *  Calls a function "method" with two parameters "param1" and "param2" in the structure "p".
 *
 *  \param[in]  p       Structure name.
 *  \param[in]  method  Function name.
 *  \param[in]  param1  First function parameter.
 *  \param[in]  param2  Second function parameter.
 */


//----------------------------------------------------------------------------
/*!
 *  \def BOXING_VIRTUAL2(p, method1, method2) utils.h
 *  \brief Calls a function without parameters from the structure located in the structure.
 *
 *  Calls a function "method2" without parameters in the structure which is obtained after function "method1" located in the structure "p".
 *
 *  \param[in]  p        Structure name.
 *  \param[in]  method1  First method.
 *  \param[in]  method2  Second method.
 */


//----------------------------------------------------------------------------
/*!
 *  \def BOXING_VIRTUAL2p(p, method1, method2, param) utils.h
 *  \brief Calls a function with one parameter from the structure located in the structure.
 *
 *  Calls a function "method2" with one parameter "param" in the structure which is obtained after function "method1" located in the structure "p".
 *
 *  \param[in]  p        Structure name.
 *  \param[in]  method1  First method.
 *  \param[in]  method2  Second method.
 *  \param[in]  param    Function parameter.
 */


//----------------------------------------------------------------------------
/*!
 *  \def BOXING_VIRTUAL2p2(p, method1, method2, param1, param2) utils.h
 *  \brief Calls a function with two parametesr from the structure located in the structure.
 *
 *  Calls a function "method2" with two parameters "param1" and "param2" in the structure which is obtained after function "method1" located in the structure "p".
 *
 *  \param[in]  p        Structure name.
 *  \param[in]  method1  First method.
 *  \param[in]  method2  Second method.
 *  \param[in]  param1   First function parameter.
 *  \param[in]  param2   Second function parameter.
 */


//----------------------------------------------------------------------------
/*!
 *  \def BOXING_UNUSED_PARAMETER(parameter) utils.h
 *  \brief Macro for unused function parameters.
 *
 *  Macro for unused function parameters. Used to eliminate the compiler warning.
 *
 *  \param[in]  parameter  Unused function parameter.
 */


// PUBLIC UTILS FUNCTIONS
//

//----------------------------------------------------------------------------
/*!
 *  \brief Destroy g hash table string item.
 *
 *  Destroy g hash table string item function description.
 *
 *  \param[in] data  String item.
 */

void boxing_utils_g_hash_table_destroy_item_string(void *data)
{
    free(data);
}


//----------------------------------------------------------------------------
/*!
 *  \brief Destroy g hash table ghash item.
 *
 *  Destroy g hash table ghash item function description.
 *
 *  \param[in] data  Ghash item.
 */

void boxing_utils_g_hash_table_destroy_item_ghash(void *data)
{
    g_hash_table_destroy(data);
}


//----------------------------------------------------------------------------
/*!
 *  \brief Destroy g hash table Gvariant item.
 *
 *  Destroy g hash table Gvariant item function description.
 *
 *  \param[in] data  Gvariant item.
 */

void boxing_utils_g_hash_table_destroy_item_g_variant(void *data)
{
    g_variant_free(data);
}


//----------------------------------------------------------------------------
/*!
  * \} end of unbox group
  */
