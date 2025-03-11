/*****************************************************************************
**
**  Implementation of the matrix interface
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
#include "boxing/matrix.h"
#include "boxing/log.h"


/*! 
  * \addtogroup unbox
  * \{
  */


//----------------------------------------------------------------------------
/*!
 *  \def MATRIX_MULTIPAGE_ROW_PTR(matrix, row, page) matrix.h
 *  \brief Get matrix row.
 *
 *  Returns the pointer of a multi-matrix row defined by the page number and row number.
 *
 *  \param[in] matrix  Pointer to the matrix with float elements.
 *  \param[in] row     Required row number.
 *  \param[in] page    Required multi-matrix page number.
 */


//----------------------------------------------------------------------------
/*!
 *  \def MATRIX_MULTIPAGE_PAGE_PTR(matrix, page) matrix.h
 *  \brief Get multi-matrix page.
 *
 *  Returns the pointer of a multi-matrix page defined by the page number.
 *
 *  \param[in] matrix  Pointer to the matrix with float elements.
 *  \param[in] page    Required multi-matrix page number.
 */


//----------------------------------------------------------------------------
/*!
 *  \def MATRIX_ELEMENT(matrix, x, y) matrix.h
 *  \brief Get matrix element.
 *
 *  Returns the value of the matrix element specified by its coordinates in the matrix.
 *
 *  \param[in] matrix  Pointer to the matrix.
 *  \param[in] x       Required column number.
 *  \param[in] y       Required row number.
 */


//----------------------------------------------------------------------------
/*!
 *  \def MATRIX_PELEMENT(matrix, x, y) matrix.h
 *  \brief Get pointer to matrix element.
 *
 *  Returns the pointer of the matrix element specified by its coordinates in the matrix.
 *
 *  \param[in] matrix  Pointer to the matrix.
 *  \param[in] x       Required column number.
 *  \param[in] y       Required row number.
 */


//----------------------------------------------------------------------------
/*!
 *  \def MATRIX_ROW(matrix, y) matrix.h
 *  \brief Get pointer to matrix row.
 *
 *  Returns the pointer of a matrix row defined by row number.
 *
 *  \param[in] matrix  Pointer to the matrix with float elements.
 *  \param[in] y       Required row number.
 */


//----------------------------------------------------------------------------
/*!
 *  \struct     boxing_matrix_float_s  matrix.h
 *  \brief      Multimatrix with float elements.
 *  
 *  \param cols            Number of columns.
 *  \param rows            Number of rows.
 *  \param pages           Number of matrices.
 *  \param is_owning_data  Is owning data sign.
 *  \param data            Multimatrix data pointer.
 *
 *  Structure for storing multimatrix with float elements data.
 */


//----------------------------------------------------------------------------
/*!
 *  \struct     boxing_matrixf_s  matrix.h
 *  \brief      Matrix with float elements.
 *  
 *  \param width           Number of columns.
 *  \param height          Number of rows.
 *  \param is_owning_data  Is owning data sign.
 *  \param data            Matrix data pointer.
 *
 *  Structure for storing matrix with float elements data.
 */


//----------------------------------------------------------------------------
/*!
 *  \struct     boxing_matrixi_s  matrix.h
 *  \brief      Matrix with integer elements.
 *  
 *  \param width           Number of columns.
 *  \param height          Number of rows.
 *  \param is_owning_data  Is owning data sign.
 *  \param data            Matrix data pointer.
 *
 *  Structure for storing matrix with integer elements data.
 */


// PUBLIC MATRIX FUNCTIONS
//

//----------------------------------------------------------------------------
/*!
 *  \brief Create a multipage matrix with float elements.
 *
 *  Create multipage matrix.
 *
 *  \param[in] rows   Number of rows.
 *  \param[in] cols   Number of columns.
 *  \param[in] pages  Number of pages.
 *  \return instance of allocated multipage matrix with float elements.
 */

boxing_matrix_float *boxing_matrix_float_multipage_create(unsigned int rows, unsigned int cols, unsigned int pages)
{
    if (cols == 0 || rows == 0)
    {
        return NULL;
    }

    boxing_matrix_float *matrix = malloc(sizeof(boxing_matrix_float));
    matrix->cols = cols;
    matrix->rows = rows;
    matrix->pages = pages;
    matrix->data = calloc(cols * rows * pages, sizeof(boxing_float));
    matrix->is_owning_data = DTRUE;
    if (matrix->data == NULL && cols * rows * pages != 0)
    {
        free(matrix);
        DLOG_ERROR("(boxing_matrix_float_multipage_create) Failed to allocate matrix");
        return NULL;
    }
    return matrix;
}


//----------------------------------------------------------------------------
/*!
 *  \brief Frees occupied memory of multipage matrix with float elements.
 *
 *  Frees occupied memory of multipage matrix with float elements.
 *  If the data does not belong to this structure, then the memory allocated for this data is not released.
 *
 *  \param[in]  matrix  Pointer to the multipage matrix structure.
 */

void boxing_matrix_float_free(boxing_matrix_float *matrix)
{
    if (matrix != NULL)
    {
        if (matrix->is_owning_data)
        {
            free(matrix->data);
        }
        free(matrix);
    }
}


//----------------------------------------------------------------------------
/*!
 *  \brief Create a matrix with float elements.
 *
 *  Allocate memory for the matrix with float elements
 *  according to the input width and height values.
 *  Return instance of allocated matrix structure.
 *
 *  \param[in] width   Number of columns.
 *  \param[in] height  Number of rows.
 *  \return instance of allocated matrix with float elements.
 */

boxing_matrixf *boxing_matrixf_create(unsigned int width, unsigned int height)
{
    if (width == 0 || height == 0)
    {
        return NULL;
    }

    boxing_matrixf *matrix = malloc(sizeof(boxing_matrixf));
    matrix->width = width;
    matrix->height = height;
    matrix->data = calloc(width * height, sizeof(boxing_pointf));
    matrix->is_owning_data = DTRUE;
    if (matrix->data == NULL)
    {
        free(matrix);
        DLOG_ERROR("(boxing_matrixf_create) Failed to allocate matrix");
        return NULL;
    }
    return matrix;
}


//----------------------------------------------------------------------------
/*!
 *  \brief Create a matrix with integer elements.
 *
 *  Allocate memory for the matrix with integer elements
 *  according to the input width and height values.
 *  Return instance of allocated matrix structure.
 *
 *  \param[in] width   Number of columns.
 *  \param[in] height  Number of rows.
 *  \return instance of allocated matrix with integer elements.
 */

boxing_matrixi *boxing_matrixi_create(unsigned int width, unsigned int height)
{
    if (width == 0 || height == 0)
    {
        return NULL;
    }

    boxing_matrixi *matrix = malloc(sizeof(boxing_matrixi));
    matrix->width = width;
    matrix->height = height;
    matrix->data = calloc(width * height, sizeof(boxing_pointi));
    matrix->is_owning_data = DTRUE;
    if (matrix->data == NULL)
    {
        free(matrix);
        DLOG_ERROR("(boxing_matrixi_create) Failed to allocate matrix");
        return NULL;
    }
    return matrix;
}


//----------------------------------------------------------------------------
/*!
 *  \brief Recreate a matrix with float elements.
 *
 *  Allocate new memory for the input matrix with float elements
 *  according to the input width and height values.
 *  The memory allocated for the source data is freed.
 *  Return instance of allocated matrix structure with new sizes.
 *
 *  \param[in] matrix   Pointer to the matrix with float elements.
 *  \param[in] width    Number of columns.
 *  \param[in] height   Number of rows.
 *  \return instance of reallocated matrix with float elements.
 */

boxing_matrixf *boxing_matrixf_recreate(boxing_matrixf *matrix, unsigned int width, unsigned int height)
{
    if (width == 0 || height == 0)
    {
        return NULL;
    }

    if (matrix == NULL)
    {
        return boxing_matrixf_create(width, height);
    }
    else
    {
        if (matrix->width == width && matrix->height == height && matrix->data != NULL && matrix->is_owning_data)
        {
            return matrix;
        }
        else
        {
            if (matrix->is_owning_data && matrix->data != NULL)
            {
                free(matrix->data);
                matrix->data = NULL;
            }
            boxing_matrixf_init_in_place(matrix, width, height);
            return matrix;
        }
    }
}


//----------------------------------------------------------------------------
/*!
 *  \brief Recreate a matrix with integer elements.
 *
 *  Allocate new memory for the input matrix with integer elements
 *  according to the input width and height values.
 *  The memory allocated for the source data is freed.
 *  Return instance of allocated matrix structure with new sizes.
 *
 *  \param[in] matrix   Pointer to the matrix with integer elements.
 *  \param[in] width    Number of columns.
 *  \param[in] height   Number of rows.
 *  \return instance of reallocated matrix with integer elements.
 */

boxing_matrixi *boxing_matrixi_recreate(boxing_matrixi *matrix, unsigned int width, unsigned int height)
{
    if (width == 0 || height == 0)
    {
        return NULL;
    }

    if (matrix == NULL)
    {
        return boxing_matrixi_create(width, height);
    }
    else
    {
        if (matrix->width == width && matrix->height == height && matrix->data != NULL && matrix->is_owning_data)
        {
            return matrix;
        }
        else
        {
            if (matrix->is_owning_data && matrix->data != NULL)
            {
                free(matrix->data);
                matrix->data = NULL;
            }
            boxing_matrixi_init_in_place(matrix, width, height);
            return matrix;
        }
    }
}


//----------------------------------------------------------------------------
/*!
 *  \brief Initialize matrix.
 *
 *  Allocate new memory for the input matrix data with float elements
 *  according to the input width and height values.
 *  The memory allocated for the source matrix data does not freed.
 *
 *  \param[in] matrix   Pointer to the matrix with float elements.
 *  \param[in] width    Number of columns.
 *  \param[in] height   Number of rows.
 *  \return instance of matrix with reallocated data array.
 */

void boxing_matrixf_init_in_place(boxing_matrixf *matrix, unsigned int width, unsigned int height)
{
    if (matrix == NULL || width == 0 || height == 0)
    {
        return;
    }

    matrix->is_owning_data = DTRUE;
    matrix->width = width;
    matrix->height = height;
    matrix->data = calloc(width * height, sizeof(boxing_pointf));
    if (matrix->data == NULL)
    {
        boxing_throw("out of memory");
    }
}


//----------------------------------------------------------------------------
/*!
 *  \brief Initialize matrix.
 *
 *  Allocate new memory for the input matrix data with integer elements
 *  according to the input width and height values.
 *  The memory allocated for the source matrix data does not freed.
 *
 *  \param[in] matrix   Pointer to the matrix with integer elements.
 *  \param[in] width    Number of columns.
 *  \param[in] height   Number of rows.
 *  \return instance of matrix with reallocated data array.
 */

void boxing_matrixi_init_in_place(boxing_matrixi *matrix, unsigned int width, unsigned int height)
{
    if (matrix == NULL || width == 0 || height == 0)
    {
        return;
    }

    matrix->is_owning_data = DTRUE;
    matrix->width = width;
    matrix->height = height;
    matrix->data = calloc(width * height, sizeof(boxing_pointi));
    if (matrix->data == NULL)
    {
        boxing_throw("out of memory");
    }
}


//----------------------------------------------------------------------------
/*!
 *  \brief Copy matrix.
 *
 *  Function create a new copy of input matrix with float elements and return it.
 *  If matrix pointer is NULL function return NULL.
 *
 *  \param[in]  matrix  Pointer to the matrix with float elements.
 *  \return new copy of matrix with float elements or NULL.
 */

boxing_matrixf *boxing_matrixf_copy(const boxing_matrixf *matrix)
{
    if (matrix == NULL)
    {
        return NULL;
    }

    boxing_matrixf *copy = boxing_matrixf_create(matrix->width, matrix->height);

    if (matrix->data == NULL)
    {
        free(copy->data);
        copy->data = NULL;
    }
    else
    {
        memcpy(copy->data, matrix->data, matrix->width * matrix->height * sizeof(boxing_pointf));
    }
    
    return copy;
}


//----------------------------------------------------------------------------
/*!
 *  \brief Copy matrix.
 *
 *  Function create a new copy of input matrix with integer elements and return it.
 *  If matrix pointer is NULL function return NULL.
 *
 *  \param[in]  matrix  Pointer to the matrix with integer elements.
 *  \return new copy of matrix with integer elements or NULL.
 */

boxing_matrixi *boxing_matrixi_copy(const boxing_matrixi *matrix)
{
    if (matrix == NULL)
    {
        return NULL;
    }

    boxing_matrixi *copy = boxing_matrixi_create(matrix->width, matrix->height);

    if (matrix->data == NULL)
    {
        free(copy->data);
        copy->data = NULL;
    }
    else
    {
        memcpy(copy->data, matrix->data, matrix->width * matrix->height * sizeof(boxing_pointi));
    }

    return copy;
}


//----------------------------------------------------------------------------
/*!
 *  \brief Frees matrix.
 *
 *  Frees occupied memory of matrix with float elements.
 *  If the data does not belong to this structure, then the memory allocated for this data is not released.
 *
 *  \param[in]  matrix  Pointer to the matrix structure.
 */

void boxing_matrixf_free(boxing_matrixf *matrix)
{
    if (matrix != NULL)
    {
        if (matrix->is_owning_data)
        {
            free(matrix->data);
        }
        free(matrix);
    }
}


//----------------------------------------------------------------------------
/*!
 *  \brief Frees matrix.
 *
 *  Frees occupied memory of matrix with integer elements.
 *  If the data does not belong to this structure, then the memory allocated for this data is not released.
 *
 *  \param[in]  matrix  Pointer to the matrix structure.
 */

void boxing_matrixi_free(boxing_matrixi *matrix)
{
    if (matrix != NULL)
    {
        if (matrix->is_owning_data)
        {
            free(matrix->data);
        }
        free(matrix);
    }
}


//----------------------------------------------------------------------------
/*!
  * \} end of unbox group
  */
