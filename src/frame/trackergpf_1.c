/*****************************************************************************
**
**  Implementation of the dframetrackergpf_1 interface
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

//  PROJECT INCLUDES
//
#include    "boxing/frame/trackergpf_1.h"
#include    "boxing/graphics/genericframegpf_1.h"
#include    "boxing/unboxer/frametrackerutil.h"
#include    "boxing/graphics/referencepoint.h"
#include    "boxing/graphics/referencebar.h"
#include    "boxing/graphics/calibrationbar.h"
#include    "boxing/unboxer/syncpoints.h"
#include    "2polinomialsampler.h"
#include    "areasampler.h"
#include    "boxing/unboxer/horizontalmeasures.h"
#include    "../unboxer/datapoints.h"

//  BASE INCLUDES
//
#include    "boxing/log.h"

//  SYSTEM INCLUDES
//
#include    <math.h>

//  DEFINES
//

#define SMEMBER(member) (((boxing_tracker_gpf_1 *)tracker)->member)
#define BASEMEMBER(member) (((boxing_tracker_gpf *)tracker)->member)
#define BASEBASEMEMBER(member) (((boxing_tracker*)tracker)->member)
#define REFBAR_INVALID_SYNC_REF -1

typedef struct {
    int actual;
    int expected;
} sync_location;


//  PRIVATE INTERFACE
//

static int   track_frame(boxing_tracker * tracker, const boxing_image8 * frame);
static void  tracker_gpf_1_free(boxing_tracker * tracker);
static int   validate_coordinate(int width, int w_margin, int height, int h_margin, struct boxing_pointi_s *location);
static int   track_frame_analog_mode(boxing_tracker_gpf * tracker, const boxing_image8 * input_image);
static void  boxing_coordmapper_init_empty(boxing_coordmapper *mapper);
static void  get_displacement_matrix(boxing_tracker_gpf_1 * tracker, const boxing_image8 * image, boxing_matrixf * displacement_matrix);
static DBOOL calc_horizontal_offset(boxing_tracker_gpf_1 * tracker, const boxing_image8 * image, const boxing_pointi from, const boxing_pointi to, boxing_float * offsets, int scan_direction);
static DBOOL calculate_reference_bars(boxing_tracker_gpf_1 * tracker, const boxing_image8 * image);

static DBOOL calculate_intersection_points(
    const boxing_linef * horizontal_lines, 
    const boxing_linef * vertical_lines,
    int lines_size_h,
    int lines_size_v,
    const boxing_float * offsets,
    const boxing_float vertical_barwidth,
    const boxing_float width,
    boxing_matrixf * intersections_points);

static boxing_linef * calculate_lines(
    const boxing_pointf * start_points,
    const int start_points_size,
    const boxing_pointf * end_points,
    const int end_points_size,
    boxing_linef * lines);


/*! 
  * \addtogroup frame
  * \{
  */


/*! 
 *  \var typedef boxing_matrixf* (*boxing_correct_frame_geometry_cb)
 *               (void* user, const boxing_image8 * image, boxing_matrixf * symbol_location_matrix,
 *               const boxing_matrixi *sync_point_location_index,
 *               const correct_frame_geometry_properties * properties)
 *  \brief Uses the sync points at given locations to compensate for
 *         any geometrical distortions not detected by the general tracking mechanism.
 *
 *  \param[in,out]  user                       Pointer to the user data.
 *  \param[in]      image                      Image.
 *  \param[in,out]  symbol_location_matrix     NxM matrix with coordinates to data symbols within a frame.
 *  \param[in]      sync_point_location_index  KxL matrix with indexs to each sync point location found in the symbol_location_matrix.
 *  \param[in]      properties                 Function properties.
 *   
 *  The input image is a super sampled frame and the symbol_location_matrix contains the
 *  location of frame's data symbols. Special sync symbols are embedded in the data container
 *  to detect and compensate for geometrical distortions within the super sampled frame.
 *  Each element in the sync_point_location_index represents sync symbol where the index refers
 *  an element in the location_matrix in which holds the locatation sync symbol. By searching
 *  for sync symbols in the proximity of the alleged position given by the index_matrix one can
 *  determine how much the location matrix is off tracking and compesate for it.
 *
 *  Note: The symbol_location_matrix parameter is modified and finally returned.
 */


/*! 
 *  \var typedef DBOOL( *boxing_calculate_sampling_locations_cb )
 *  (void* user, struct boxing_tracker_gpf_1_s * tracker,
 *   const boxing_pointf * left_bar_points, const int left_bar_points_size,
 *   const boxing_pointf * right_bar_points, const int right_bar_points_size,
 *   const boxing_pointf * top_bar_points, const int top_bar_points_size,
 *   const boxing_pointf * bottom_bar_points, const int bottom_bar_points_size,
 *   const boxing_float * left_offset_xs, const boxing_float * right_offset_xs, int offsets_size,
 *   int width, int vertical_barwidth,
 *   const frame_corner_marks* corner_marks)
 *  \brief Calculate sampling location callback function.
 *
 *  \param[in,out]  user                    Pointer to the user data.
 *  \param[in]      tracker                 Tracker.
 *  \param[in]      left_bar_points         Left bar points.
 *  \param[in]      left_bar_points_size    Left bar points size.
 *  \param[in]      right_bar_points        Right bar points.
 *  \param[in]      right_bar_points_size   Right bar points size.
 *  \param[in]      top_bar_points          Top bar points.
 *  \param[in]      top_bar_points_size     Top bar points size.
 *  \param[in]      bottom_bar_points       Bottom bar points.
 *  \param[in]      bottom_bar_points_size  Bottom bar points size.
 *  \param[in]      left_offset_xs          Left offset along the X axis.
 *  \param[in]      right_offset_xs         Right offset along the X axis.
 *  \param[in]      offsets_size            Offset size.
 *  \param[in]      width                   Width.
 *  \param[in]      vertical_barwidth       Vertical bar width.
 *  \param[in]      corner_marks            Corner marks.
 *   
 *  Sampling location callback function description.
 */


/*! 
 *  \var typedef DBOOL( *boxing_track_vertical_shift_cb )
 *  (void* user, struct boxing_tracker_gpf_1_s* tracker,
 *   const boxing_image8* input_image);
 *
 *  \brief Calculate track vertical shift callback function.
 *
 *  \param[in,out]  user         Pointer to the user data.
 *  \param[in]      tracker      Tracker.
 *  \param[in]      input_image  Image.
 *   
 *  Calculate track vertical shift callback function description.
 */


//----------------------------------------------------------------------------
/*!
 *  \struct     correct_frame_geometry_properties_s  frametrackercbgpf_1.h
 *  \brief      Frame geometry properties data storage.
 *  
 *  \param sync_point_search_radius          Scan radius in pixels, default = 4.5f.
 *  \param sync_point_center_is_bright       Is center pixel white, default = DTRUE.
 *  \param sync_point_max_allowed_variation  Max allowed squared offset variation, default = 0.1f.
 *
 *  The correct_frame_geometry_properties serve as input parameters to the correct_frame_geometry function
 *  which purpose is to find syncpoints and validate their positions.
 */


 //----------------------------------------------------------------------------
/*!
 *  \struct     boxing_coordmapper_s  frametrackergpf_1.h
 *  \brief      Coordination mapper data storage.
 *  
 *  \param vector_top           Top vector.
 *  \param vector_bottom        Bottom vector.
 *  \param vector_left          Left vector.
 *  \param top_left             Top left point.
 *  \param top_left_offset      Top left offrset.
 *  \param horizontal_distance  Horizontal distance.
 *  \param vertical_distance    Vertical distance.
 *
 *  Coordination mapper structure description.
 */


//----------------------------------------------------------------------------
/*!
 *  \struct     boxing_tracker_gpf_1_s  frametrackergpf_1.h
 *  \brief      Boxing tracker gpf 1 data storage.
 *  
 *  \param base                              boxing_tracker_gpf base structure instance.
 *  \param frame_dimension                   Frame dimension.
 *  \param corner_mark_dimension             Corner mark dimension.
 *  \param corner_mark_gap_size              Corner mark gap size.
 *  \param avg_max                           Avg max.
 *  \param avg_min                           Avg min.
 *  \param left_reference_bar_sampler        Left reference bar sampler.
 *  \param right_reference_bar_sampler       Right reference bar sampler.
 *  \param top_reference_bar_sampler         Top reference bar sampler.
 *  \param bottom_reference_bar_sampler      Bottom reference bar sampler.
 *  \param top_left_corner_mark_sampler      Top left corner mark sampler.
 *  \param top_right_corner_mark_sampler     Top right corner mark sampler.
 *  \param bottom_left_corner_mark_sampler   Bottom left corner mark sampler.
 *  \param bottom_right_corner_mark_sampler  Bottom right corner mark sampler.
 *  \param syncpoint_index                   Syncpoint index.
 *  \param tile_size                         Tile size.
 *  \param frame                             Frame.
 *  \param horizontal_distance               Horizontal distance.
 *  \param vertical_distance                 Vertical distance.
 *  \param x_offset                          Offset along the X axis.
 *  \param y_offset                          Offset along the Y axis.
 *  \param coordinate_mapper                 Coordinate mapper.
 *  \param calculate_sampling_location       Calculate sampling location callback function. (boxing_calculate_sampling_locations_cb)
 *  \param track_vertical_shift              Calculate track vertical shift callback function. (boxing_track_vertical_shift_cb)
 *  \param correct_frame_geometry            Calculate correct frame geometry callback function. (boxing_correct_frame_geometry_cb)
 *
 *  boxing_tracker_gpf_1_s structure description.
 */


// PUBLIC FRAME TRACKER GPF 1 FUNCTIONS
//

//----------------------------------------------------------------------------
/*!
 *  \brief Create a new boxing_tracker_gpf_1 structure instance.
 *
 *  Allocate memory for the boxing_frame_gpf_1 type
 *  and initializes all structure data accoding to generic frame data.
 *  Return instance of allocated structure.
 *
 *  \param[in] generic_frame  Pointer to the boxing_frame_gpf_1 structure instance.
 *  \return instance of allocated boxing_tracker_gpf_1 structure.
 */

boxing_tracker_gpf_1 * boxing_tracker_gpf_1_create(boxing_frame_gpf_1 * generic_frame)
{
    boxing_tracker_gpf_1 * tracker = malloc(sizeof(boxing_frame_gpf_1));
    boxing_tracker_gpf_init(&tracker->base, (boxing_frame*)generic_frame);
    tracker->base.base.mode = BOXING_TRACK_HORIZONTAL_SHIFT|
                              BOXING_TRACK_CONTENT_CONTAINER|
                              BOXING_TRACK_METADATA_CONTAINER|
                              BOXING_TRACK_CALIBRATION_BAR|
                              BOXING_TRACK_SYNC_POINTS;
    tracker->base.base.track_frame = track_frame;
    tracker->base.base.free = tracker_gpf_1_free;
    tracker->base.base.type = BOXING_TRACKER_GPF1_NAME;
    tracker->base.track_frame_analog_mode = track_frame_analog_mode;
    tracker->frame_dimension = generic_frame->base.size(&generic_frame->base);
    tracker->frame = generic_frame;
    tracker->corner_mark_dimension = boxing_generic_frame_gpf_1_corner_mark_dimension(generic_frame);
    tracker->corner_mark_gap_size = boxing_generic_frame_gpf_1_corner_mark_gap_size(generic_frame);
    tracker->calculate_sampling_location = boxing_calculate_sampling_location;
    tracker->track_vertical_shift = boxing_track_vertical_shift;
    tracker->correct_frame_geometry = boxing_syncpoints_correct_frame_geometry;

    boxing_generic_container * container = tracker->base.generic_frame->metadata_container(tracker->base.generic_frame);
    boxing_pointi tile_size          = container->get_tile_size(container);
    boxing_pointi metadata_dimension = {container->dimension(container).x/tile_size.x,
            container->dimension(container).y/tile_size.y};

    container = tracker->base.generic_frame->container(tracker->base.generic_frame);
    boxing_pointi container_tile_size = container->get_tile_size(container);
    boxing_pointi container_dimension1 = {container->dimension(container).x/container_tile_size.x,
            container->dimension(container).y/container_tile_size.y};

    boxing_coordmapper_init_empty(&tracker->coordinate_mapper);
    

    boxing_pointi container_dimension = generic_frame->base.container(&generic_frame->base)->dimension(generic_frame->base.container(&generic_frame->base));
    container_dimension.x /= generic_frame->reference_bar_freq_divider;
    container_dimension.y /= generic_frame->reference_bar_freq_divider;

    tracker->left_reference_bar_sampler = boxing_2polinomialsampler_create(container_dimension.y, 1);
    tracker->right_reference_bar_sampler = boxing_2polinomialsampler_create(container_dimension.y, 1);
    tracker->top_reference_bar_sampler = boxing_2polinomialsampler_create(container_dimension.x,  1);
    tracker->bottom_reference_bar_sampler = boxing_2polinomialsampler_create(container_dimension.x,  1);

    tracker->top_left_corner_mark_sampler = boxing_2polinomialsampler_create(1, 1);
    tracker->top_right_corner_mark_sampler = boxing_2polinomialsampler_create(1, 1);
    tracker->bottom_left_corner_mark_sampler = boxing_2polinomialsampler_create(1, 1);
    tracker->bottom_right_corner_mark_sampler = boxing_2polinomialsampler_create(1, 1);

    tracker->base.content_sampler          = boxing_2polinomialsampler_create(container_dimension1.x, container_dimension1.y);
    tracker->base.metadata_sampler         = boxing_areasampler_create(metadata_dimension.x, metadata_dimension.y);
    tracker->base.calibration_bar_sampler  = boxing_areasampler_create(1, 256);


    g_hash_table_replace(tracker->base.base.container_sampler_list, boxing_string_clone(BOXING_SAMPLE_CONTAINER_CONTENT),  tracker->base.content_sampler);
    g_hash_table_replace(tracker->base.base.container_sampler_list, boxing_string_clone(BOXING_SAMPLE_CONTAINER_METADATA), tracker->base.metadata_sampler);
    g_hash_table_replace(tracker->base.base.container_sampler_list, boxing_string_clone(BOXING_SAMPLE_BAR_CALIBRATION),    tracker->base.calibration_bar_sampler);

    size_t strlen_bar = boxing_string_length(BOXING_SAMPLE_BAR_REFERENCE);
    size_t strlen_marks = boxing_string_length(BOXING_SAMPLE_CORNERMARKS);
    char * strbuf = boxing_string_allocate(BOXING_MATH_MAX(strlen_bar, strlen_marks) + 20); 
    memcpy(strbuf, BOXING_SAMPLE_BAR_REFERENCE, strlen_bar);
    memcpy(strbuf + strlen_bar, ".left", boxing_string_length(".left") + 1);
    g_hash_table_replace(tracker->base.base.container_sampler_list, boxing_string_clone(strbuf),   tracker->left_reference_bar_sampler);
    memcpy(strbuf + strlen_bar, ".right", boxing_string_length(".right") + 1);
    g_hash_table_replace(tracker->base.base.container_sampler_list, boxing_string_clone(strbuf),   tracker->right_reference_bar_sampler);
    memcpy(strbuf + strlen_bar, ".top", boxing_string_length(".top") + 1);
    g_hash_table_replace(tracker->base.base.container_sampler_list, boxing_string_clone(strbuf),   tracker->top_reference_bar_sampler);
    memcpy(strbuf + strlen_bar, ".bottom", boxing_string_length(".bottom") + 1);
    g_hash_table_replace(tracker->base.base.container_sampler_list, boxing_string_clone(strbuf),   tracker->bottom_reference_bar_sampler);

    memcpy(strbuf, BOXING_SAMPLE_CORNERMARKS, strlen_marks);
    memcpy(strbuf + strlen_marks, ".top.left", boxing_string_length(".top.left") + 1);
    g_hash_table_replace(tracker->base.base.container_sampler_list, boxing_string_clone(strbuf),   tracker->top_left_corner_mark_sampler);
    memcpy(strbuf + strlen_marks, ".top.right", boxing_string_length(".top.right") + 1);
    g_hash_table_replace(tracker->base.base.container_sampler_list, boxing_string_clone(strbuf),   tracker->top_right_corner_mark_sampler);
    memcpy(strbuf + strlen_marks, ".bottom.left", boxing_string_length(".bottom.left") + 1);
    g_hash_table_replace(tracker->base.base.container_sampler_list, boxing_string_clone(strbuf),   tracker->bottom_left_corner_mark_sampler);
    memcpy(strbuf + strlen_marks, ".bottom.right", boxing_string_length(".bottom.right") + 1);
    g_hash_table_replace(tracker->base.base.container_sampler_list, boxing_string_clone(strbuf),   tracker->bottom_right_corner_mark_sampler);

    free(strbuf);
    strbuf = NULL;

    tracker->tile_size = generic_frame->base.metadata_container(&generic_frame->base)->get_tile_size(generic_frame->base.metadata_container(&generic_frame->base)).y;

    int syncpoint_offset_h;
    int num_syncpoints_h;
    if (generic_frame->sync_point_h_offset < 0)
    {
        num_syncpoints_h = (container_dimension.x - 2 * generic_frame->sync_point_radius - 1) /
            generic_frame->sync_point_h_distance + 1;
        syncpoint_offset_h = (container_dimension.x - ((num_syncpoints_h - 1)*generic_frame->sync_point_h_distance + 1)) / 2;
    }
    else
    {
        num_syncpoints_h = ((container_dimension.x - generic_frame->sync_point_h_offset) - 2 * generic_frame->sync_point_radius - 1) /
            generic_frame->sync_point_h_distance + 1;
        syncpoint_offset_h = generic_frame->sync_point_h_offset;
    }


    int syncpoint_offset_v;
    int num_syncpoints_v;
    if (generic_frame->sync_point_v_offset < 0)
    {
        num_syncpoints_v = ((container_dimension.y) - 2 * generic_frame->sync_point_radius - 1) /
            generic_frame->sync_point_v_distance + 1;
        syncpoint_offset_v = (container_dimension.y - ((num_syncpoints_v - 1)*generic_frame->sync_point_v_distance + 1)) / 2;
    }
    else
    {
        num_syncpoints_v = ((container_dimension.y - generic_frame->sync_point_v_offset) - 2 * generic_frame->sync_point_radius - 1) /
            generic_frame->sync_point_v_distance + 1;
        syncpoint_offset_v = generic_frame->sync_point_v_offset;
    }

    // Calculate sync point indices 
    boxing_matrixi_init_in_place(&tracker->syncpoint_index, num_syncpoints_h, num_syncpoints_v);  
    int index_v = syncpoint_offset_v;
    for(unsigned int row = 0; row != tracker->syncpoint_index.height; row++)
    {
        int index_h = syncpoint_offset_h;
        for(unsigned int col = 0; col < tracker->syncpoint_index.width; col++)
        {            
            MATRIX_ELEMENT(&tracker->syncpoint_index, row, col).x = index_h;
            MATRIX_ELEMENT(&tracker->syncpoint_index, row, col).y = index_v;
            index_h += generic_frame->sync_point_h_distance;
        }
        index_v += generic_frame->sync_point_v_distance;
    }

    tracker->horizontal_distance = generic_frame->top_right_reference_point->base.pos.x - generic_frame->top_left_reference_point->base.pos.x;
    tracker->vertical_distance   = generic_frame->bottom_left_reference_point->base.pos.y - generic_frame->top_left_reference_point->base.pos.y;
    tracker->x_offset = boxing_component_absolute_location((boxing_component*)generic_frame->top_left_reference_point).x + generic_frame->top_left_reference_point->base.size.x / 2;
    tracker->y_offset = boxing_component_absolute_location((boxing_component*)generic_frame->top_left_reference_point).y + generic_frame->top_left_reference_point->base.size.y / 2;
    return tracker;
}


//----------------------------------------------------------------------------
/*!
 *  \brief Calculate sampling location.
 *
 *  Calculate sampling location fuction description.
 *
 *  \param[in,out]  user                    Pointer to the user data.
 *  \param[in]      tracker                 Tracker.
 *  \param[in]      left_bar_points         Left bar points.
 *  \param[in]      left_bar_points_size    Left bar points size.
 *  \param[in]      right_bar_points        Right bar points.
 *  \param[in]      right_bar_points_size   Right bar ponts size.
 *  \param[in]      top_bar_points          Top bar points.
 *  \param[in]      top_bar_points_size     Top bar points size.
 *  \param[in]      bottom_bar_points       Bottom bar points.
 *  \param[in]      bottom_bar_points_size  Bottom bar points size.
 *  \param[in]      left_offset_xs          Left offset.
 *  \param[in]      right_offset_xs         Right offset.
 *  \param[in]      offsets_size            Offset size.
 *  \param[in]      width                   Width.
 *  \param[in]      vertical_barwidth       Vertical bar width.
 *  \param[in]      corner_marks            Corner marks.
 *  \return a sign of the success of the function.
 */

DBOOL boxing_calculate_sampling_location(
    void* user, boxing_tracker_gpf_1 * tracker,
    const boxing_pointf * left_bar_points, const int left_bar_points_size,
    const boxing_pointf * right_bar_points, const int right_bar_points_size,
    const boxing_pointf * top_bar_points, const int top_bar_points_size,
    const boxing_pointf * bottom_bar_points, const int bottom_bar_points_size,
    const boxing_float * left_offset_xs,
    const boxing_float * right_offset_xs,
    int     offsets_size,
    int     width,
    int     vertical_barwidth,
    const frame_corner_marks* corner_marks)
{
    BOXING_UNUSED_PARAMETER(user);

    if (left_bar_points_size != right_bar_points_size)
    {
        DLOG_ERROR2( "(boxing_calculate_sampling_location) Different vertical bar sizes left=%d, right=%d", left_bar_points_size, right_bar_points_size );
        return DFALSE;
    }

    if (left_bar_points_size == 0)
    {
        DLOG_ERROR("(boxing_calculate_sampling_location) No vertical points");
        return DFALSE;
    }

    if (top_bar_points_size != bottom_bar_points_size)
    {
        DLOG_ERROR2( "(boxing_calculate_sampling_location) Different horizontal bar sizes top=%d, bottom=%d", top_bar_points_size, bottom_bar_points_size );
        return DFALSE;
    }

    if (top_bar_points_size == 0)
    {
        DLOG_ERROR("(boxing_calculate_sampling_location) No horizontal points");
        return DFALSE;
    }

    // Group data for better cache performance
    boxing_float *offsets = calloc(offsets_size * 2, sizeof(boxing_float));
    boxing_float *p = offsets;

    for (int i = 0; i < offsets_size; i++)
    {
        *p = left_offset_xs[i];
        p++;
        *p = right_offset_xs[i];
        p++;
    }

    // Calculate horizontal lines
    int line_count_h = left_bar_points_size > right_bar_points_size ? right_bar_points_size : left_bar_points_size;
    boxing_linef *horizontal_lines = calloc(line_count_h, sizeof(boxing_linef));
    calculate_lines(left_bar_points, left_bar_points_size, right_bar_points, right_bar_points_size, horizontal_lines);

    // Calculate vertical lines
    int line_count_v = top_bar_points_size > bottom_bar_points_size ? bottom_bar_points_size : top_bar_points_size;
    boxing_linef *vertical_lines = calloc(line_count_v, sizeof(boxing_linef));
    calculate_lines(top_bar_points, top_bar_points_size, bottom_bar_points, bottom_bar_points_size, vertical_lines);

    DBOOL retval = DTRUE;
    retval &= calculate_intersection_points(horizontal_lines,
        vertical_lines,
        line_count_h,
        line_count_v,
        offsets,
        (boxing_float)vertical_barwidth,
        (boxing_float)width,
        &tracker->base.content_sampler->location_matrix);

    free(horizontal_lines);
    horizontal_lines = NULL;

    if (retval)
    {
        tracker->base.content_sampler->state = DTRUE;
    }

    boxing_generic_container *container = SMEMBER(frame)->base.metadata_container(&SMEMBER(frame)->base);
    boxing_pointi tile_size = container->get_tile_size(container);
    boxing_pointi metadata_container_size = container->dimension(container);
    int horizontal_tile_count = metadata_container_size.x / tile_size.x;
    int vertical_tile_count = metadata_container_size.y / tile_size.y;

    // Calculate metadata_vertical_lines lines
    boxing_linef *metadata_vertical_lines = calloc(horizontal_tile_count, sizeof(boxing_linef));

    int inc = tile_size.x / SMEMBER(frame)->reference_bar_freq_divider;
    if ((inc - 1) % 2)
    {
        int offset = (inc - 1) / 2;
        for (int i = 0; i < horizontal_tile_count; i++)
        {
            boxing_linef a = vertical_lines[offset + inc*i];
            boxing_linef b = vertical_lines[offset + inc*i + 1];
            boxing_linef *line = &metadata_vertical_lines[i];
            line->p1.x = (a.p1.x + b.p1.x) / 2;
            line->p1.y = (a.p1.y + b.p1.y) / 2;
            line->p2.x = (a.p2.x + b.p2.x) / 2;
            line->p2.y = (a.p2.y + b.p2.y) / 2;
        }
    }
    else
    {
        int offset = inc / 2;
        for (int i = 0; i < horizontal_tile_count; i++)
        {
            metadata_vertical_lines[i] = vertical_lines[offset + inc*i];
        }
    }

    free(vertical_lines);
    vertical_lines = NULL;

    // Calculate metadata_horizontal_lines lines
    boxing_pointf *metadata_right_bar_points = calloc(vertical_tile_count, sizeof(boxing_pointf));
    boxing_pointf *metadata_left_bar_points = calloc(vertical_tile_count, sizeof(boxing_pointf));
    for (int i = 0; i < vertical_tile_count; i++)
    {
        metadata_left_bar_points[i].x = (boxing_float)corner_marks->bottom_left.x;
        metadata_left_bar_points[i].y = corner_marks->bottom_left.y + BASEMEMBER(base.y_sampling_rate) * ((boxing_float)tile_size.y / (boxing_float)2 + tile_size.y * i);
        metadata_right_bar_points[i].x = (boxing_float)corner_marks->bottom_right.x;
        metadata_right_bar_points[i].y = corner_marks->bottom_right.y + BASEMEMBER(base.y_sampling_rate) * ((boxing_float)tile_size.y / (boxing_float)2 + tile_size.y * i);
    }

    boxing_linef *metadata_horizontal_lines = calloc(vertical_tile_count, sizeof(boxing_linef));

    calculate_lines(metadata_left_bar_points, vertical_tile_count, metadata_right_bar_points, vertical_tile_count, metadata_horizontal_lines);

    retval &= calculate_intersection_points(metadata_horizontal_lines,
        metadata_vertical_lines,
        vertical_tile_count,
        horizontal_tile_count,
        offsets,
        (boxing_float)vertical_barwidth,
        (boxing_float)width,
        &tracker->base.metadata_sampler->location_matrix);
    if (retval)
    {
        tracker->base.metadata_sampler->state = DTRUE;
    }
    free(metadata_horizontal_lines);
    free(metadata_left_bar_points);
    free(metadata_right_bar_points);
    free(metadata_vertical_lines);
    free(offsets);

    return retval;
}


//----------------------------------------------------------------------------
/*!
 *  \brief Track vertical shift function.
 *
 *  Track vertical shift function description.
 *
 *  \param[in, out]  user         Pointer to the user data.
 *  \param[in]       tracker      Tracker.
 *  \param[in]       input_image  Image.
 *  \return a sign of the success of the function.
 */

DBOOL boxing_track_vertical_shift(void* user, boxing_tracker_gpf_1* tracker, const boxing_image8* input_image)
{
    BOXING_UNUSED_PARAMETER(user);
    boxing_matrixf displacement_matrix;
    boxing_matrixf_init_in_place(&displacement_matrix, 0, 0);
    get_displacement_matrix(tracker, input_image, &displacement_matrix);
    boxing_frame_tracker_util_add_displacement(&displacement_matrix, &(BASEMEMBER(content_sampler)->location_matrix));
    return DTRUE;
}


//----------------------------------------------------------------------------
/*!
 * \} end of frame group
 */


// PRIVATE FRAME TRACKER GPF 1 FUNCTIONS
//

static boxing_pointf vector_sub(const boxing_pointi* a, const boxing_pointi* b)
{
    boxing_pointf return_value = { (boxing_float)(b->x - a->x), (boxing_float)(b->y - a->y) };
    return return_value;
}


static void boxing_coordmapper_set(
    boxing_coordmapper * mapper,
    const frame_corner_marks* corner_marks,
    const boxing_pointi* top_left_offset,
    boxing_float horizontal_distance,
    boxing_float vertical_distance)
{
    mapper->vector_top = vector_sub(&corner_marks->top_left, &corner_marks->top_right);
    mapper->vector_bottom = vector_sub(&corner_marks->bottom_left, &corner_marks->bottom_right);
    mapper->vector_left = vector_sub(&corner_marks->top_left, &corner_marks->bottom_left);
    mapper->top_left = boxing_math_pointi_to_float(&corner_marks->top_left);
    mapper->top_left_offset = boxing_math_pointi_to_float(top_left_offset);
    mapper->vertical_distance = vertical_distance;
    mapper->horizontal_distance = horizontal_distance;
}


static void boxing_coordmapper_init_empty(boxing_coordmapper *mapper)
{
    frame_corner_marks cm = { { 0, 0 }, { 1, 0 }, { 0, 1 }, { 1, 1 } };
    boxing_pointi p5 = { 0, 0 };
    boxing_coordmapper_set(mapper, &cm, &p5, 1, 1);
}


static boxing_pointf boxing_coordmapper_map(const boxing_coordmapper * mapper, int x, int y)
{
    boxing_float b = (y - mapper->top_left_offset.y) / (boxing_float)mapper->vertical_distance;
    boxing_float a = (x - mapper->top_left_offset.x) / (boxing_float)mapper->horizontal_distance;
    boxing_float c = a*b;

    boxing_pointf location;
    location.x = mapper->vector_bottom.x*c + mapper->vector_left.x*b + mapper->vector_top.x*(a - c) + mapper->top_left.x;
    location.y = mapper->vector_bottom.y*c + mapper->vector_left.y*b + mapper->vector_top.y*(a - c) + mapper->top_left.y;

    return location;
}


static void tracker_gpf_1_free(boxing_tracker * tracker)
{
    free(SMEMBER(syncpoint_index).data);
    boxing_sampler_destroy(SMEMBER(left_reference_bar_sampler));
    boxing_sampler_destroy(SMEMBER(right_reference_bar_sampler)); 
    boxing_sampler_destroy(SMEMBER(top_reference_bar_sampler));
    boxing_sampler_destroy(SMEMBER(bottom_reference_bar_sampler));
    boxing_sampler_destroy(SMEMBER(top_left_corner_mark_sampler));
    boxing_sampler_destroy(SMEMBER(top_right_corner_mark_sampler));
    boxing_sampler_destroy(SMEMBER(bottom_left_corner_mark_sampler));
    boxing_sampler_destroy(SMEMBER(bottom_right_corner_mark_sampler));

    boxing_tracker_gpf_free(tracker);
}


static int validate_coordinate(int width, int w_margin, int height, int h_margin, struct boxing_pointi_s *location)
{
    return ((location->x+w_margin >= width) || (location->x < w_margin) || (location->y+h_margin >= height ) || (location->y < h_margin));
}


static int track_frame(boxing_tracker * tracker, const boxing_image8 * frame)
{
    int retval = boxing_tracker_gpf_track_frame(tracker, frame);
    boxing_areasampler_set_radius((boxing_areasampler*)BASEMEMBER(metadata_sampler), (int)( (SMEMBER(tile_size)/(boxing_float)2.4f)*BASEMEMBER(base.x_sampling_rate) ));
    boxing_areasampler_set_radius((boxing_areasampler*)BASEMEMBER(calibration_bar_sampler),
                                  (int)( (SMEMBER(frame)->calibration_bar->base.size.y/(boxing_float)2.4f)*BASEMEMBER(base.x_sampling_rate) ));
    return retval;
}


static int track_frame_analog_mode(boxing_tracker_gpf * tracker, const boxing_image8 * input_image)
{
    DLOG_INFO("track_frame_analog_mode Retrieving marks");

    BASEBASEMEMBER(x_sampling_rate) = (boxing_float)input_image->width/(boxing_float)SMEMBER(frame_dimension).x;
    BASEBASEMEMBER(y_sampling_rate) = (boxing_float)input_image->height/(boxing_float)SMEMBER(frame_dimension).y;
     
    boxing_corner_mark_definition definition;
    definition.corner_mark_symbol = (boxing_float)SMEMBER(corner_mark_dimension).x;
    definition.corner_mark_gap = (boxing_float)SMEMBER(corner_mark_gap_size);
    definition.printable_area_width = (float)SMEMBER(frame_dimension).x;
    definition.printable_area_height = (float)SMEMBER(frame_dimension).y;
    definition.border = 1;
    definition.border_gap = 1;

    frame_corner_marks corner_marks;
    int res = BASEBASEMEMBER(track_corner_mark)(BASEBASEMEMBER(user_data), input_image, &definition, BASEBASEMEMBER(x_sampling_rate), BASEBASEMEMBER(y_sampling_rate), &corner_marks);

#ifdef BOXINGLIB_CALLBACK
    if (BASEBASEMEMBER(on_corner_mark_complete))
    {
        res = BASEBASEMEMBER(on_corner_mark_complete)(BASEBASEMEMBER(user_data), &res, &corner_marks);
    }
#endif

    float raw_margin = definition.corner_mark_symbol/2 + definition.border_gap + definition.border;
    int w_margin = (int)ceil( BASEBASEMEMBER(x_sampling_rate)*raw_margin) + 1;
    int h_margin = (int)ceil(BASEBASEMEMBER(y_sampling_rate)*raw_margin) + 1;
    res |= validate_coordinate(input_image->width, w_margin, input_image->height, h_margin, &corner_marks.top_left);
    res |= validate_coordinate(input_image->width, w_margin, input_image->height, h_margin, &corner_marks.top_right);
    res |= validate_coordinate(input_image->width, w_margin, input_image->height, h_margin, &corner_marks.bottom_left);
    res |= validate_coordinate(input_image->width, w_margin, input_image->height, h_margin, &corner_marks.bottom_right);

    // update corner mark sampler locations
    *SMEMBER(top_left_corner_mark_sampler)->location_matrix.data = boxing_math_pointi_to_float( &corner_marks.top_left );
    SMEMBER(top_left_corner_mark_sampler)->state = DTRUE;

    *SMEMBER(top_right_corner_mark_sampler)->location_matrix.data = boxing_math_pointi_to_float( &corner_marks.top_right );
    SMEMBER(top_right_corner_mark_sampler)->state = DTRUE;

    *SMEMBER(bottom_left_corner_mark_sampler)->location_matrix.data  = boxing_math_pointi_to_float( &corner_marks.bottom_left );
    SMEMBER(bottom_left_corner_mark_sampler)->state = DTRUE;

    *SMEMBER(bottom_right_corner_mark_sampler)->location_matrix.data = boxing_math_pointi_to_float( &corner_marks.bottom_right );
    SMEMBER(bottom_right_corner_mark_sampler)->state = DTRUE;


    if (res != BOXING_CORNER_MARK_OK)
    {
        DLOG_ERROR("(track_frame_analog_mode) Finding corner marks failed");
        return 1;
    }


    boxing_pointi dimension = boxing_corner_mark_dimension( &corner_marks );
    BASEMEMBER(base.x_sampling_rate) = (boxing_float)(dimension.x)/(boxing_float)(SMEMBER(frame_dimension).x - SMEMBER(corner_mark_dimension).x);
    BASEMEMBER(base.y_sampling_rate) = (boxing_float)(dimension.y)/(boxing_float)(SMEMBER(frame_dimension).y - SMEMBER(corner_mark_dimension).y);

    DLOG_INFO1("Horizontal sampling rate = %f", BASEMEMBER(base.x_sampling_rate));
    DLOG_INFO1("Vertical sampling rate   = %f", BASEMEMBER(base.y_sampling_rate));


    boxing_pointi point_temp = {SMEMBER(x_offset), SMEMBER(y_offset)};
    boxing_coordmapper_set(&SMEMBER(coordinate_mapper), &corner_marks, 
                              &point_temp, (boxing_float)SMEMBER(horizontal_distance), (boxing_float)SMEMBER(vertical_distance));

    if(tracker->base.mode & BOXING_TRACK_CALIBRATION_BAR)
    {
        boxing_pointi location = boxing_component_absolute_location((boxing_component*)SMEMBER(frame)->calibration_bar);
        boxing_pointi  size     = SMEMBER(frame)->calibration_bar->base.size;
        boxing_pointf point = boxing_coordmapper_map(&SMEMBER(coordinate_mapper), location.x, location.y+size.y/2); /// \todo ask if conversion to float is ok
        boxing_pointi left = {(int)roundf(point.x), (int)roundf(point.y)};
        point = boxing_coordmapper_map(&SMEMBER(coordinate_mapper), location.x+size.x, location.y+size.y/2);
        boxing_pointi right = {(int)roundf(point.x), (int)roundf(point.y)};
        boxing_matrixf * locations = &tracker->calibration_bar_sampler->location_matrix;

        int step = locations->height;
        if(step)
        {
            boxing_pointf v_step;
            boxing_pointf v_pos;
            v_step.x = ((boxing_float)(right.x - left.x))/step;
            v_step.y = ((boxing_float)(right.y - left.y))/step;
            v_pos.x = left.x + v_step.x/2;
            v_pos.y = left.y + v_step.y/2;
            for(int i = 0; i < step; i++)
            {
                MATRIX_ELEMENT(locations, i, 0) = v_pos;
                v_pos.x += v_step.x;
                v_pos.y += v_step.y;
            }
        }
        tracker->calibration_bar_sampler->state = DTRUE;
    }

    const boxing_pointi * corners[4] = { &corner_marks.top_left, &corner_marks.top_right, &corner_marks.bottom_left, &corner_marks.bottom_right };
    boxing_frame_tracker_util_calculate_average_maxmin(input_image, corners, 4, BASEMEMBER(base.x_sampling_rate), BASEMEMBER(base.y_sampling_rate), &SMEMBER(avg_max), &SMEMBER(avg_min));
    DLOG_INFO2("Average max, min = %f, %f", SMEMBER(avg_max), SMEMBER(avg_min));

    if(!calculate_reference_bars((boxing_tracker_gpf_1 *)tracker, input_image))
    {
        return 1;
    }

    boxing_float *horizontal_left_shift  = calloc(input_image->height, sizeof(boxing_float));
    boxing_float *horizontal_right_shift = calloc(input_image->height, sizeof(boxing_float));

    if(tracker->base.mode & BOXING_TRACK_HORIZONTAL_SHIFT)
    {
        boxing_pointi location = boxing_component_absolute_location((boxing_component*)SMEMBER(frame)->top_left_reference_point);
        boxing_pointi size     = SMEMBER(frame)->top_left_reference_point->base.size;

        boxing_pointi frame_size = SMEMBER(frame)->base.size(&SMEMBER(frame)->base);
        int offset = location.y + size.y/4;

        boxing_pointf pointq = boxing_coordmapper_map(&SMEMBER(coordinate_mapper), 0, offset);
        boxing_pointi hstl = boxing_math_pointf_round_to_int(&pointq);
        pointq = boxing_coordmapper_map(&SMEMBER(coordinate_mapper), 0, frame_size.y-1-offset);
        boxing_pointi hsbl = boxing_math_pointf_round_to_int(&pointq);
        calc_horizontal_offset((boxing_tracker_gpf_1 *)tracker, input_image,
                hstl, hsbl, horizontal_left_shift, BOXING_TRACKERUTIL_SCAN_DIRECTION_LEFT_TO_RIGHT);

        pointq = boxing_coordmapper_map(&SMEMBER(coordinate_mapper), frame_size.x-1, offset);
        boxing_pointi hstr = boxing_math_pointf_round_to_int(&pointq);
        pointq = boxing_coordmapper_map(&SMEMBER(coordinate_mapper), frame_size.x-1, frame_size.y-1-offset);
        boxing_pointi hsbr = boxing_math_pointf_round_to_int(&pointq);
        calc_horizontal_offset((boxing_tracker_gpf_1 *)tracker, input_image,
                hstr, hsbr, horizontal_right_shift, BOXING_TRACKERUTIL_SCAN_DIRECTION_RIGHT_TO_LEFT);
    }

    if( (tracker->content_sampler->location_matrix.width  != SMEMBER(top_reference_bar_sampler)->location_matrix.width) ||
        (tracker->content_sampler->location_matrix.height != SMEMBER(left_reference_bar_sampler)->location_matrix.width) )
    {
        DLOG_ERROR4("(track_frame_analog_mode) Container size (%u, %u) does not match reference bar size (%u,%u)",
                    tracker->content_sampler->location_matrix.width,
                    tracker->content_sampler->location_matrix.height,
                    SMEMBER(top_reference_bar_sampler)->location_matrix.width,
                    SMEMBER(left_reference_bar_sampler)->location_matrix.width);
        return 1;
    }

    DLOG_INFO("Retrieving data locations");
    const boxing_matrixf * left_sample =   &SMEMBER(left_reference_bar_sampler)->location_matrix;
    const boxing_matrixf * right_sample =  &SMEMBER(right_reference_bar_sampler)->location_matrix;
    const boxing_matrixf * top_sample =    &SMEMBER(top_reference_bar_sampler)->location_matrix;
    const boxing_matrixf * bottom_sample = &SMEMBER(bottom_reference_bar_sampler)->location_matrix;
    if ( !SMEMBER(calculate_sampling_location)(
                                   BASEBASEMEMBER(user_data), (boxing_tracker_gpf_1 *)tracker,
                                   left_sample->data, left_sample->width,
                                   right_sample->data, right_sample->width,
                                   top_sample->data, top_sample->width,
                                   bottom_sample->data, bottom_sample->width,
                                   horizontal_left_shift,
                                   horizontal_right_shift,
                                   input_image->height,
                                   SMEMBER(frame_dimension).x,
                                   SMEMBER(corner_mark_dimension).x,
                                   &corner_marks))
    {
        DLOG_ERROR("(track_frame_analog_mode) Calculating initial location matrix failed");
        return 1;
    }

    free(horizontal_right_shift);
    horizontal_right_shift = NULL;
    free(horizontal_left_shift);
    horizontal_left_shift = NULL;

    if(tracker->base.mode & BOXING_TRACK_VERTICAL_SHIFT)
    {
        SMEMBER(track_vertical_shift)( BASEBASEMEMBER(user_data), (boxing_tracker_gpf_1 *)tracker, input_image);
    }

    if(tracker->base.mode & BOXING_TRACK_SYNC_POINTS)
    {
        correct_frame_geometry_properties props;
        props.sync_point_center_is_bright = DTRUE;
        props.sync_point_max_allowed_variation = 0.1f;
        props.sync_point_search_radius = 4.5f;
        boxing_matrixf * locations = &tracker->content_sampler->location_matrix;
        SMEMBER(correct_frame_geometry)(BASEBASEMEMBER(user_data), input_image, locations, &SMEMBER( syncpoint_index ), &props);
    }

    return 0;
}


static void get_displacement_matrix(boxing_tracker_gpf_1 *tracker, const boxing_image8 * image, boxing_matrixf * displacement_matrix)
{
     boxing_matrixf_init_in_place(displacement_matrix, 3, 3);

     memset(displacement_matrix->data, 0, 3 * 3 * sizeof(boxing_pointf));

     boxing_pointi start;
     boxing_pointi stop;
     boxing_pointf reference_point;
     boxing_float yoffset;
     boxing_pointf left_edge_location;
     boxing_pointf center_edge_location;
     boxing_pointf right_edge_location;

     boxing_matrixf * location_matrix = &tracker->base.content_sampler->location_matrix;
     int line_points_size = location_matrix->width;
     // top
     boxing_pointf * line_points = location_matrix->data;
     reference_point = (line_points)[25];
     start.x = (int)( reference_point.x );
     start.y = (int)( reference_point.y-4*BASEMEMBER(base.y_sampling_rate) );
     stop = boxing_math_pointf_to_int( &reference_point );
     left_edge_location = boxing_frame_tracker_util_find_h_reference_bar_edge(image, start, stop, BASEMEMBER(base.x_sampling_rate), BASEMEMBER(base.y_sampling_rate));
     boxing_math_pointf_add( &left_edge_location, &start );

     reference_point = (line_points)[line_points_size/2 - 1];
     start.x = (int)( reference_point.x );
     start.y = (int)( reference_point.y-4*BASEMEMBER(base.y_sampling_rate) );
     stop = boxing_math_pointf_to_int( &reference_point );
     center_edge_location = boxing_frame_tracker_util_find_h_reference_bar_edge(image, start, stop, BASEMEMBER(base.x_sampling_rate), BASEMEMBER(base.y_sampling_rate));
     boxing_math_pointf_add( &center_edge_location, &start );

     reference_point = (line_points)[line_points_size - 25];
     start.x = (int)( reference_point.x );
     start.y = (int)( reference_point.y-4*BASEMEMBER(base.y_sampling_rate) );
     stop = boxing_math_pointf_to_int( &reference_point );
     right_edge_location = boxing_frame_tracker_util_find_h_reference_bar_edge(image, start, stop, BASEMEMBER(base.x_sampling_rate), BASEMEMBER(base.y_sampling_rate));
     boxing_math_pointf_add( &right_edge_location, &start );

     yoffset = center_edge_location.y - (left_edge_location.y+right_edge_location.y)/2;
     MATRIX_ELEMENT(displacement_matrix, 0, 1).x = 0.0f;
     MATRIX_ELEMENT(displacement_matrix, 0, 1).y = yoffset;

     // bottom
     line_points =  MATRIX_ROW(&tracker->base.content_sampler->location_matrix, tracker->base.content_sampler->location_matrix.height-1);
     reference_point = (line_points)[25];
     start.x = (int)( reference_point.x );
     start.y = (int)( reference_point.y+4*BASEMEMBER(base.y_sampling_rate) );
     stop = boxing_math_pointf_to_int( &reference_point );
     left_edge_location = boxing_frame_tracker_util_find_h_reference_bar_edge(image, start, stop, BASEMEMBER(base.x_sampling_rate), BASEMEMBER(base.y_sampling_rate));
     boxing_math_pointf_add( &left_edge_location, &start );

     reference_point = (line_points)[line_points_size/2 - 1];
     start.x = (int)( reference_point.x );
     start.y = (int)( reference_point.y+4*BASEMEMBER(base.y_sampling_rate) );
     stop = boxing_math_pointf_to_int( &reference_point );
     center_edge_location = boxing_frame_tracker_util_find_h_reference_bar_edge(image, start, stop, BASEMEMBER(base.x_sampling_rate), BASEMEMBER(base.y_sampling_rate));
     boxing_math_pointf_add( &center_edge_location, &start );

     reference_point = (line_points)[line_points_size - 25];
     start.x = (int)( reference_point.x );
     start.y = (int)( reference_point.y+4*BASEMEMBER(base.y_sampling_rate) );
     stop = boxing_math_pointf_to_int( &reference_point );
     right_edge_location = boxing_frame_tracker_util_find_h_reference_bar_edge(image, start, stop, BASEMEMBER(base.x_sampling_rate), BASEMEMBER(base.y_sampling_rate));
     boxing_math_pointf_add( &right_edge_location, &start );

     yoffset = center_edge_location.y - (left_edge_location.y+right_edge_location.y)/2;
     MATRIX_ELEMENT(displacement_matrix, 2, 1).x = 0;
     MATRIX_ELEMENT(displacement_matrix, 2, 1).y = (boxing_float)yoffset;

     MATRIX_ELEMENT(displacement_matrix, 1, 1).x = (MATRIX_ELEMENT(displacement_matrix, 2, 1).x + MATRIX_ELEMENT(displacement_matrix, 0, 1).x)/2;
     MATRIX_ELEMENT(displacement_matrix, 1, 1).y = (MATRIX_ELEMENT(displacement_matrix, 2, 1).y + MATRIX_ELEMENT(displacement_matrix, 0, 1).y)/2;

     DLOG_INFO("Dislacement matrix");
     DLOG_INFO3("(%f %f %f)",MATRIX_ELEMENT(displacement_matrix, 0, 0).y, MATRIX_ELEMENT(displacement_matrix, 0, 1).y, MATRIX_ELEMENT(displacement_matrix, 0, 2).y);
     DLOG_INFO3("(%f %f %f)",MATRIX_ELEMENT(displacement_matrix, 1, 0).y, MATRIX_ELEMENT(displacement_matrix, 1, 1).y, MATRIX_ELEMENT(displacement_matrix, 1, 2).y);
     DLOG_INFO3("(%f %f %f)",MATRIX_ELEMENT(displacement_matrix, 2, 0).y, MATRIX_ELEMENT(displacement_matrix, 2, 1).y, MATRIX_ELEMENT(displacement_matrix, 2, 2).y);

}


static DBOOL calc_horizontal_offset(boxing_tracker_gpf_1 * tracker, const boxing_image8 * image, const boxing_pointi from, const boxing_pointi to, float * offsets, int scan_direction)
{
    boxing_float *border_location = calloc(image->height, sizeof(boxing_float));
    boxing_frame_tracker_util_track_vertical_border(image, &from, &to, border_location, scan_direction, BASEMEMBER(base.x_sampling_rate));

    boxing_pointf * top    = tracker->top_reference_bar_sampler->location_matrix.data;
    boxing_pointf * bottom = tracker->bottom_reference_bar_sampler->location_matrix.data;

    boxing_pointf start = top[0];
    boxing_pointf end = bottom[0];

    if(scan_direction == BOXING_TRACKERUTIL_SCAN_DIRECTION_RIGHT_TO_LEFT)
    {
        start = top[tracker->top_reference_bar_sampler->location_matrix.width-1];
        end = bottom[tracker->bottom_reference_bar_sampler->location_matrix.width-1];
    }

    boxing_double dx = (border_location[(int)end.y] - border_location[(int)start.y])/(end.y - start.y);
    for(int y = from.y; y < to.y; y++)
    {
        boxing_double x = dx*(y - start.y) + border_location[(int)start.y];
        offsets[y] = (boxing_float)-(x - border_location[y]);

        assert( isfinite( offsets[y] ) );
    }

    free(border_location);
    return DTRUE;
}


static gvector * sample_reference_bar(boxing_sampler * sampler, const boxing_image8 * image)
{
    boxing_image8 * sampled_image = NULL;
    sampled_image = sampler->sample(sampler, image);
    int block_width = sampler->location_matrix.width > 32 ? 32 : sampler->location_matrix.width;
    int block_height = sampler->location_matrix.height > 32 ? 32 : sampler->location_matrix.height;

    gvector * data = boxing_datapoints_quantize(sampled_image, block_width, block_height, 2);

    boxing_image8_free(sampled_image);

    return data;
}


static gvector * track_reference_bar_sync(boxing_sampler * sampler, const boxing_image8 * image, int sync_distance, int sync_offset)
{
    gvector * reference_bar = sample_reference_bar(sampler, image);
    gvector * sync_locations = gvector_create(sizeof(sync_location), 2 + ((reference_bar->size - sync_offset) / sync_distance));

    sync_location * sync = (sync_location*)sync_locations->buffer;
    size_t sync_count = 0;

    // insert dummy sync
    sync->actual = REFBAR_INVALID_SYNC_REF;
    sync->expected = 0;
    sync++;
    sync_count++;

    // track sync
    char * ref_value_ptr = reference_bar->buffer;
    const int search_width = 4;
    char sync_symbol[] = { 1, 0, 1, 0, 0, 0, 1, 0, 1 }; //NOTE: must have size = 2*search_width + 1;

    for (int i = search_width; (i < (int)reference_bar->size - search_width) && (sync_count < (sync_locations->size - 1)); i++)
    {
        int sum = 0;
        sum += abs(sync_symbol[0] - ref_value_ptr[i - 4]);
        sum += abs(sync_symbol[1] - ref_value_ptr[i - 3]);
        sum += abs(sync_symbol[2] - ref_value_ptr[i - 2]);
        sum += abs(sync_symbol[3] - ref_value_ptr[i - 1]);
        sum += abs(sync_symbol[4] - ref_value_ptr[i]);
        sum += abs(sync_symbol[5] - ref_value_ptr[i + 1]);
        sum += abs(sync_symbol[6] - ref_value_ptr[i + 2]);
        sum += abs(sync_symbol[7] - ref_value_ptr[i + 3]);
        sum += abs(sync_symbol[8] - ref_value_ptr[i + 4]);
        if (sum == 0)
        {
            sync->actual = i;
            sync->expected = REFBAR_INVALID_SYNC_REF;
            sync++;
            sync_count++;
        }
    }

    // insert dummy sync
    sync->actual = REFBAR_INVALID_SYNC_REF;
    sync->expected = (int)reference_bar->size;
    sync_count++;

    if (sync_locations->size != sync_count)
    {
        gvector_resize(sync_locations, (unsigned int)sync_count);
    }

    gvector_free(reference_bar);
    return sync_locations;
}


static boxing_float getAvgSamplingrate(boxing_sampler * sampler)
{
    boxing_int32   w = sampler->location_matrix.width;
    boxing_int32   h = sampler->location_matrix.height;


    boxing_pointf delta = boxing_math_pointf_subtract(sampler->location_matrix.data + (w*h - 1), sampler->location_matrix.data);
    boxing_float  length = boxing_math_pointf_length(&delta);

    return  length / sqrtf((boxing_float)(w*w + h*h));
}


static void validate_reference_bar_sync(
    boxing_coordmapper * mapper, boxing_sampler * sampler, gvector * sync_locations,
    boxing_pointi location, boxing_pointi direction,
    int sync_distance, int sync_offset)
{
    /** 
      only allow a deviation between expected and actual sync loaction
      of +/- 4 times the sampling rate  */
    const boxing_float max_sync_deviation = getAvgSamplingrate(sampler)*4;

    /// \todo warning: variable 'j_start' set but not used
    int j_start = 0;
    for (int i = 0; i < (int)sync_locations->size; i++)
    {
        boxing_float error = BOXING_FLOAT_MAX;
        sync_location * sync = (sync_location*)sync_locations->buffer + i;

        if (sync->expected != REFBAR_INVALID_SYNC_REF)
        {
            continue;
        }

        boxing_pointf * sync_loc = sampler->location_matrix.data + sync->actual;
        int size = sampler->location_matrix.height * sampler->location_matrix.width;
        int j = 0;// j_start;
        while (1)
        {
            int pos = sync_offset + j*sync_distance;
            boxing_pointf start_left = boxing_coordmapper_map(mapper, location.x + pos*direction.x, location.y + pos*direction.y);

            boxing_pointf delta = boxing_math_pointf_subtract( &start_left, sync_loc );
            boxing_float err = boxing_math_pointf_length( &delta );
            if (err > error)
            {
                // discard spurious sync points
                if (error > max_sync_deviation)
                {
                    sync->actual = REFBAR_INVALID_SYNC_REF;
                    sync->expected = REFBAR_INVALID_SYNC_REF;
                }
                break; 
            }

            if (pos > size)
            {
                sync->actual = REFBAR_INVALID_SYNC_REF;
                sync->expected = REFBAR_INVALID_SYNC_REF;
                break;
            }
            sync->expected = pos;
            error = err;
            j++;
            j_start = j;
        }
    }
    (void)j_start;
}


static sync_location * get_next_sync(sync_location * cur, sync_location * first, sync_location * last)
{
    if (!cur)
    {
        return first;
    }
    else if (cur == last)
    {
        return NULL;
    }
    else
    {
        return (cur + 1);
    }
}


static inline boxing_pointf * synth(boxing_pointf start, boxing_pointf step, int steps, boxing_pointf * vector)
{
    boxing_pointf * sync = vector;
    for (int i = 0; i < steps; i++,  sync++)
    {
        sync->x = start.x + i * step.x;
        sync->y = start.y + i * step.y;
    }
    return sync;
}


static inline boxing_pointi vec_scale_add(boxing_pointi a, int scale, boxing_pointi b)
{
    boxing_pointi res = { a.x * scale + b.x, a.y * scale + b.y };
    return res;
}


static void reference_bar_sync(boxing_pointi location, boxing_pointi direction, boxing_coordmapper * mapper, boxing_sampler * sampler, const boxing_image8 * image, int sync_distance, int sync_offset)
{
    // sample bar
    gvector * sync_locations = track_reference_bar_sync(sampler, image, sync_distance, sync_offset);

    validate_reference_bar_sync(mapper, sampler, sync_locations, location, direction, sync_distance, sync_offset);

    int samples = sampler->location_matrix.height * sampler->location_matrix.width;

    // resync
    boxing_matrixf * org_location_matrix = boxing_matrixf_copy(&sampler->location_matrix);
    boxing_pointf * org_locations = org_location_matrix->data;
    boxing_pointf * dst_locations = sampler->location_matrix.data;
    sync_location * sync_begin = (sync_location*)sync_locations->buffer;
    sync_location * sync_end = sync_begin + sync_locations->size - 1;
    sync_location * sync = NULL;

    sync = get_next_sync(sync, sync_begin, sync_end);

    while (sync != sync_end)
    {
        sync_location * sync_next = get_next_sync(sync, sync_begin, sync_end);
        while (sync_next->actual == REFBAR_INVALID_SYNC_REF && sync_next->expected == REFBAR_INVALID_SYNC_REF)
        {
            sync_next = get_next_sync(sync_next, sync_begin, sync_end);
        }

        if (sync->actual == REFBAR_INVALID_SYNC_REF && sync_next->actual == REFBAR_INVALID_SYNC_REF)
        {
            // synth
            int synth_count = sync_next->expected - sync->expected;
            if (synth_count)
            {
                boxing_pointi ai = vec_scale_add(direction, sync->expected, location);
                boxing_pointi bi = vec_scale_add(direction, sync_next->expected, location);

                boxing_pointf a = boxing_coordmapper_map(mapper, ai.x, ai.y);
                boxing_pointf b = boxing_coordmapper_map(mapper, bi.x, bi.y);
                boxing_pointf sr = { (b.x - a.x) / synth_count, (b.y - a.y) / synth_count };

                boxing_pointf *dst = dst_locations + sync->expected;

                (void)synth(a, sr, synth_count, dst);
            }
        }
        else if (sync->actual == REFBAR_INVALID_SYNC_REF && sync_next->actual != REFBAR_INVALID_SYNC_REF)
        {
            // tailin - copy what we have and synth the rest

            int expected_copy_count = (sync_next->expected);
            int copy_count = (sync_next->actual);

            if (copy_count > expected_copy_count)
            {
                copy_count = expected_copy_count;
            }
            int synth_count = expected_copy_count - copy_count;

            boxing_pointf *src = org_locations + sync_next->actual - copy_count;
            boxing_pointf *dst = dst_locations + sync->expected + synth_count;

            memcpy(dst, src, copy_count*sizeof(boxing_pointf));

            if (synth_count)
            {
                boxing_pointf *a = org_locations + sync_next->actual - 1;
                boxing_pointf *b = org_locations + sync_next->actual + 1;
                boxing_pointf sr = { (b->x - a->x) / 2, (b->y - a->y) / 2 };

                boxing_pointf start = { dst->x - (sr.x*synth_count), dst->y - (sr.y*synth_count) };

                dst = dst_locations + sync->expected;
                synth(start, sr, synth_count, dst);
            }
        }
        else if (sync->actual != REFBAR_INVALID_SYNC_REF && sync_next->actual == REFBAR_INVALID_SYNC_REF)
        {
            // tailout - copy what we have and synth the rest
            int expected_copy_count = (samples - sync->expected);
            int copy_count = (samples - sync->actual);

            if (copy_count > expected_copy_count)
            {
                copy_count = expected_copy_count;
            }
            int synth_count = expected_copy_count - copy_count;

            boxing_pointf *src = org_locations + sync->actual;
            boxing_pointf *dst = dst_locations + sync->expected;

            memcpy(dst, src, copy_count*sizeof(boxing_pointf));

            if (synth_count)
            {
                boxing_pointf *a = org_locations + sync->actual - 1;
                boxing_pointf *b = org_locations + sync->actual + 1;
                boxing_pointf sr = { (b->x - a->x) / 2, (b->y - a->y) / 2 };

                dst += copy_count -1;
                boxing_pointf start = { dst->x + sr.x, dst->y + sr.y };
                dst++;

                dst = synth(start, sr, synth_count, dst);
            }
        }
        else
        {
            boxing_pointf *dst = dst_locations + sync->expected;
            int location_count = sync_next->expected - sync->expected;
            if (sync_next->actual - sync->actual == location_count)
            {
                // copy what we have
                boxing_pointf *src = org_locations + sync->actual;
                memcpy(dst, src, location_count*sizeof(boxing_pointf));
            }
            else
            {
                // synth
                if (location_count)
                {
                    boxing_pointf *a = org_locations + sync->actual;
                    boxing_pointf *b = org_locations + sync_next->actual;
                    boxing_pointf sr = { (b->x - dst->x) / location_count, (b->y - dst->y) / location_count };

                    (void)synth(*a, sr, location_count, dst);
                }
            }
        }

        sync = sync_next;
    }

    // free
    gvector_free(sync_locations);
    boxing_matrixf_free(org_location_matrix);
}


static DBOOL calculate_reference_bars(boxing_tracker_gpf_1 * tracker, const boxing_image8 * image)
{

    boxing_pointf start_left;
    boxing_pointf end_right;
    boxing_pointf reference_point;
    int reference_bar_sync_distance = tracker->frame->reference_bar_sync_distance;
    int reference_bar_sync_offset = tracker->frame->reference_bar_sync_offset;

    boxing_pointi location = boxing_component_absolute_location((boxing_component*)SMEMBER(frame)->top_reference_bar);
    boxing_pointi  size     = SMEMBER(frame)->top_reference_bar->base.size;
    int perpendicular_samples = (int)( (size.y - 2) * BASEMEMBER(base.y_sampling_rate) );

    start_left = boxing_coordmapper_map(&SMEMBER(coordinate_mapper), location.x-2, location.y+size.y/2);
    end_right  = boxing_coordmapper_map(&SMEMBER(coordinate_mapper), location.x+size.x, location.y+size.y/2);
    reference_point = boxing_coordmapper_map(&SMEMBER(coordinate_mapper), location.x, location.y+size.y/2);

    if (!boxing_frame_tracker_util_track_reference_bar(image, &start_left, &end_right, tracker->top_reference_bar_sampler->location_matrix.data,
        tracker->top_reference_bar_sampler->location_matrix.width, &reference_point, perpendicular_samples))
    {
        DLOG_ERROR("(calculate_reference_bars) Failed tracking top reference bar");
        return DFALSE;
    }
    tracker->top_reference_bar_sampler->state = DTRUE;

    if (reference_bar_sync_distance > 0)
    {
        location.y = location.y + size.y / 2;
        boxing_pointi direction = { 1, 0 };
        reference_bar_sync(location, direction, &tracker->coordinate_mapper, tracker->top_reference_bar_sampler, image, reference_bar_sync_distance, reference_bar_sync_offset);
    }


    location   = boxing_component_absolute_location((boxing_component*)SMEMBER(frame)->bottom_reference_bar);
    size       = SMEMBER(frame)->bottom_reference_bar->base.size;
    start_left = boxing_coordmapper_map(&SMEMBER(coordinate_mapper), location.x-2, location.y+size.y/2);
    end_right  = boxing_coordmapper_map(&SMEMBER(coordinate_mapper), location.x+size.x, location.y+size.y/2);
    reference_point = boxing_coordmapper_map(&SMEMBER(coordinate_mapper), location.x, location.y+size.y/2);

    if (!boxing_frame_tracker_util_track_reference_bar(image, &start_left, &end_right, tracker->bottom_reference_bar_sampler->location_matrix.data,
            tracker->bottom_reference_bar_sampler->location_matrix.width, &reference_point, perpendicular_samples))    {
        DLOG_ERROR( "(calculate_reference_bars) Failed tracking bottom reference bar" );
        return DFALSE;
    }    
    tracker->bottom_reference_bar_sampler->state = DTRUE;

    if (reference_bar_sync_distance > 0)
    {
        location.y = location.y + size.y / 2;
        boxing_pointi direction = { 1, 0 };
        reference_bar_sync(location, direction, &tracker->coordinate_mapper, tracker->bottom_reference_bar_sampler, image, reference_bar_sync_distance, reference_bar_sync_offset);
    }

    boxing_pointf start_top;
    boxing_pointf end_bottom;

    location   = boxing_component_absolute_location((boxing_component*)SMEMBER(frame)->left_reference_bar);
    size       = SMEMBER(frame)->left_reference_bar->base.size;
    perpendicular_samples = (int)( (size.x - 2) * BASEMEMBER(base.x_sampling_rate) );
    start_top  = boxing_coordmapper_map(&SMEMBER(coordinate_mapper), location.x+size.x/2, location.y-2);
    end_bottom = boxing_coordmapper_map(&SMEMBER(coordinate_mapper), location.x+size.x/2, location.y+size.y);
    reference_point = boxing_coordmapper_map(&SMEMBER(coordinate_mapper), location.x+size.x/2, location.y);

    if (!boxing_frame_tracker_util_track_reference_bar(image, &start_top, &end_bottom, tracker->left_reference_bar_sampler->location_matrix.data,
            tracker->left_reference_bar_sampler->location_matrix.width, &reference_point, perpendicular_samples))    {
        DLOG_ERROR("(calculate_reference_bars) Failed tracking left reference bar");
        return DFALSE;
    }
    tracker->left_reference_bar_sampler->state = DTRUE;    
    if (reference_bar_sync_distance > 0)
    {
        location.x = location.x + size.x / 2;
        boxing_pointi direction = { 0, 1 };
        reference_bar_sync(location, direction, &tracker->coordinate_mapper, tracker->left_reference_bar_sampler, image, reference_bar_sync_distance, reference_bar_sync_offset);
    }

    location   = boxing_component_absolute_location((boxing_component*)SMEMBER(frame)->right_reference_bar);
    size       = SMEMBER(frame)->right_reference_bar->base.size;
    start_top  = boxing_coordmapper_map(&SMEMBER(coordinate_mapper), location.x+size.x/2, location.y-2);
    end_bottom = boxing_coordmapper_map(&SMEMBER(coordinate_mapper), location.x+size.x/2, location.y+size.y);
    reference_point = boxing_coordmapper_map(&SMEMBER(coordinate_mapper), location.x+size.x/2, location.y);

    if (!boxing_frame_tracker_util_track_reference_bar(image, &start_top, &end_bottom, tracker->right_reference_bar_sampler->location_matrix.data,
            tracker->right_reference_bar_sampler->location_matrix.width, &reference_point, perpendicular_samples))    {
        DLOG_ERROR("(calculate_reference_bars) Failed tracking right reference bar");
        return DFALSE;
    }
    tracker->right_reference_bar_sampler->state = DTRUE;
    if (reference_bar_sync_distance > 0)
    {
        location.x = location.x + size.x / 2;
        boxing_pointi direction = { 0, 1 };
        reference_bar_sync(location, direction, &tracker->coordinate_mapper, tracker->right_reference_bar_sampler, image, reference_bar_sync_distance, reference_bar_sync_offset);
    }

    return DTRUE;
}


#if defined (USE_INTERSECTION_WITH_OVERFLOW_CHEKC) 
static inline DBOOL intersect_debug(const boxing_linef la, const boxing_linef lb, boxing_pointf *intersection_point)
{
    // ipmlementation is based on Graphics Gems III's "Faster Line Segment Intersection"
    const boxing_pointf a = {la.p2.x - la.p1.x, la.p2.y - la.p1.y};
    const boxing_pointf b = {lb.p1.x - lb.p2.x, lb.p1.y - lb.p2.y};
    const boxing_pointf c = {la.p1.x - lb.p1.x, la.p1.y - lb.p1.y};

    const boxing_float denominator = a.y * b.x - a.x * b.y;
    if (denominator == 0 || !isfinite( denominator ) )
        return DFALSE;

    const boxing_double reciprocal = 1 / denominator;
    const boxing_double na = (b.y * c.x - b.x * c.y) * reciprocal;
    intersection_point->x = (boxing_float)(la.p1.x + a.x * na);
    intersection_point->y = (boxing_float)(la.p1.y + a.y * na);

    return DTRUE;
}
#endif // USE_INTERSECTION_WITH_OVERFLOW_CHEKC


static inline void intersect_fast(const boxing_linef la, const boxing_pointf lb1, const boxing_pointf lbdir, boxing_pointf *intersection_point) 
{
    // ipmlementation is based on Graphics Gems III's "Faster Line Segment Intersection"
    const boxing_pointf a = {la.p2.x - la.p1.x, la.p2.y - la.p1.y};
    const boxing_pointf b = lbdir;
    const boxing_pointf c = {la.p1.x - lb1.x, la.p1.y - lb1.y};

    const boxing_double denominator = a.y * b.x - a.x * b.y;

    const boxing_double na = (b.y * c.x - b.x * c.y) / denominator;
    intersection_point->x = (boxing_float)(la.p1.x + a.x * na);
    intersection_point->y = (boxing_float)(la.p1.y + a.y * na);
}


static boxing_linef * calculate_lines(
    const boxing_pointf * start_points, 
    const int start_points_size,
    const boxing_pointf * end_points, 
    const int end_points_size,
    boxing_linef * lines)
{
    int line_count = start_points_size > end_points_size ? end_points_size : start_points_size;
    boxing_linef * line = lines;
    const boxing_pointf * line_start = start_points;
    const boxing_pointf * line_end = end_points;

    while(line_count)
    {
        line->p1 = *line_start;
        line->p2 = *line_end;
        line++;
        line_start++;
        line_end++;
        line_count--;
    }

    return lines;
}


static DBOOL calculate_intersection_points(
    const boxing_linef * horizontal_lines, 
    const boxing_linef * vertical_lines,
    int lines_size_h,
    int lines_size_v,
    const boxing_float * offsets,
    const boxing_float vertical_barwidth,
    const boxing_float width,
    boxing_matrixf * intersections_points)
{
    const boxing_linef * horizontal_line = horizontal_lines;
    boxing_pointf * points = intersections_points->data;
    const boxing_linef * vertical_line = vertical_lines;

    boxing_float index_w=0;
    for (int i = 0; i < lines_size_h; i += 1)
    {
        vertical_line = vertical_lines;
        boxing_pointf * point = points;
        index_w = 0.0f;
        boxing_pointf la = horizontal_line->p1;
        boxing_pointf lb = horizontal_line->p2;
        boxing_pointf ldir = boxing_math_pointf_subtract( &la, &lb );
        for (int j = 0; j < lines_size_v; j += 1)
        {
            boxing_pointf intersection;

#if defined (USE_INTERSECTION_WITH_OVERFLOW_CHEKC)
            if ( !intersect_debug( *vertical_line, *horizontal_line, &intersection) )
            {
                return DFALSE;
            }
#else
            intersect_fast( *vertical_line, la, ldir, &intersection );
#endif
            boxing_float k = (index_w + vertical_barwidth)/width;
            int   offset_index = (int)(intersection.y+0.5);
            
            const boxing_float *left_offset = offsets + offset_index * 2;
            const boxing_float *right_offset = left_offset + 1;
            boxing_float horizontal_shift = *left_offset * (1-k) + *right_offset * k;

            assert( isfinite( intersection.x + horizontal_shift ) );
            assert( isfinite( intersection.y ) );

            point->x = intersection.x + horizontal_shift;
            point->y = intersection.y;
            ++point;
            ++vertical_line;
            ++index_w;
        }
        points += intersections_points->width;
        horizontal_line++;
    }
    return DTRUE;
}
