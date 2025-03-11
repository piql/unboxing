/*****************************************************************************
**
**  Implementation of the synchronisation point interface
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
#include "boxing/unboxer/syncpoints.h"
#include "boxing/utils.h"
#include "boxing/log.h"

//  DEFINES
//

typedef struct rectanglef_s
{
    boxing_pointf top_left;
    boxing_pointf top_right;
    boxing_pointf bot_left;
    boxing_pointf bot_right;
} rectanglef;

//  PRIVATE INTERFACE
//

static void correct_symbol_location_matrix_with_sync_points(boxing_matrixf *initial_symbol_location_matrix, const boxing_matrixi *syncpoint_locations_source, const boxing_matrixf *syncpoint_location_scan);
static void correct_symbol_location_matrix_area(boxing_matrixf * initial_symbol_location_matrix, const boxing_recti *ref_area_src, const rectanglef *offset, DBOOL correct_right_border, DBOOL correct_bottom_border);
static boxing_pointf determine_precise_syncpoint_position_in_scan_image(const boxing_pointf syncpoint_positions_in_scan_image, const boxing_image8 * scan_image, boxing_float search_radius_scan_image, DBOOL is_center_point_bright);
static boxing_matrixf * determine_syncpoints(const boxing_matrixf * syncpoint_positions_in_scan_image_from_syncbars, const boxing_image8 * scan_image, boxing_float search_radius_scan_image, DBOOL is_center_point_bright);
static boxing_matrixf * correct_syncpoint_offsets(const boxing_matrixf *sync_point_offsets, boxing_float max_allowed_sqr_offset_variation);

// PUBLIC SYNC POINTS FUNCTIONS
//

//---------------------------------------------------------------------------- 
/*!
 *  \brief GFP 1 default implementation of boxing_syncpoints_correct_frame_geometry_cb.
 *
 *  This function uses the sync points at given locations to compensate for
 *  any geometrical distortions not detected by the general tracking mechanism.
 *
 *  \ingroup unboxer
 *  \param[in,out] user                        Pointer to the user data.
 *  \param[in]     frame                       Super sampled frame
 *  \param[in,out] symbol_location_matrix NxM  matrix with coordinates to data symbols within a frame
 *  \param[in]     sync_point_location_index   KxL matrix with indexs to each sync point location found in the symbol_location_matrix
 *  \param[in]     properties                  Function properties
 *
 *  \return Return optmized NxM location matrix
 */

boxing_matrixf * boxing_syncpoints_correct_frame_geometry(void * user, const boxing_image8 * frame, boxing_matrixf * symbol_location_matrix, 
    const boxing_matrixi * sync_point_location_index, const correct_frame_geometry_properties * properties)
{
    BOXING_UNUSED_PARAMETER( user );

    // Determine the locations of the sync points from the symbol_location_matrix
    boxing_matrixf sync_point_locations_from_raster_locations;
    boxing_matrixf_init_in_place(&sync_point_locations_from_raster_locations, sync_point_location_index->width, sync_point_location_index->height);
    for (unsigned int iy = 0; iy < sync_point_location_index->height; iy++)
    {
        for (unsigned int ix = 0; ix < sync_point_location_index->width; ix++)
        {
            boxing_pointi src_location = MATRIX_ELEMENT(sync_point_location_index, iy, ix);
            boxing_pointf pos = MATRIX_ELEMENT(symbol_location_matrix, src_location.y, src_location.x);
            MATRIX_ELEMENT(&sync_point_locations_from_raster_locations, iy, ix) = pos;
        }
    }

    // Determine the locations of the sync points from the scanned image by searching the centers of the sync points

    // Calculate offset
    boxing_matrixf * sync_point_offsets = determine_syncpoints( &sync_point_locations_from_raster_locations, frame, properties->sync_point_search_radius, properties->sync_point_center_is_bright );

    int use_rows = BOXING_MATH_MIN(sync_point_offsets->height, sync_point_locations_from_raster_locations.height);
    int use_cols = BOXING_MATH_MIN(sync_point_offsets->width, sync_point_locations_from_raster_locations.width);
    for (int iy = 0; iy < use_rows; iy++)
    {
        for (int ix = 0; ix < use_cols; ix++)
        {
            boxing_pointf *to = MATRIX_PELEMENT(sync_point_offsets, iy, ix);
            boxing_pointf from = MATRIX_ELEMENT(&sync_point_locations_from_raster_locations, iy, ix);
            to->x -= from.x;
            to->y -= from.y;
        }
    }

    // Correct sync point offsets
    boxing_matrixf * sync_point_offsets_corrected = correct_syncpoint_offsets(sync_point_offsets, properties->sync_point_max_allowed_variation);

    boxing_matrixf_free(sync_point_offsets);
    sync_point_offsets = NULL;

    // Correct sync points
    use_rows = BOXING_MATH_MIN(sync_point_locations_from_raster_locations.height, sync_point_offsets_corrected->height);
    use_cols = BOXING_MATH_MIN(sync_point_locations_from_raster_locations.width, sync_point_offsets_corrected->width);
    for (int iy = 0; iy < use_rows; iy++)
    {
        for (int ix = 0; ix < use_cols; ix++)
        {
            boxing_pointf *to = MATRIX_PELEMENT(&sync_point_locations_from_raster_locations, iy, ix);
            boxing_pointf from = MATRIX_ELEMENT(sync_point_offsets_corrected, iy, ix);
            to->x += from.x;
            to->y += from.y;
        }
    }

    boxing_matrixf_free(sync_point_offsets_corrected);
    sync_point_offsets_corrected = NULL;

    // Correct location matrix
    correct_symbol_location_matrix_with_sync_points(symbol_location_matrix, sync_point_location_index, &sync_point_locations_from_raster_locations);

    free(sync_point_locations_from_raster_locations.data);

    return symbol_location_matrix;
}


// PRIVATE SYNC POINTS FUNCTIONS
//

/**
 * The symbol location matrix for all symbols in the frame are corrected using the given sync points.
 * \param initial_symbol_location_matrix The symbol location matrix that is modified
 * \param syncpoint_locations_source The locations of the sync points in the (not supersampled) source frame
 * \param syncpoint_location_scan The sync point locations in the scanned image
 */
static void correct_symbol_location_matrix_with_sync_points(boxing_matrixf *initial_symbol_location_matrix, const boxing_matrixi *syncpoint_locations_source, const boxing_matrixf *syncpoint_location_scan)
{
    // Remember the locations of the sync points as given in the initial_symbol_location_matrix
    boxing_matrixf sync_point_locations_scan_initial;
    boxing_matrixf_init_in_place(&sync_point_locations_scan_initial, syncpoint_locations_source->width, syncpoint_locations_source->height);
    for (unsigned int iy = 0; iy < syncpoint_locations_source->height; iy++)
    {
        for (unsigned int ix = 0; ix < syncpoint_locations_source->width; ix++)
        {
            boxing_pointi loc = MATRIX_ELEMENT(syncpoint_locations_source, iy, ix);
            MATRIX_ELEMENT(&sync_point_locations_scan_initial, iy, ix) = MATRIX_ELEMENT(initial_symbol_location_matrix, loc.y, loc.x);
        }
    }

    int width_src = initial_symbol_location_matrix->width;
    int height_src = initial_symbol_location_matrix->height;
    int num_sync_points_h = syncpoint_locations_source->width;
    int num_sync_points_v = syncpoint_locations_source->height;

    // Calculate locations for the main area
    for (unsigned int i_sync_point_y = 0; i_sync_point_y < syncpoint_location_scan->height - 1; i_sync_point_y++)
    {
        for (unsigned int i_sync_point_x = 0; i_sync_point_x < syncpoint_location_scan->width - 1; i_sync_point_x++)
        {
            boxing_pointi pointi1 = MATRIX_ELEMENT(syncpoint_locations_source, i_sync_point_y, i_sync_point_x);
            boxing_pointi pointi2 = MATRIX_ELEMENT(syncpoint_locations_source, i_sync_point_y + 1, i_sync_point_x + 1);
            boxing_recti ref_area_src = {pointi1.x, pointi1.y, pointi2.x - pointi1.x + 1, pointi2.y - pointi1.y + 1};

            boxing_pointf ref_point_scan_top_left = MATRIX_ELEMENT(syncpoint_location_scan, i_sync_point_y, i_sync_point_x);
            boxing_pointf ref_point_scan_top_right = MATRIX_ELEMENT(syncpoint_location_scan, i_sync_point_y, i_sync_point_x + 1);
            boxing_pointf ref_point_scan_bot_left = MATRIX_ELEMENT(syncpoint_location_scan, i_sync_point_y + 1, i_sync_point_x);
            boxing_pointf ref_point_scan_bot_right = MATRIX_ELEMENT(syncpoint_location_scan, i_sync_point_y + 1, i_sync_point_x + 1);

            rectanglef offset;
            offset.top_right = MATRIX_ELEMENT(&sync_point_locations_scan_initial, i_sync_point_y, i_sync_point_x + 1);
            offset.top_right.x -= ref_point_scan_top_right.x;
            offset.top_right.y -= ref_point_scan_top_right.y;

            offset.bot_right = MATRIX_ELEMENT(&sync_point_locations_scan_initial, i_sync_point_y + 1, i_sync_point_x + 1);
            offset.bot_right.x -= ref_point_scan_bot_right.x;
            offset.bot_right.y -= ref_point_scan_bot_right.y;

            offset.top_left = MATRIX_ELEMENT(&sync_point_locations_scan_initial, i_sync_point_y, i_sync_point_x);
            offset.top_left.x -= ref_point_scan_top_left.x;
            offset.top_left.y -= ref_point_scan_top_left.y;

            offset.bot_left = MATRIX_ELEMENT(&sync_point_locations_scan_initial, i_sync_point_y + 1, i_sync_point_x);
            offset.bot_left.x -= ref_point_scan_bot_left.x;
            offset.bot_left.y -= ref_point_scan_bot_left.y;

            correct_symbol_location_matrix_area(initial_symbol_location_matrix, &ref_area_src, &offset, DFALSE, DFALSE);
        }
    }

    // Calculate locations for the left border
    if (MATRIX_ELEMENT(syncpoint_locations_source, 0, 0).x != 0)
    {
        for (int i_sync_point_y = 0; i_sync_point_y < num_sync_points_v - 1; i_sync_point_y++)
        {
            boxing_pointi pointi1 = { 0, MATRIX_ELEMENT(syncpoint_locations_source, i_sync_point_y, 0).y };
            boxing_pointi pointi2 = MATRIX_ELEMENT(syncpoint_locations_source, i_sync_point_y + 1, 0);
            boxing_recti ref_area_src = {pointi1.x, pointi1.y, pointi2.x - pointi1.x + 1, pointi2.y - pointi1.y + 1};

            boxing_pointf ref_point_scan_top_right = MATRIX_ELEMENT(syncpoint_location_scan, i_sync_point_y, 0);
            boxing_pointf ref_point_scan_bot_right = MATRIX_ELEMENT(syncpoint_location_scan, i_sync_point_y + 1, 0);

            rectanglef offset;
            offset.top_right = MATRIX_ELEMENT(&sync_point_locations_scan_initial, i_sync_point_y, 0);
            offset.top_right.x -= ref_point_scan_top_right.x;
            offset.top_right.y -= ref_point_scan_top_right.y;

            offset.bot_right = MATRIX_ELEMENT(&sync_point_locations_scan_initial, i_sync_point_y + 1, 0);
            offset.bot_right.x -= ref_point_scan_bot_right.x;
            offset.bot_right.y -= ref_point_scan_bot_right.y;

            offset.top_left.x = offset.top_right.x;
            offset.top_left.y = 0;
            offset.bot_left.x = offset.bot_right.x;
            offset.bot_left.y = 0;

            correct_symbol_location_matrix_area(initial_symbol_location_matrix, &ref_area_src, &offset, DFALSE, DFALSE);
        }
    }

    // Calculate locations for the right border
    if (MATRIX_ELEMENT(syncpoint_locations_source, 0, num_sync_points_h - 1).x != width_src - 1)
    {
        for (int i_sync_point_y = 0; i_sync_point_y < num_sync_points_v - 1; i_sync_point_y++)
        {
            boxing_pointi pointi1 = MATRIX_ELEMENT(syncpoint_locations_source, i_sync_point_y, num_sync_points_h - 1);
            boxing_pointi pointi2 = { width_src - 1, MATRIX_ELEMENT(syncpoint_locations_source, i_sync_point_y + 1, num_sync_points_h - 1).y };
            boxing_recti ref_area_src = {pointi1.x, pointi1.y, pointi2.x - pointi1.x + 1, pointi2.y - pointi1.y + 1};

            boxing_pointf ref_point_scan_top_left = MATRIX_ELEMENT(syncpoint_location_scan, i_sync_point_y, num_sync_points_h - 1);
            boxing_pointf ref_point_scan_bot_left = MATRIX_ELEMENT(syncpoint_location_scan, i_sync_point_y + 1, num_sync_points_h - 1);

            rectanglef offset;
            offset.top_left = MATRIX_ELEMENT(&sync_point_locations_scan_initial, i_sync_point_y, num_sync_points_h - 1);
            offset.top_left.x -= ref_point_scan_top_left.x;
            offset.top_left.y -= ref_point_scan_top_left.y;

            offset.bot_left = MATRIX_ELEMENT(&sync_point_locations_scan_initial, i_sync_point_y + 1, num_sync_points_h - 1);
            offset.bot_left.x -= ref_point_scan_bot_left.x;
            offset.bot_left.y -= ref_point_scan_bot_left.y;

            offset.top_right.x = offset.top_left.x;
            offset.top_right.y = 0;
            offset.bot_right.x = offset.bot_left.x;
            offset.bot_right.y = 0;

            correct_symbol_location_matrix_area(initial_symbol_location_matrix, &ref_area_src, &offset, DTRUE, DFALSE);
        }
    }

    // Calculate locations for the top border
    if (MATRIX_ELEMENT(syncpoint_locations_source, 0, 0).y != 0)
    {
        for (int i_sync_point_x = 0; i_sync_point_x < num_sync_points_h - 1; i_sync_point_x++)
        {
            boxing_pointi pointi1 = { MATRIX_ELEMENT(syncpoint_locations_source, 0, i_sync_point_x).x, 0 };
            boxing_pointi pointi2 = MATRIX_ELEMENT(syncpoint_locations_source, 0, i_sync_point_x + 1);
            boxing_recti ref_area_src = {pointi1.x, pointi1.y, pointi2.x - pointi1.x + 1, pointi2.y - pointi1.y + 1};

            boxing_pointf ref_point_scan_bot_left = MATRIX_ELEMENT(syncpoint_location_scan, 0, i_sync_point_x);
            boxing_pointf ref_point_scan_bot_right = MATRIX_ELEMENT(syncpoint_location_scan, 0, i_sync_point_x + 1);

            rectanglef offset;
            offset.bot_left = MATRIX_ELEMENT(&sync_point_locations_scan_initial, 0, i_sync_point_x);
            offset.bot_left.x -= ref_point_scan_bot_left.x;
            offset.bot_left.y -= ref_point_scan_bot_left.y;

            offset.bot_right = MATRIX_ELEMENT(&sync_point_locations_scan_initial, 0, i_sync_point_x + 1);
            offset.bot_right.x -= ref_point_scan_bot_right.x;
            offset.bot_right.y -= ref_point_scan_bot_right.y;

            offset.top_left.x = 0;
            offset.top_left.y = offset.bot_left.y;
            offset.top_right.x = 0;
            offset.top_right.y = offset.bot_right.y;

            correct_symbol_location_matrix_area(initial_symbol_location_matrix, &ref_area_src, &offset, DFALSE, DFALSE);
        }
    }

    // Calculate locations for the bottom border
    if (MATRIX_ELEMENT(syncpoint_locations_source, num_sync_points_v - 1, 0).y != height_src - 1)
    {
        for (int i_sync_point_x = 0; i_sync_point_x < num_sync_points_h - 1; i_sync_point_x++)
        {
            boxing_pointi pointi1 = MATRIX_ELEMENT(syncpoint_locations_source, num_sync_points_v - 1, i_sync_point_x);
            boxing_pointi pointi2 = { MATRIX_ELEMENT(syncpoint_locations_source, num_sync_points_v - 1, i_sync_point_x + 1).x, height_src - 1 };
            boxing_recti ref_area_src = {pointi1.x, pointi1.y, pointi2.x - pointi1.x + 1, pointi2.y - pointi1.y + 1};

            boxing_pointf ref_point_scan_top_left = MATRIX_ELEMENT(syncpoint_location_scan, num_sync_points_v - 1, i_sync_point_x);
            boxing_pointf ref_point_scan_top_right = MATRIX_ELEMENT(syncpoint_location_scan, num_sync_points_v - 1, i_sync_point_x + 1);

            rectanglef offset;
            offset.top_left = MATRIX_ELEMENT(&sync_point_locations_scan_initial, num_sync_points_v - 1, i_sync_point_x);
            offset.top_left.x -= ref_point_scan_top_left.x;
            offset.top_left.y -= ref_point_scan_top_left.y;

            offset.top_right = MATRIX_ELEMENT(&sync_point_locations_scan_initial, num_sync_points_v - 1, i_sync_point_x + 1);
            offset.top_right.x -= ref_point_scan_top_right.x;
            offset.top_right.y -= ref_point_scan_top_right.y;

            offset.bot_left.x = 0;
            offset.bot_left.y = offset.top_left.y;
            offset.bot_right.x = 0;
            offset.bot_right.y = offset.top_right.y;

            correct_symbol_location_matrix_area(initial_symbol_location_matrix, &ref_area_src, &offset, DFALSE, DTRUE);
        }
    }

    // Calculate locations for the top left corner
    if (syncpoint_locations_source->data->x != 0 || syncpoint_locations_source->data->y != 0)
    {
        boxing_pointi pointi1 = { 0, 0 };
        boxing_pointi pointi2 = MATRIX_ELEMENT(syncpoint_locations_source, 0, 0);
        boxing_recti ref_area_src = {pointi1.x, pointi1.y, pointi2.x - pointi1.x + 1, pointi2.y - pointi1.y + 1};

        boxing_pointf ref_point_scan_bot_right = MATRIX_ELEMENT(syncpoint_location_scan, 0, 0);

        rectanglef offset;
        offset.bot_right = MATRIX_ELEMENT(&sync_point_locations_scan_initial, 0, 0);
        offset.bot_right.x -= ref_point_scan_bot_right.x;
        offset.bot_right.y -= ref_point_scan_bot_right.y;

        offset.top_right.x = 0;
        offset.top_right.y = offset.bot_right.y;
        offset.top_left.x = offset.top_left.y = 0;
        offset.bot_left.x = offset.bot_right.x;
        offset.bot_left.y = 0;

        correct_symbol_location_matrix_area(initial_symbol_location_matrix, &ref_area_src, &offset, DFALSE, DFALSE);
    }

    // Calculate locations for the top right corner
    boxing_pointi pointi = MATRIX_ELEMENT(syncpoint_locations_source, 0, num_sync_points_h - 1);
    if (pointi.y != 0 || pointi.x != width_src - 1)
    {
        boxing_pointi pointi1 = { MATRIX_ELEMENT(syncpoint_locations_source, 0, num_sync_points_h - 1).x, 0 };
        boxing_pointi pointi2 = { width_src - 1, MATRIX_ELEMENT(syncpoint_locations_source, 0, num_sync_points_h - 1).y };
        boxing_recti ref_area_src = {pointi1.x, pointi1.y, pointi2.x - pointi1.x + 1, pointi2.y - pointi1.y + 1};

        boxing_pointf ref_point_scan_bot_left = MATRIX_ELEMENT(syncpoint_location_scan, 0, num_sync_points_h - 1);

        rectanglef offset;
        offset.bot_left = MATRIX_ELEMENT(&sync_point_locations_scan_initial, 0, num_sync_points_h - 1);
        offset.bot_left.x -= ref_point_scan_bot_left.x;
        offset.bot_left.y -= ref_point_scan_bot_left.y;

        offset.top_right.x = 0;
        offset.top_right.y = 0;
        offset.bot_right.x = offset.bot_left.x;
        offset.bot_right.y = 0;
        offset.top_left.x = 0;
        offset.top_left.y = offset.bot_left.y;

        correct_symbol_location_matrix_area(initial_symbol_location_matrix, &ref_area_src, &offset, DTRUE, DFALSE);
    }

    // Calculate locations for the bottom left corner
    pointi = MATRIX_ELEMENT(syncpoint_locations_source, num_sync_points_v - 1, 0);
    if (pointi.x != 0 || pointi.y != height_src - 1)
    {
        boxing_pointi pointi1 = { 0, MATRIX_ELEMENT(syncpoint_locations_source, num_sync_points_v - 1, 0).y };
        boxing_pointi pointi2 = { MATRIX_ELEMENT(syncpoint_locations_source, num_sync_points_v - 1, 0).x, height_src - 1 };
        boxing_recti ref_area_src = {pointi1.x, pointi1.y, pointi2.x - pointi1.x + 1, pointi2.y - pointi1.y + 1};

        boxing_pointf ref_point_scan_top_right = MATRIX_ELEMENT(syncpoint_location_scan, num_sync_points_v - 1, 0);

        rectanglef offset;
        offset.top_right = MATRIX_ELEMENT(&sync_point_locations_scan_initial, num_sync_points_v - 1, 0);
        offset.top_right.x -= ref_point_scan_top_right.x;
        offset.top_right.y -= ref_point_scan_top_right.y;

        offset.bot_left.x = offset.bot_left.y = 0;
        offset.bot_right.x = 0.0f;
        offset.bot_right.y = offset.top_right.y;
        offset.top_left.x = offset.top_right.x;
        offset.top_left.y = 0;

        correct_symbol_location_matrix_area(initial_symbol_location_matrix, &ref_area_src, &offset, DFALSE, DTRUE);
    }

    // Calculate locations for the bottom right corner
    pointi = MATRIX_ELEMENT(syncpoint_locations_source, num_sync_points_v - 1, num_sync_points_h - 1);
    if (pointi.x != width_src - 1 || pointi.y != height_src - 1)
    {
        boxing_pointi pointi1 = MATRIX_ELEMENT(syncpoint_locations_source, num_sync_points_v - 1, num_sync_points_h - 1);
        boxing_pointi pointi2 = { width_src - 1, height_src - 1 };
        boxing_recti ref_area_src = {pointi1.x, pointi1.y, pointi2.x - pointi1.x + 1, pointi2.y - pointi1.y + 1};

        boxing_pointf ref_point_scan_top_left = MATRIX_ELEMENT(syncpoint_location_scan, num_sync_points_v - 1, num_sync_points_h - 1);

        rectanglef offset;
        offset.top_left = MATRIX_ELEMENT(&sync_point_locations_scan_initial, num_sync_points_v - 1, num_sync_points_h - 1);
        offset.top_left.x -= ref_point_scan_top_left.x;
        offset.top_left.y -= ref_point_scan_top_left.y;

        offset.bot_right.x = offset.bot_right.y = 0;
        offset.bot_left.x = 0;
        offset.bot_left.y = offset.top_left.y;
        offset.top_right.x = offset.top_left.x;
        offset.top_right.y = 0;

        correct_symbol_location_matrix_area(initial_symbol_location_matrix, &ref_area_src, &offset, DTRUE, DTRUE);
    }

    free(sync_point_locations_scan_initial.data);
}


/**
 * An area of the symbol location matrix is corrected.
 * \param initial_symbol_location_matrix The symbol location matrix that is modified
 * \param ref_area_src The area given in coordinates of the source image
 * \param offset The offsets that shall be applied
 * \param correct_right_border if DTRUE, also the pixels on the right border are corrected
 * \param correct_bottom_border if DTRUE, also the pixels on the bottom border are corrected
 */
static void correct_symbol_location_matrix_area(boxing_matrixf * initial_symbol_location_matrix, const boxing_recti *ref_area_src, const rectanglef *offset, DBOOL correct_right_border, DBOOL correct_bottom_border)
{
    unsigned int rect_width = ref_area_src->width;
    unsigned int rect_height = ref_area_src->height;
    if (correct_right_border == DFALSE) rect_width--;
    if (correct_bottom_border == DFALSE) rect_height--;

    int ref_area_src_y = ref_area_src->y;
    int ref_area_src_x = ref_area_src->x;

    if ( ref_area_src_y < 0 || (ref_area_src_y + rect_height) > initial_symbol_location_matrix->height ||
         ref_area_src_x < 0 || (ref_area_src_x + rect_width) > initial_symbol_location_matrix->width )
    {
        DLOG_FATAL4( "correct_symbol_location_matrix_area: ref_area_src out of bounds [(%d, %d), (%d,%d)]", ref_area_src_x, ref_area_src_y, rect_width, rect_height );
        return;
    }

    boxing_float rectangle_width_f = (boxing_float)rect_width;
    boxing_float rectangle_height_f = (boxing_float)rect_height;

    boxing_pointf offset_top_width = offset->top_right;
    offset_top_width.x -= offset->top_left.x;
    offset_top_width.y -= offset->top_left.y;

    boxing_pointf offset_bot_width = offset->bot_right;
    offset_bot_width.x -= offset->bot_left.x;
    offset_bot_width.y -= offset->bot_left.y;

    for (unsigned int iy = 0; iy < rect_height; iy++)
    {
        boxing_float fraction_y = (boxing_float)iy / rectangle_height_f;
        unsigned int y = ref_area_src_y + iy;

        boxing_float fraction_x = 0;
        boxing_float fraction_x_add = (boxing_float)1.0 / rectangle_width_f;
        for (unsigned int ix = 0; ix < rect_width; ix++)
        {
            boxing_pointf offset_top;
            offset_top.x = offset->top_left.x + offset_top_width.x * fraction_x;
            offset_top.y = offset->top_left.y + offset_top_width.y * fraction_x;

            boxing_pointf offset_bot;
            offset_bot.x = offset->bot_left.x + offset_bot_width.x * fraction_x;
            offset_bot.y = offset->bot_left.y + offset_bot_width.y * fraction_x;

            boxing_pointf offset_final;
            offset_final.x = offset_top.x + (offset_bot.x - offset_top.x) * fraction_y;
            offset_final.y = offset_top.y + (offset_bot.y - offset_top.y) * fraction_y;

            boxing_pointf *element = MATRIX_PELEMENT(initial_symbol_location_matrix, y, ref_area_src_x + ix);
            element->x -= offset_final.x;
            element->y -= offset_final.y;
            fraction_x += fraction_x_add;
        }
    }
}


/**
 * Determine the locations of the sync points in the scanned frame. This is done by searching for
 * sync points at the assumed positions derived from the sync bars.
 * \param syncpoint_positions_in_scan_image_from_syncbars The positions of the sync point within the scanned image
 * \param scan_image The scanned image
 * \param search_radius_scan_image The search radius (horizontal and vertical around the assumed position of a sync point) where a sync point is searched
 * \param is_center_point_bright DTRUE if the center of the sync point is bright, DFALSE if not
 * \return A matrix with all found sync point positions
 */
static boxing_matrixf * determine_syncpoints(const boxing_matrixf * syncpoint_positions_in_scan_image_from_syncbars, const boxing_image8 * scan_image, boxing_float search_radius_scan_image, DBOOL is_center_point_bright)
{
    boxing_matrixf * result = boxing_matrixf_create(syncpoint_positions_in_scan_image_from_syncbars->width, syncpoint_positions_in_scan_image_from_syncbars->height);

    for (unsigned int iy = 0; iy < syncpoint_positions_in_scan_image_from_syncbars->height; iy++)
    {
        for (unsigned int ix = 0; ix < syncpoint_positions_in_scan_image_from_syncbars->width; ix++)
        {
            boxing_pointf sync_point_position_from_sync_bar = MATRIX_ELEMENT(syncpoint_positions_in_scan_image_from_syncbars, iy, ix);
            boxing_pointf found_position = determine_precise_syncpoint_position_in_scan_image(sync_point_position_from_sync_bar, scan_image, search_radius_scan_image, is_center_point_bright);
            MATRIX_ELEMENT(result, iy, ix) = found_position;
        }
    }

    return result;
}

/**
 * The position of one sync point is determined from the information in the scanned image
 * \param syncpoint_positions_in_scan_image The assumed position of the sync point in the scanned image
 * \param scan_image The scanned image
 * \param search_radius_scan_image The radius for searching the sync point
 * \param is_center_point_bright DTRUE if the center of the sync point is bright, DFALSE if not
 * \return The found position of the sync point
 */
static boxing_pointf determine_precise_syncpoint_position_in_scan_image(const boxing_pointf syncpoint_positions_in_scan_image, const boxing_image8 * scan_image, boxing_float search_radius_scan_image, DBOOL is_center_point_bright)
{
    // Define the area where searching shall be performed
    boxing_recti check_area_scan = {lround(syncpoint_positions_in_scan_image.x - search_radius_scan_image), lround(syncpoint_positions_in_scan_image.y - search_radius_scan_image), lround(2.0*search_radius_scan_image), lround(2.0*search_radius_scan_image)};

    // Find the minimum and maximum gray value in that area
    int min_gray = IMAGE8_PIXEL(scan_image, check_area_scan.x, check_area_scan.y);
    int max_gray = min_gray;
    for (int iy = 0; iy < check_area_scan.height; iy++) {
        for (int ix = 0; ix < check_area_scan.width; ix++) {
            int gray_value = IMAGE8_PIXEL(scan_image, check_area_scan.x + ix, check_area_scan.y + iy);
            if (gray_value < min_gray) {
                min_gray = gray_value;
            }
            else if (gray_value > max_gray) {
                max_gray = gray_value;
            }
        }
    }


    // Take grey values above middle between min and max and calculate gravity
    boxing_pointf gravity_point;
    {
        int sum_prod_x = 0;
        int sum_prod_y = 0;
        int sum_gray = 0;

        if (is_center_point_bright == DTRUE) {
            //int gray_threshold = min_gray + (max_gray - min_gray) / 5;
            int gray_threshold = min_gray;
            for (int iy = check_area_scan.y; iy <= check_area_scan.y + check_area_scan.height; iy++) {
                for (int ix = check_area_scan.x; ix <= check_area_scan.x + check_area_scan.width; ix++) {
                    int gray_value = IMAGE8_PIXEL(scan_image, ix, iy);
                    if (gray_value > gray_threshold) {
                        int gray_used = gray_value - gray_threshold;
                        sum_prod_x += ix * gray_used;
                        sum_prod_y += iy * gray_used;
                        sum_gray += gray_used;
                    }
                }
            }
        }
        else {
            //int gray_threshold = max_gray - (max_gray - min_gray) / 5;
            int gray_threshold = max_gray;
            for (int iy = check_area_scan.y; iy <= check_area_scan.y + check_area_scan.height; iy++) {
                for (int ix = check_area_scan.x; ix <= check_area_scan.x + check_area_scan.width; ix++) {
                    int gray_value = IMAGE8_PIXEL(scan_image, ix, iy);
                    if (gray_value < gray_threshold) {
                        int gray_used = gray_threshold - gray_value;
                        sum_prod_x += ix * gray_used;
                        sum_prod_y += iy * gray_used;
                        sum_gray += gray_used;
                    }
                }
            }
        }

        if (!sum_gray)
        {
            /* No valid sync point location where found,
               returning inital location instead
               */
            return syncpoint_positions_in_scan_image;
        }

        gravity_point.x = (boxing_float)sum_prod_x / sum_gray;
        gravity_point.y = (boxing_float)sum_prod_y / sum_gray;
    }

    return gravity_point;
}


/**
 * The sync point offsets (containing the offset between the syncpoint positions calculated from the scanned image and the positions
 * calculated from the sync bars) are corrected - if necessary: If single offsets are found that are completely different to its
 * neighboring sync points, they are replaced by a mean value of their neighbors.
 * \param sync_point_offsets The sync point offsets calculated before
 * \param max_allowed_sqr_offset_variation The maximum allowed variation of the offset (square)
 * \return The corrected sync point offsets
 */
static boxing_matrixf * correct_syncpoint_offsets(const boxing_matrixf *sync_point_offsets, boxing_float max_allowed_sqr_offset_variation)
{
    boxing_matrixf * result = boxing_matrixf_create(sync_point_offsets->width, sync_point_offsets->height);

    for (int iy=0; (unsigned int)iy<sync_point_offsets->height; iy++) {
        for ( int ix = 0; (unsigned int)ix<sync_point_offsets->width; ix++ ) {
            // Calculate the (square) difference to the the neighboring offsets
            boxing_float diff_offset_sqr = 0.0f;
            {
                boxing_float offset_mean_x = 0.0f;
                boxing_float offset_mean_y = 0.0f;
                int start_x = BOXING_MATH_MAX(ix-2,0);
                int end_x = BOXING_MATH_MIN(ix+2,(int)sync_point_offsets->width-1);
                int start_y = BOXING_MATH_MAX(iy-2,0);
                int end_y = BOXING_MATH_MIN(iy+2,(int)sync_point_offsets->height-1);
                int count = 0;
                for (int iiy=start_y;iiy<=end_y;iiy++) {
                    for (int iix=start_x;iix<=end_x;iix++) {
                        if (iiy != iy || iix != ix) {
                            offset_mean_x += MATRIX_ELEMENT(sync_point_offsets, iiy,iix).x;
                            offset_mean_y += MATRIX_ELEMENT(sync_point_offsets, iiy,iix).y;
                            count++;
                        }
                    }
                }
                offset_mean_x = offset_mean_x / (boxing_float)count;
                offset_mean_y = offset_mean_y / (boxing_float)count;
                boxing_float diff_offset_x = fabsf(MATRIX_ELEMENT(sync_point_offsets, iy, ix).x - offset_mean_x);
                boxing_float diff_offset_y = fabsf(MATRIX_ELEMENT(sync_point_offsets, iy, ix).y - offset_mean_y);
                diff_offset_sqr = diff_offset_x*diff_offset_x + diff_offset_y*diff_offset_y;
            }

            // Correct the offset if necessary
            if (diff_offset_sqr > max_allowed_sqr_offset_variation) {
                boxing_pointf use_offset = {0.0f,0.0f};
                int count = 0;
                if (ix > 0) {
                    const boxing_pointf offset = MATRIX_ELEMENT(sync_point_offsets, iy,ix-1);
                    use_offset.x += offset.x;
                    use_offset.y += offset.y;
                    count++;
                }
                if (ix < (int)sync_point_offsets->width-1) {
                    const boxing_pointf offset = MATRIX_ELEMENT(sync_point_offsets, iy,ix+1);
                    use_offset.x += offset.x;
                    use_offset.y += offset.y;
                    count++;
                }
                if (iy > 0) {
                    const boxing_pointf offset = MATRIX_ELEMENT(sync_point_offsets, iy-1,ix);
                    use_offset.x += offset.x;
                    use_offset.y += offset.y;
                    count++;
                }
                if (iy < (int)sync_point_offsets->height-1) {
                    const boxing_pointf offset = MATRIX_ELEMENT(sync_point_offsets, iy+1,ix);
                    use_offset.x += offset.x;
                    use_offset.y += offset.y;
                    count++;
                }
                use_offset.x /= (boxing_float)count;
                use_offset.y /= (boxing_float)count;
                MATRIX_ELEMENT(result, iy,ix) = use_offset;
            } else {
                MATRIX_ELEMENT(result, iy,ix) = MATRIX_ELEMENT(sync_point_offsets, iy,ix);
            }
        }
    }

    return result;
}
