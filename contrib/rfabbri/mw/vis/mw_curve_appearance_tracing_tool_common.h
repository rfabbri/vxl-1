// This is mw_curve_tracing_tool_common.h
#ifndef mw_curve_tracing_tool_common_h
#define mw_curve_tracing_tool_common_h
//:
//\file
//\brief common headers for curve_tracing_tool cxx files. NOT TO BE INSTALLED 
//\author Ricardo Fabbri (rfabbri), Brown University  (rfabbri@lems.brown.edu)
//\date 03/16/2006 03:46:47 PM EST
//

#include <vil/vil_image_resource.h> 
#include <vgui/vgui.h>
#include <vgui/vgui_projection_inspector.h>
#include <vgui/vgui_style.h>
#include <bvis1/bvis1_manager.h>
#include <bvis1/bvis1_view_tableau.h>
#include <vidpro1/vidpro1_repository.h>
#include <bpro1/bpro1_storage_sptr.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_image_storage.h>

#include <dvpgl/pro/dvpgl_camera_storage.h>

#include <vcl_set.h>
#include <vnl/vnl_math.h>

#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_polyline_2d_sptr.h>
#include <vsol/vsol_spatial_object_2d_sptr.h>
#include <vsol/vsol_digital_curve_2d.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_box_2d.h>

#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_line_2d.h>
#include <vgl/vgl_distance.h>

#include <dbdif/dbdif_rig.h>
#include <mw/mw_intersection_sets.h>
#include <mw/mw_dist.h>
#include <mw/mw_sift_curve.h>
#include <mw/algo/mw_sift_curve_algo.h>

#define MANAGER bvis1_manager::instance()
#define RICK_INVALID_CURVE_ID ((unsigned)-1)

#endif // mw_curve_tracing_tool_common_h

