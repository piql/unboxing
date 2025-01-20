#ifndef BOXING_VIEWPORT_H
#define BOXING_VIEWPORT_H

/*****************************************************************************
**
**  Definition of the viewport C interface
**
**  Creation date:  2018/11/20
**  Created by:     Piql AS
**
**
**  Copyright (c) 2018 Piql AS. All rights reserved.
**
**  This file is part of the boxing library
**
*****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

//  PROJECT INCLUDES
//

//  DEFINE
//

//  TYPES
//
typedef struct boxing_viewport_s boxing_viewport;


//  FUNCTIONS
//

boxing_viewport * boxing_viewport_create(void *buffer, int width, int height, int scanline, int element_size);
boxing_viewport * boxing_viewport_clone(const boxing_viewport *vp);
void              boxing_viewport_reset(boxing_viewport *vp);
void              boxing_viewport_set_view(boxing_viewport *vp, int width, int height, int x_offset, int y_offset);
void *            boxing_viewport_scanline(const boxing_viewport *vp, int height);
int               boxing_viewport_scanline_size(const boxing_viewport *vp);
int               boxing_viewport_element_size(const boxing_viewport *vp);
void              boxing_viewport_free(boxing_viewport *vp);
int               boxing_viewport_width(const boxing_viewport *vp);
int               boxing_viewport_height(const boxing_viewport *vp);
int               boxing_viewport_offset_x(const boxing_viewport *vp);
int               boxing_viewport_offset_y(const boxing_viewport *vp);
int               boxing_viewport_buffer_width(const boxing_viewport *vp);
int               boxing_viewport_buffer_height(const boxing_viewport *vp);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
