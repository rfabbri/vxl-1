// This is basic/dbgl/algo/dbsksp_morph_image_utils.cxx
//:
// \file

#include "dbsksp_morph_image_utils.h"


//#include <vgui/vgui.h> 
#include <vgui/vgui_easy2D_tableau.h> 
//#include <vgui/vgui_viewer2D_tableau.h>
//#include <vgui/vgui_shell_tableau.h>
//
//#include <dbgl/algo/dbgl_circ_arc.h>
#include <vnl/vnl_math.h>
//#include <vgui/vgui_soview2D.h>
//
#include <dbsksp/dbsksp_shapelet.h>
//#include <dbgl/algo/dbgl_interp_arc_spline.h>
//#include <dbgl/algo/dbgl_closest_point.h>
//#include <vcl_iostream.h>
//
//#include <dbgui/dbgui_soview2D.h>
#include <vnl/vnl_vector.h>


//: A point on a circle
vgl_point_2d<double > circle_pt(const vgl_point_2d<double >& center, 
                                double radius,
                                double angle)
{
  return center + radius * vgl_vector_2d<double >(vcl_cos(angle), vcl_sin(angle));
}


// plot a polyline on an easy2D tableau
void dbsksp_draw_polyline(const vgui_easy2D_tableau_sptr& easy2D,
                          const vcl_vector<vgl_point_2d<double > >& pts)
{
  // draw the polyline with GL
  unsigned pt_list_size = pts.size();
  vnl_vector<float > pt_list_x(pt_list_size, 0);
  vnl_vector<float > pt_list_y(pt_list_size, 0);

  for (unsigned i = 0; i < pt_list_size; ++i)
  {
    pt_list_x[i] = pts[i].x();
    pt_list_y[i] = pts[i].y();
  }

  easy2D->add_linestrip(pt_list_size, pt_list_x.data_block(), pt_list_y.data_block());
}



// -----------------------------------------------------------------------------
// plot a polyline on an easy2D tableau
void dbsksp_draw_points(const vgui_easy2D_tableau_sptr& easy2D,
                          const vcl_vector<vgl_point_2d<double > >& pts)
{
  // draw the polyline with GL
  for (unsigned i = 0; i < pts.size(); ++i)
  {
    easy2D->add_point(pts[i].x(), pts[i].y());
  }
}




// -----------------------------------------------------------------------------
// print a set of points to a file
bool dbsksp_write_points(const vcl_string& filename, 
                  const vcl_vector<vgl_point_2d<double > >& pts)
{
  // create the file
  vcl_ofstream outfp(filename.c_str());
  if (!outfp)
  {
    vcl_cout << " Error writing file  " << filename << vcl_endl;
    return false;
  }
  
  // Write coordinates of the points
  for (unsigned int i=0; i<pts.size(); ++i)
  {
    vgl_point_2d<double > pt = pts[i];
    outfp <<pt.x() << " " << pt.y() << vcl_endl;
  }

  // 3) close the file
  outfp.close();

  return true;
}



// --------------------------------------------------------------------
//: Sample the boundary of a fragment
void dbsksp_shape_frag_bnd_pts(const dbsksp_shapelet_sptr& source_frag, 
                        vcl_vector<vgl_point_2d<double > >& source_pts)
{
  source_pts.clear();

  // get the set of corresponding points of the two arcs
  dbgl_circ_arc left_bnd = source_frag->bnd_arc_left();
  dbgl_circ_arc right_bnd = source_frag->bnd_arc_right();

  // list of boundary points
  for (int i = 0; i <= 10; ++i)
  {
    double t = i * 1.0 / 10;
    vgl_point_2d<double > pt = right_bnd.point_at(t);
    source_pts.push_back(pt);
  }
  source_pts.push_back(source_frag->end());

  for (int i = 10; i >= 0; --i)
  {
    double t = i * 1.0 / 10;
    vgl_point_2d<double > pt = left_bnd.point_at(t);
    source_pts.push_back(pt);
  }
  source_pts.push_back(source_frag->start());

  return;
}





void dbsksp_compute_grid_curves(const dbsksp_shapelet_sptr& source_frag,
    int num_horz_segs, int num_vert_segs,
    vcl_vector<dbgl_eulerspiral >& horz_grid_curves,
    vcl_vector<vcl_vector<vgl_point_2d<double > > >& vert_grid_curves)
{
  horz_grid_curves.clear();
  vert_grid_curves.clear();


  // draw left and right circular arcs
  dbgl_circ_arc bnd_arc_left = source_frag->bnd_arc_left();
  dbgl_circ_arc bnd_arc_right = source_frag->bnd_arc_right();

  // draw front and back circular arcs
  dbgl_circ_arc bnd_arc_front;
  dbgl_circ_arc bnd_arc_back;

  bnd_arc_front.set_from(bnd_arc_right.start(), source_frag->start()-bnd_arc_right.start(),
    bnd_arc_left.start());
  bnd_arc_back.set_from(bnd_arc_right.end(), source_frag->end()-bnd_arc_right.end(),
    bnd_arc_left.end());



  // draw the horizontal grid curves. They are Euler spirals
  for (int i =0; i <= num_horz_segs; ++i)
  {
    double v = (1.0 / num_horz_segs) * i - 0.5;
    
    // start and end points
    vgl_point_2d<double > es_start = bnd_arc_front.point_at(v + 0.5);
    vgl_point_2d<double > es_end = bnd_arc_back.point_at(v + 0.5);

    // start and end tangents
    vgl_vector_2d<double > es_start_tangent = 
      rotated(bnd_arc_front.tangent_at(v+0.5), -vnl_math::pi_over_2);
    vgl_vector_2d<double > es_end_tangent = 
      rotated(bnd_arc_back.tangent_at(v+0.5), -vnl_math::pi_over_2);

    dbgl_eulerspiral es(es_start, vcl_atan2(es_start_tangent.y(), es_start_tangent.x()),
      es_end, vcl_atan2(es_end_tangent.y(), es_end_tangent.x()));

    horz_grid_curves.push_back(es);
  }

  // Now draw the vertical grid curves
  double L0 = bnd_arc_front.length();
  double L1 = bnd_arc_back.length();
  for (int i = 0; i <= num_vert_segs; ++i)
  {
    double u = (1.0 / num_vert_segs) * i;
    
    vcl_vector<vgl_point_2d<double > > vert_curve;
    for (int j =0; j <= num_horz_segs; ++j)
    {
      dbgl_eulerspiral es = horz_grid_curves[j];

      // convert from parameter u to arc length
      double L = es.length();

      // polynomial parameters
      double a = -2*L + L0 + L1;
      double b = 3*L - 2*L0 - L1;
      double c = L0;
      double d = 0;

      // arclength - polynomial s(u) = a *u^3 + b *u^2 + c *u + d
      double s = d + u *(c + u *(b + u*a));
      vert_curve.push_back(es.point_at_length(s));    
    }
    vert_grid_curves.push_back(vert_curve);
  }


}





//: Plot the grid curves
void dbsksp_draw_grid_curves(const vgui_easy2D_tableau_sptr& easy2D,
                             int num_skip_horz,
                             int num_skip_vert,
  vcl_vector<dbgl_eulerspiral >& horz_grid_curves,
  vcl_vector<vcl_vector<vgl_point_2d<double > > >& vert_grid_curves)
{
  // draw the euler spirals
  easy2D->set_foreground(0, 1, 0);
  for (unsigned int i =0; i < horz_grid_curves.size(); i = i + num_skip_horz)
  {
    dbgl_eulerspiral es = horz_grid_curves[i];
    vcl_vector<vgl_point_2d<double > > pts;
    
    double es_len = es.length();
    int num_es_segs = 100;
    for (int j = 0; j <= num_es_segs; ++j)
    {
      double s = j * (es_len / num_es_segs);
      pts.push_back(es.point_at_length(s));
    }
    dbsksp_draw_polyline(easy2D, pts);
  }

  // Now draw the vertical grid curves

  easy2D->set_foreground(1, 0, 0.5);
  for (unsigned int i =0; i < vert_grid_curves.size(); i = i + num_skip_vert)
  {
    vcl_vector<vgl_point_2d<double > > vert_curve = vert_grid_curves[i];
    dbsksp_draw_polyline(easy2D, vert_curve);
  }

}





//: Compute the grid curves for a shape fragment using euler spirals
void dbsksp_compute_A13_frag_grid_curves(
  const dbsksp_shapelet_sptr& source_frag,
    int num_horz_segs, int num_vert_segs,
    vcl_vector<dbgl_eulerspiral >& horz_grid_curves,
    vcl_vector<vcl_vector<vgl_point_2d<double > > >& vert_grid_curves)
{
  horz_grid_curves.clear();
  vert_grid_curves.clear();


  // draw left and right circular arcs
  dbgl_circ_arc bnd_arc_left = source_frag->bnd_arc_left();
  dbgl_circ_arc bnd_arc_right = source_frag->bnd_arc_right();

  // draw front and back circular arcs
  dbgl_circ_arc bnd_arc_front;
  dbgl_circ_arc bnd_arc_back;

  bnd_arc_front.set_from(bnd_arc_right.start(), source_frag->start()-bnd_arc_right.start(),
    bnd_arc_left.start());
  bnd_arc_back.set_from(bnd_arc_right.end(), source_frag->end()-bnd_arc_right.end(),
    bnd_arc_left.end());



  // draw the horizontal grid curves. They are Euler spirals
  for (int i =0; i <= num_horz_segs; ++i)
  {
    double v = (1.0 / num_horz_segs) * i - 0.5;
    
    // start and end points
    // start point is on the contact shock
    // start tangent forms with the contact shock an angle linearly interpolated from
    // pi/2 to phi_A
    vgl_point_2d<double > es_start;
    vgl_vector_2d<double > es_start_tangent;
    if (v >=0)
    {
      es_start = source_frag->start() + 2*v * (bnd_arc_left.start() - source_frag->start());
      
      vgl_vector_2d<double > n = normalized(bnd_arc_left.start() - source_frag->start());




      // START HERE: set angle = pi / 2 for all curves











      double angle = source_frag->phi_start() + 
        (2*v) * (vnl_math::pi_over_2 - source_frag->phi_start());
      
      es_start_tangent = rotated(n, -angle);
    }
    else
    {
      es_start = source_frag->start() + (-2*v) * (bnd_arc_right.start() - source_frag->start());

      vgl_vector_2d<double > n = normalized(bnd_arc_right.start() - source_frag->start());
      double angle = source_frag->phi_start() + 
        (-2*v) * (vnl_math::pi_over_2 - source_frag->phi_start());

      

      es_start_tangent = rotated(n, angle);
    }
    
    // end point and tangent
    vgl_point_2d<double > es_end = bnd_arc_back.point_at(v + 0.5);
    vgl_vector_2d<double > es_end_tangent = 
      rotated(bnd_arc_back.tangent_at(v+0.5), -vnl_math::pi_over_2);

    dbgl_eulerspiral es(es_start, vcl_atan2(es_start_tangent.y(), es_start_tangent.x()),
      es_end, vcl_atan2(es_end_tangent.y(), es_end_tangent.x()));

    horz_grid_curves.push_back(es);
  }

  // Now draw the vertical grid curves
  double L0 = bnd_arc_front.length()/2;
  double L1 = bnd_arc_back.length()/2;
  for (int i = 0; i <= num_vert_segs; ++i)
  {
    double u = (1.0 / num_vert_segs) * i;
    
    vcl_vector<vgl_point_2d<double > > vert_curve;
    for (int j =0; j <= num_horz_segs; ++j)
    {
      dbgl_eulerspiral es = horz_grid_curves[j];

      // convert from parameter u to arc length
      double L = es.length();

      // polynomial parameters
      double a = -2*L + L0 + L1;
      double b = 3*L - 2*L0 - L1;
      double c = L0;
      double d = 0;

      // arclength - polynomial s(u) = a *u^3 + b *u^2 + c *u + d
      double s = d + u *(c + u *(b + u*a));
      vert_curve.push_back(es.point_at_length(s));    
    }
    vert_grid_curves.push_back(vert_curve);
  }

  return;
}











