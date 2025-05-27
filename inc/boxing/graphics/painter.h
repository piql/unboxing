#ifndef BOXING_PAINTER_H
#define BOXING_PAINTER_H

/*****************************************************************************
**
**  Definition of the painter struct
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
#include "boxing/graphics/paintdevice.h"


//============================================================================
//  STRUCT:  boxing_painter
typedef struct boxing_painter_s
{
    void (*draw_line)(struct boxing_painter_s * painter, int x1, int y1, int x2, int y2, const int color);
    void (*draw_rect)(struct boxing_painter_s * painter, int x, int y, int width, int height, const int color);
    void (*fill_rect)(struct boxing_painter_s * painter, int x, int y, int width, int height, const int color);
    void (*draw_image)(struct boxing_painter_s *painter, int x, int y, const char * image, int scan_line_width, int sx, int sy, int sw, int sh);
    int  (*width)(struct boxing_painter_s * painter);
    int  (*height)(struct boxing_painter_s * painter);

    boxing_recti         region;
    boxing_paintdevice * device;
} boxing_painter;

boxing_painter * boxing_painter_create(void);
void             boxing_painter_free(boxing_painter * painter);

void boxing_painter_init(boxing_painter * painter, boxing_paintdevice * device);
void boxing_painter_clip(boxing_painter * copy_to, const boxing_painter * copy_from, const boxing_recti * region);

#ifdef __cplusplus
} /* extern "C" */
#endif

//============================================================================


#endif
