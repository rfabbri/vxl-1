// This is /ntrinh/cmd/dbsksp_morph_image_utils.h

#ifndef dbsksp_morph_image_utils_h_
#define dbsksp_morph_image_utils_h_

//:
// \file
// \brief 
// 
// \author Nhon Trinh ( ntrinh@lems.brown.edu)
// \date April 28, 2008
//
// \verbatim
//  Modifications:
// \endverbatim



#include <vgl/vgl_point_2d.h>
#include <vgui/vgui_easy2D_tableau_sptr.h> 
#include <vcl_string.h>
#include <dbsksp/dbsksp_shapelet_sptr.h>
#include <dbgl/algo/dbgl_eulerspiral.h>


//: A point on a circle
vgl_point_2d<double > circle_pt(const vgl_point_2d<double >& center, 
                                double radius,
                                double angle);


// plot a polyline on an easy2D tableau
void dbsksp_draw_polyline(const vgui_easy2D_tableau_sptr& easy2D,
                          const vcl_vector<vgl_point_2d<double > >& pts);


// -----------------------------------------------------------------------------
// plot a polyline on an easy2D tableau
void dbsksp_draw_points(const vgui_easy2D_tableau_sptr& easy2D,
                          const vcl_vector<vgl_point_2d<double > >& pts);



// -----------------------------------------------------------------------------
// print a set of points to a file
bool dbsksp_write_points(const vcl_string& filename, 
                  const vcl_vector<vgl_point_2d<double > >& pts);


// --------------------------------------------------------------------
//: Sample the boundary of a fragment
void dbsksp_shape_frag_bnd_pts(const dbsksp_shapelet_sptr& source_frag, 
                        vcl_vector<vgl_point_2d<double > >& source_pts);



//: Compute the grid curves for a shape fragment using euler spirals
void dbsksp_compute_grid_curves(const dbsksp_shapelet_sptr& source_frag,
    int num_horz_segs, int num_vert_segs,
    vcl_vector<dbgl_eulerspiral >& horz_grid_curves,
    vcl_vector<vcl_vector<vgl_point_2d<double > > >& vert_grid_curves);


//: Plot the grid curves
void dbsksp_draw_grid_curves(const vgui_easy2D_tableau_sptr& easy2D,
                             int num_skip_horz,
                             int num_skip_vert,
  vcl_vector<dbgl_eulerspiral >& horz_grid_curves,
  vcl_vector<vcl_vector<vgl_point_2d<double > > >& vert_grid_curves);


//: Compute the grid curves for a shape fragment using euler spirals
void dbsksp_compute_A13_frag_grid_curves(
  const dbsksp_shapelet_sptr& source_frag,
    int num_horz_segs, int num_vert_segs,
    vcl_vector<dbgl_eulerspiral >& horz_grid_curves,
    vcl_vector<vcl_vector<vgl_point_2d<double > > >& vert_grid_curves);



#endif // basic/dbgl/algo/dbsksp_morph_image_utils.h

