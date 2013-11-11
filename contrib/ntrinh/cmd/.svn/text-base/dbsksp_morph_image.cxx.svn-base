// This example displays 

#include <vgui/vgui.h> 
#include <vgui/vgui_easy2D_tableau.h> 
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_shell_tableau.h>
//
#include <dbgl/algo/dbgl_circ_arc.h>
#include <vnl/vnl_math.h>
#include <dbsksp/dbsksp_shapelet.h>
#include <dbgl/algo/dbgl_fit_circ_arc_spline.h>
#include <dbgl/algo/dbgl_closest_point.h>
#include <dbgl/algo/dbgl_eulerspiral.h>
#include <vcl_iostream.h>
//
#include <dbgui/dbgui_soview2D.h>
#include <dbgui/dbgui_utils.h>
#include "dbsksp_morph_image_utils.h"



// ----------------------------------------------------------------------------
// Draw various grid curves inside a shape fragment and at a junction
void draw_fragment_grid_curves(vgui_easy2D_tableau_sptr& easy2D)
{

  easy2D->set_foreground(1,1,1); 
  easy2D->set_line_width(1); 

  // Add a circle
  vgl_point_2d<double > center(200,200);
  double radius = 100;
  easy2D->add_circle(center.x(), center.y(), radius);

  // Add the three arcs
  // ptA
  double angleA = vnl_math::pi_over_2 * 0.7;
  vgl_point_2d<double > ptA = circle_pt(center, radius, angleA);
  vgl_vector_2d<double > nA (-vcl_cos(angleA), -vcl_sin(angleA));

  // ptB
  double angleB = vnl_math::pi * 7/6;
  vgl_point_2d<double > ptB = circle_pt(center, radius, angleB);
  vgl_vector_2d<double > nB (-vcl_cos(angleB), -vcl_sin(angleB));

  // ptC
  double angleC = vnl_math::pi * 11/6;
  vgl_point_2d<double > ptC = circle_pt(center, radius, angleC);
  vgl_vector_2d<double > nC (-vcl_cos(angleC), -vcl_sin(angleC));

  // display the three points
  easy2D->set_point_radius(4);
  easy2D->set_foreground(0, 0, 1);
  easy2D->add_point(ptA.x(), ptA.y());
  easy2D->add_point(ptB.x(), ptB.y());
  easy2D->add_point(ptC.x(), ptC.y());

  // construct the three arcs
  dbgl_circ_arc arc_AB;
  arc_AB.set_from(ptA, nA, ptB);
  
  dbgl_circ_arc arc_BC;
  arc_BC.set_from(ptB, nB, ptC);

  dbgl_circ_arc arc_CA;
  arc_CA.set_from(ptC, nC, ptA);


  // draw the three arcs
  easy2D->set_foreground(1,1,1); 
  easy2D->add(new dbgui_soview2D_circ_arc(arc_AB));
  easy2D->add(new dbgui_soview2D_circ_arc(arc_BC));
  easy2D->add(new dbgui_soview2D_circ_arc(arc_CA));

  // draw ten arcs from A to the opposite arc
  for (unsigned i = 1; i < 10; ++i)
  {
    double angle = angleB + i * (angleC - angleB)/10;
    vgl_point_2d<double > pt = circle_pt(center, radius, angle);
    dbgl_circ_arc arc;
    arc.set_from(ptA, nA, pt);
    easy2D->add(new dbgui_soview2D_circ_arc(arc));
  }
  




    
  // draw the curve connecting the mid points of the arcs from A to BC

  easy2D->set_foreground(1,1,1); 

  // draw the polyline with GL
  int pt_list_A_size = 51;
  float pt_list_A_x[51];
  float pt_list_A_y[51];

  for (unsigned i = 0; i <= 50; ++i)
  {
    double angle = angleB + i * (angleC - angleB)/50;
    vgl_point_2d<double > pt = circle_pt(center, radius, angle);
    dbgl_circ_arc arc;
    arc.set_from(ptA, nA, pt);
    vgl_point_2d<double > mid_pt = arc.point_at(0.5);

    pt_list_A_x[i] = mid_pt.x();
    pt_list_A_y[i] = mid_pt.y();
  }

  easy2D->set_foreground(0, 1, 0); 
  easy2D->add_linestrip(pt_list_A_size, pt_list_A_x, pt_list_A_y);

  
  

  
  // draw the curve connecting the mid points of the arcs from B to CA
  int pt_list_B_size = 51;
  float pt_list_B_x[51];
  float pt_list_B_y[51];

  for (unsigned i = 0; i <= 50; ++i)
  {
    double angle = angleC + i * (angleA + 2*vnl_math::pi - angleC)/50;
    vgl_point_2d<double > pt = circle_pt(center, radius, angle);
    dbgl_circ_arc arc;
    arc.set_from(ptB, nB, pt);
    vgl_point_2d<double > mid_pt = arc.point_at(0.5);

    pt_list_B_x[i] = mid_pt.x();
    pt_list_B_y[i] = mid_pt.y();
  }

  easy2D->set_foreground(0, 1, 0); 
  easy2D->add_linestrip(pt_list_B_size, pt_list_B_x, pt_list_B_y);


  // draw the curve connecting the mid points of the arcs from C to AB
  int pt_list_C_size = 51;
  float pt_list_C_x[51];
  float pt_list_C_y[51];

  for (unsigned i = 0; i <= 50; ++i)
  {
    double angle = angleA + i * (angleB - angleA)/50;
    vgl_point_2d<double > pt = circle_pt(center, radius, angle);
    dbgl_circ_arc arc;
    arc.set_from(ptC, nC, pt);
    vgl_point_2d<double > mid_pt = arc.point_at(0.5);

    pt_list_C_x[i] = mid_pt.x();
    pt_list_C_y[i] = mid_pt.y();
  }

  easy2D->set_foreground(0, 1, 0); 
  easy2D->add_linestrip(pt_list_C_size, pt_list_C_x, pt_list_C_y);


  // Construct a fragment with interface AOB
  dbsksp_shapelet_sptr fragment_AB = new dbsksp_shapelet();
  double m0 = 0.5;
  double len = 200;
  double phi1 = vnl_math::pi * 2/3;
  vgl_vector_2d<double > bisect_AB ( vcl_cos( (angleA+angleB)/2), 
    vcl_sin( (angleA+angleB)/2 ));
  
  fragment_AB->set_from(center, radius, bisect_AB, (angleB-angleA)/2, m0, len, phi1);

  // draw the 2 boundary arcs
  dbgl_circ_arc arc_left = fragment_AB->bnd_arc(0);
  dbgl_circ_arc arc_right = fragment_AB->bnd_arc(1);

  easy2D->set_foreground(1, 1, 0);
  easy2D->add(new dbgui_soview2D_circ_arc(arc_left));
  easy2D->add(new dbgui_soview2D_circ_arc(arc_right));


  // draw the grid arcs of the fragment
  easy2D->set_foreground(1, 1, 1);

  dbgl_conic_arc conic = fragment_AB->shock_geom();
  for (unsigned i = 0; i <= 10; ++i)
  {
    double t = i * 1.0/10;
    vgl_point_2d<double > shock_pt = conic.point_at(t);
    double phi = fragment_AB->phi_at(t);
    vgl_vector_2d<double > shock_tangent = conic.tangent_at(t);

    //
    vgl_vector_2d<double > n_left = rotated(shock_tangent, phi);
    vgl_vector_2d<double > n_right = rotated(shock_tangent, -phi);

    // boundary point
    vgl_point_2d<double > bnd_pt_left = shock_pt + fragment_AB->radius_at(t) * n_left;
    vgl_point_2d<double > bnd_pt_right = shock_pt + fragment_AB->radius_at(t) * n_right;

    // grid arcs
    dbgl_circ_arc grid_arc;
    grid_arc.set_from(bnd_pt_left, -n_left, bnd_pt_right);

    easy2D->add(new dbgui_soview2D_circ_arc(grid_arc));
    
  
  }




  // draw the curve connecting the mid points of the grid arcs
  
  int n = 51;
  float pt_list_x[51];
  float pt_list_y[51];

  for (unsigned i = 0; i <= 50; ++i)
  {
    double t = i * 1.0/50;
    vgl_point_2d<double > shock_pt = conic.point_at(t);
    double phi = fragment_AB->phi_at(t);
    vgl_vector_2d<double > shock_tangent = conic.tangent_at(t);

    //
    vgl_vector_2d<double > n_left = rotated(shock_tangent, phi);
    vgl_vector_2d<double > n_right = rotated(shock_tangent, -phi);

    // boundary point
    vgl_point_2d<double > bnd_pt_left = shock_pt + fragment_AB->radius_at(t) * n_left;
    vgl_point_2d<double > bnd_pt_right = shock_pt + fragment_AB->radius_at(t) * n_right;

    // grid arcs
    dbgl_circ_arc grid_arc;
    grid_arc.set_from(bnd_pt_left, -n_left, bnd_pt_right);

    // mid point of the arc
    vgl_point_2d<double > pt = grid_arc.point_at(0.5);

    pt_list_x[i] = pt.x();
    pt_list_y[i] = pt.y();
  }

  easy2D->set_foreground(0, 1, 0);
  easy2D->add_linestrip(n, pt_list_x, pt_list_y);











  
  // Coordinate system inside a circular triangle
  double tAB = 0.5;
  double tBC = 0.6;
  double tCA = -1;

  // arc from C to a point between AB
  vgl_point_2d<double > mAB = arc_AB.point_at(tAB);
  dbgl_circ_arc arc_C_mAB;
  arc_C_mAB.set_from(ptC, nC, mAB);


  // arc from A to a point between BC
  vgl_point_2d<double > mBC = arc_BC.point_at(tBC);
  dbgl_circ_arc arc_A_mBC;
  arc_A_mBC.set_from(ptA, nA, mBC);


  easy2D->set_foreground(0, 1, 0);
  easy2D->add(new dbgui_soview2D_circ_arc(arc_C_mAB));
  easy2D->add(new dbgui_soview2D_circ_arc(arc_A_mBC));


  // Intersection of the two arcs
  vcl_vector<double > ratio_AB, ratio_BC;
  dbgl_closest_point::circular_arc_to_circular_arc(
    arc_C_mAB.start(), arc_C_mAB.end(), arc_C_mAB.k(),
    arc_A_mBC.start(), arc_A_mBC.end(), arc_A_mBC.k(),
    ratio_AB, ratio_BC);



  if (ratio_AB.size() != 1)
  {
    vcl_cout << "ERROR: number of intersection points is not 1.\n";
  }
  else
  {
    easy2D->set_foreground(1, 0, 0);
    vgl_point_2d<double >  pt = arc_C_mAB.point_at(ratio_AB[0]);
    easy2D->add_point(pt.x(), pt.y());

    dbgl_circ_arc arc_B0;
    arc_B0.set_from(ptB, nB, pt);

    // double the length of arc_B0 so that in intersects with CA
    dbgl_circ_arc arc_B1(arc_B0.start(), arc_B0.point_at(2.0), arc_B0.k());
    easy2D->add(new dbgui_soview2D_circ_arc(arc_B1));


    vcl_vector<double > ratio_CA, ratio_B;
    double d = dbgl_closest_point::circular_arc_to_circular_arc(
      arc_CA.start(), arc_CA.end(), arc_CA.k(),
      arc_B1.start(), arc_B1.end(), arc_B1.k(),
      ratio_CA, ratio_B);

    if (d != 0)
    {
      vcl_cout << "ERROR: the two arcs don't intersect\n";
    }
    else
    {
      tCA = ratio_CA[0];
      vgl_point_2d<double > pt_CA = arc_CA.point_at(tCA);
      easy2D->add_point(pt_CA.x(), pt_CA.y());

      vcl_cout << "tCA = " << tCA << "\n";
    }
    


  }
}


// -----------------------------------------------------------------------------
//: Interpolate a sine curve using circular arc spline
void draw_interp_circ_arc_spline(vgui_easy2D_tableau_sptr& easy2D)
{

  // Draw a sine curve and circular arc interpolation
  // y = 100* sin(2*pi * t);
  // t = x / 100;

  vcl_vector<vgl_point_2d<double > > pts;
  for (unsigned i =0; i <= 50; ++i)
  {
    double t = i * (1.0/50);
    double x = 100 * t;
    double y = 100 * vcl_sin(2 * vnl_math::pi * t);
    pts.push_back(vgl_point_2d<double > (x, y));
  }

  // draw the sine curve
  //dbsksp_draw_polyline(easy2D, pts);

  // draw the points
  easy2D->set_point_radius(3);
  dbsksp_draw_points(easy2D, pts);

  // Interpolate these curves with a polyarc
  
  //: Interpolate a set of points with a circular arc spline
  vcl_vector<dbgl_circ_arc > arc_list;
  double tol = 2;
  bool success = dbgl_fit_circ_arc_spline_to_polyline(arc_list, pts, tol);
  

  // display the arcs
  easy2D->set_foreground(1, 0, 0);
  for (unsigned i =0; i < arc_list.size(); ++i)
  {
    easy2D->add(new dbgui_soview2D_circ_arc(arc_list[i]));
  }

  easy2D->add_point(arc_list[0].start().x(), arc_list[0].start().y());
}









// -----------------------------------------------------------------------------
// Display a circle inside a shape fragment
// Then deform the shape fragment to see the circle changes. (need to do in Matlab)
void draw_shape_frags(const vgui_easy2D_tableau_sptr& easy2D)
{
  vgl_point_2d<double > point_A(400, 400);
  vgl_vector_2d<double > tangent_A(1, 0);
  double chord_length = 400;
  double param_m = 0;
  double phi_A = vnl_math::pi * 7/12;
  double phi_B = vnl_math::pi * 8/12;
  double radius_A = 100;

  dbsksp_shapelet_sptr source_frag = new dbsksp_shapelet;
  source_frag->set_from(point_A, radius_A, tangent_A, 
    phi_A, param_m, chord_length, phi_B);



  // extract boundary points
  vcl_vector<vgl_point_2d<double > > source_pts;
  dbsksp_shape_frag_bnd_pts(source_frag, source_pts);

  // display the points
  easy2D->set_foreground(0, 0, 1);
  dbsksp_draw_polyline(easy2D, source_pts);

  easy2D->set_foreground(0, 1, 0);
  dbsksp_draw_points(easy2D, source_pts);


  // Draw a circle as a polyline
  vgl_point_2d<double > circle_center(600, 400);
  double circle_radius = 100;

  vcl_vector<vgl_point_2d<double > > circle_pts;
  for (int i = 0; i < 100; ++i)
  {
    double t = i * (1.0/ 100) * (2*vnl_math::pi);
    vgl_point_2d<double > pt = circle_center + circle_radius * 
      vgl_vector_2d<double >(vcl_cos(t), vcl_sin(t));
    circle_pts.push_back(pt);
  }

  easy2D->set_foreground(1, 1, 0);
  dbsksp_draw_polyline(easy2D, circle_pts);

  easy2D->set_foreground(0, 0, 1);
  dbsksp_draw_points(easy2D, circle_pts);


  // the deformed shape fragment
  dbsksp_shapelet_sptr deformed_frag = new dbsksp_shapelet();
  deformed_frag->set_from(point_A, radius_A, tangent_A, phi_A, param_m + 0.5, 
    chord_length, phi_B + vnl_math::pi / 24);

  // Collect the boundary points of deformed_fragment
  vcl_vector<vgl_point_2d<double > > deformed_pts;
  dbsksp_shape_frag_bnd_pts(deformed_frag, deformed_pts);

  
  // display the points
  easy2D->set_foreground(1, 0, 0);
  dbsksp_draw_polyline(easy2D, deformed_pts);

  easy2D->set_foreground(0, 0, 1);
  dbsksp_draw_points(easy2D, deformed_pts);
 
  // output the pts to a text file
  vcl_string output_folder = "D:/vision/data/interp-image/";
  vcl_string source_frag_file = "source_frag.txt";
  vcl_string deformed_frag_file = "deformed_frag.txt";
  vcl_string circle_file = "circle.txt";


  // write
  dbsksp_write_points(output_folder + source_frag_file, source_pts);
  dbsksp_write_points(output_folder + deformed_frag_file, deformed_pts);
  dbsksp_write_points(output_folder + circle_file, circle_pts);

}



//: Draw a coordinate system inside a fragment
// horizontal curves: euler spiral
// vertical curves: connecting 
void draw_frag_coord_system(const vgui_easy2D_tableau_sptr& easy2D)
{
  easy2D->set_line_width(2);
  easy2D->set_point_radius(4);
  easy2D->set_foreground(1, 1, 0);


  vgl_point_2d<double > point_A(400, 400);
  vgl_vector_2d<double > tangent_A(1, 0);
  double chord_length = 400;
  double param_m = 0.0;
  double phi_A = vnl_math::pi * 7/12;
  double phi_B = vnl_math::pi * 8/12;
  double radius_A = 100;

  dbsksp_shapelet_sptr source_frag = new dbsksp_shapelet;
  source_frag->set_from(point_A, radius_A, tangent_A, 
    phi_A, param_m, chord_length, phi_B);

  // draw left and right circular arcs
  dbgl_circ_arc bnd_arc_left = source_frag->bnd_arc_left();
  dbgl_circ_arc bnd_arc_right = source_frag->bnd_arc_right();
  
  easy2D->add(new dbgui_soview2D_circ_arc(bnd_arc_left));
  easy2D->add(new dbgui_soview2D_circ_arc(bnd_arc_right));


  // draw front and back circular arcs
  dbgl_circ_arc bnd_arc_front;
  dbgl_circ_arc bnd_arc_back;

  bnd_arc_front.set_from(bnd_arc_right.start(), source_frag->start()-bnd_arc_right.start(),
    bnd_arc_left.start());
  bnd_arc_back.set_from(bnd_arc_right.end(), source_frag->end()-bnd_arc_right.end(),
    bnd_arc_left.end());
  
  easy2D->add(new dbgui_soview2D_circ_arc(bnd_arc_front));
  easy2D->add(new dbgui_soview2D_circ_arc(bnd_arc_back));

  // draw the horizontal grid curves. They are Euler spirals
  vcl_vector<dbgl_eulerspiral > horz_grid_curves;
  int num_horz_segs = 100;

  int num_vert_segs = 100;
  vcl_vector<vcl_vector<vgl_point_2d<double > > > vert_grid_curves;

  dbsksp_compute_grid_curves(source_frag, num_horz_segs, num_vert_segs, 
    horz_grid_curves, vert_grid_curves);


  // draw the grid
  int num_skip_horz = 10;
  int num_skip_vert = 10;
  dbsksp_draw_grid_curves(easy2D, num_skip_horz, num_skip_vert,
    horz_grid_curves, vert_grid_curves);


  // Draw a curve inside the fragment.
  // The preimage of this curve is a line
  // v = 0.5*u - 0.25;

  // make this a curve of 51 points
  vcl_vector<int > u_index_list;
  vcl_vector<int > v_index_list;
  int num_curve_segs = 50;
  for (int i = 0; i <= 50; ++i)
  {
    int u_index = 2*i;
    int v_index = i + 25;

    u_index_list.push_back(u_index);
    v_index_list.push_back(v_index);
  }

  vcl_vector<vgl_point_2d<double > > source_curve;
  for (int i =0; i <= num_curve_segs; ++i)
  {
    int u_index = u_index_list[i];
    int v_index = v_index_list[i];

    vgl_point_2d<double > pt = vert_grid_curves[u_index][v_index];
    source_curve.push_back(pt);
  }

  // draw the curve
  easy2D->set_foreground(1, 1, 0);
  dbsksp_draw_polyline(easy2D, source_curve);

  // Now deform this fragment and draw the curve again
  // the deformed shape fragment
  dbsksp_shapelet_sptr deformed_frag = new dbsksp_shapelet();
  deformed_frag->set_from(point_A, radius_A, tangent_A, phi_A, param_m + 0.5, 
    chord_length, phi_B + vnl_math::pi / 24);


  // draw the horizontal grid curves. They are Euler spirals
  vcl_vector<dbgl_eulerspiral > deformed_horz_grid_curves;
  vcl_vector<vcl_vector<vgl_point_2d<double > > > deformed_vert_grid_curves;

  dbsksp_compute_grid_curves(deformed_frag, num_horz_segs, num_vert_segs, 
    deformed_horz_grid_curves, deformed_vert_grid_curves);

  //// draw the deformed grid
  //dbsksp_draw_grid_curves(easy2D, num_skip_horz, num_skip_vert,
  //  deformed_horz_grid_curves, deformed_vert_grid_curves);

  // compute the deformed curve
  vcl_vector<vgl_point_2d<double > > deformed_curve;
  for (int i =0; i <= num_curve_segs; ++i)
  {
    int u_index = u_index_list[i];
    int v_index = v_index_list[i];

    vgl_point_2d<double > pt = deformed_vert_grid_curves[u_index][v_index];
    deformed_curve.push_back(pt);
  }

  // draw the curve
  easy2D->set_foreground(1, 0, 0.5);
  dbsksp_draw_polyline(easy2D, deformed_curve);
}





//: Draw a coordinate system inside a fragment
// horizontal curves: euler spiral
// vertical curves: connecting 
void draw_two_attached_frag_coord_system(const vgui_easy2D_tableau_sptr& easy2D)
{
  easy2D->set_line_width(2);
  easy2D->set_point_radius(4);
  easy2D->set_foreground(1, 1, 0);


  vgl_point_2d<double > point_A(200, 0);
  vgl_vector_2d<double > tangent_A(1, 0);
  double len_A = 300;
  double m_A = 0.0;
  double phi_A = vnl_math::pi * 7/12;
  double phi_B = vnl_math::pi * 8/12;
  double radius_A = 100;

  // second fragment
  double len_B = 500;
  double m_B = 0.0;
  double phi_C = vnl_math::pi * 9/12;

  dbsksp_shapelet_sptr source_frag_A = new dbsksp_shapelet();
  source_frag_A->set_from(point_A, radius_A, tangent_A, 
    phi_A, m_A, len_A, phi_B);



  // draw left and right circular arcs
  dbgl_circ_arc bnd_arc_left = source_frag_A->bnd_arc_left();
  dbgl_circ_arc bnd_arc_right = source_frag_A->bnd_arc_right();
  
  //easy2D->add(new dbgui_soview2D_circ_arc(bnd_arc_left));
  //easy2D->add(new dbgui_soview2D_circ_arc(bnd_arc_right));

  // draw front and back circular arcs
  dbgl_circ_arc bnd_arc_front;
  dbgl_circ_arc bnd_arc_back;

  bnd_arc_front.set_from(bnd_arc_right.start(), source_frag_A->start()-bnd_arc_right.start(),
    bnd_arc_left.start());
  bnd_arc_back.set_from(bnd_arc_right.end(), source_frag_A->end()-bnd_arc_right.end(),
    bnd_arc_left.end());
  
  

  dbsksp_shapelet_sptr source_frag_B = new dbsksp_shapelet();
  if (!source_frag_B->set_from(source_frag_A->end(), source_frag_A->radius_end(), 
    source_frag_A->tangent_end(), source_frag_A->phi_end(), m_B, len_B, phi_C))
  {
    vcl_cout << "ERROR: could not construct source_frag_B.\n";
  }


  // draw the horizontal grid curves. They are Euler spirals
  int num_horz_segs = 100;
  int num_vert_segs = 100;

  int num_skip_horz = 10;
  int num_skip_vert = 10;


  vcl_vector<dbgl_eulerspiral > horz_grid_curves_A;
  vcl_vector<vcl_vector<vgl_point_2d<double > > > vert_grid_curves_A;

  dbsksp_compute_grid_curves(source_frag_A, num_horz_segs, num_vert_segs, 
    horz_grid_curves_A, vert_grid_curves_A);

  // draw the grid
  dbsksp_draw_grid_curves(easy2D, num_skip_horz, num_skip_vert,
    horz_grid_curves_A, vert_grid_curves_A);

  // second fragment
  vcl_vector<dbgl_eulerspiral > horz_grid_curves_B;
  vcl_vector<vcl_vector<vgl_point_2d<double > > > vert_grid_curves_B;

  dbsksp_compute_grid_curves(source_frag_B, num_horz_segs, num_vert_segs, 
    horz_grid_curves_B, vert_grid_curves_B);

  // draw the grid
  dbsksp_draw_grid_curves(easy2D, num_skip_horz, num_skip_vert,
    horz_grid_curves_B, vert_grid_curves_B);

  easy2D->set_foreground(1, 1, 0);
  easy2D->add(new dbgui_soview2D_circ_arc(bnd_arc_front));
  easy2D->add(new dbgui_soview2D_circ_arc(bnd_arc_back));


  // Draw a curve inside the fragment.
  // The preimage of this curve is a line
  // v = 0.5*u - 0.25;

  // make this a curve of 51 points
  vcl_vector<int > u_index_list_A;
  vcl_vector<int > v_index_list_A;
  int num_curve_segs_A = 50;
  for (int i = 0; i <= 50; ++i)
  {
    int u_index = 2*i;
    int v_index = i + 20;

    u_index_list_A.push_back(u_index);
    v_index_list_A.push_back(v_index);
  }

  vcl_vector<vgl_point_2d<double > > source_curve_A;
  for (unsigned i =0; i < u_index_list_A.size(); ++i)
  {
    int u_index = u_index_list_A[i];
    int v_index = v_index_list_A[i];

    vgl_point_2d<double > pt = vert_grid_curves_A[u_index][v_index];
    source_curve_A.push_back(pt);
  }

  // draw the curve
  easy2D->set_foreground(0.5, 1, 1);
  dbsksp_draw_polyline(easy2D, source_curve_A);


  // the curve on the second fragment

  // make this a curve of 51 points
  vcl_vector<int > u_index_list_B;
  vcl_vector<int > v_index_list_B;
  int num_curve_segs_B = 25;
  for (int i = 0; i <= 25; ++i)
  {
    int u_index = 2*i;
    int v_index = i + 70;

    u_index_list_B.push_back(u_index);
    v_index_list_B.push_back(v_index);
  }

  vcl_vector<vgl_point_2d<double > > source_curve_B;
  for (unsigned i =0; i < u_index_list_B.size(); ++i)
  {
    int u_index = u_index_list_B[i];
    int v_index = v_index_list_B[i];

    vgl_point_2d<double > pt = vert_grid_curves_B[u_index][v_index];
    source_curve_B.push_back(pt);
  }

  // draw the curve
  easy2D->set_foreground(0.5f, 1.0f, 0.3f);
  dbsksp_draw_polyline(easy2D, source_curve_B);


  // Now deform this fragment and draw the curve again
  // the deformed shape fragment
  dbsksp_shapelet_sptr deformed_frag_A = new dbsksp_shapelet();
  deformed_frag_A->set_from(point_A + vgl_vector_2d<double >(800, 0), radius_A, 
    tangent_A, phi_A, m_A + 0.2, len_A, phi_B + vnl_math::pi / 24);


  // draw the horizontal grid curves. They are Euler spirals
  vcl_vector<dbgl_eulerspiral > deformed_horz_grid_curves_A;
  vcl_vector<vcl_vector<vgl_point_2d<double > > > deformed_vert_grid_curves_A;

  dbsksp_compute_grid_curves(deformed_frag_A, num_horz_segs, num_vert_segs, 
    deformed_horz_grid_curves_A, deformed_vert_grid_curves_A);

  // draw the deformed grid
  dbsksp_draw_grid_curves(easy2D, num_skip_horz, num_skip_vert,
    deformed_horz_grid_curves_A, deformed_vert_grid_curves_A);

  // the boundary
  // draw left and right circular arcs
  dbgl_circ_arc deformed_bnd_arc_left_A = deformed_frag_A->bnd_arc_left();
  dbgl_circ_arc deformed_bnd_arc_right_A = deformed_frag_A->bnd_arc_right();
  
  //easy2D->add(new dbgui_soview2D_circ_arc(bnd_arc_left));
  //easy2D->add(new dbgui_soview2D_circ_arc(bnd_arc_right));

  // draw front and back circular arcs
  dbgl_circ_arc deformed_bnd_arc_front_A;
  dbgl_circ_arc deformed_bnd_arc_back_A;

  deformed_bnd_arc_front_A.set_from(deformed_bnd_arc_right_A.start(), 
    deformed_frag_A->start()-deformed_bnd_arc_right_A.start(),
    deformed_bnd_arc_left_A.start());

  deformed_bnd_arc_back_A.set_from(deformed_bnd_arc_right_A.end(), 
    deformed_frag_A->end()-deformed_bnd_arc_right_A.end(),
    deformed_bnd_arc_left_A.end());

  


  // deformed second fragment
  dbsksp_shapelet_sptr deformed_frag_B = new dbsksp_shapelet();
  if (!deformed_frag_B->set_from(deformed_frag_A->end(), deformed_frag_A->radius_end(), 
    deformed_frag_A->tangent_end(), deformed_frag_A->phi_end(), 
    m_B + 0.3, len_B + 400, phi_C - vnl_math::pi / 12))
  {
    vcl_cout << "ERROR: could not construct source_frag_B.\n";
  }

  // grid curves for the second fragment
  vcl_vector<dbgl_eulerspiral > deformed_horz_grid_curves_B;
  vcl_vector<vcl_vector<vgl_point_2d<double > > > deformed_vert_grid_curves_B;

  dbsksp_compute_grid_curves(deformed_frag_B, num_horz_segs, num_vert_segs, 
    deformed_horz_grid_curves_B, deformed_vert_grid_curves_B);

  // draw the deformed grid
  dbsksp_draw_grid_curves(easy2D, num_skip_horz, num_skip_vert,
    deformed_horz_grid_curves_B, deformed_vert_grid_curves_B);

  easy2D->set_foreground(1, 1, 0);
  easy2D->add(new dbgui_soview2D_circ_arc(deformed_bnd_arc_front_A));
  easy2D->add(new dbgui_soview2D_circ_arc(deformed_bnd_arc_back_A));
  

  // compute the deformed curve
  vcl_vector<vgl_point_2d<double > > deformed_curve_A;
  for (int i =0; i <= num_curve_segs_A; ++i)
  {
    int u_index = u_index_list_A[i];
    int v_index = v_index_list_A[i];

    vgl_point_2d<double > pt = deformed_vert_grid_curves_A[u_index][v_index];
    deformed_curve_A.push_back(pt);
  }

  // draw the curve
  easy2D->set_foreground(0.5, 1, 1);
  dbsksp_draw_polyline(easy2D, deformed_curve_A);

  // deformed curve for second fragment
  // compute the deformed curve
  vcl_vector<vgl_point_2d<double > > deformed_curve_B;
  for (int i =0; i <= num_curve_segs_B; ++i)
  {
    int u_index = u_index_list_B[i];
    int v_index = v_index_list_B[i];

    vgl_point_2d<double > pt = deformed_vert_grid_curves_B[u_index][v_index];
    deformed_curve_B.push_back(pt);
  }

  // draw the curve
  easy2D->set_foreground(0.5f, 1.0f, 0.3f);
  dbsksp_draw_polyline(easy2D, deformed_curve_B);
}




// Draw a coordinate system for an A_1^3 fragment
void draw_A13_frag_coord_system(const vgui_easy2D_tableau_sptr& easy2D)
{
  easy2D->set_line_width(2);
  easy2D->set_point_radius(4);
  easy2D->set_foreground(1, 1, 0);


  vgl_point_2d<double > point_A(400, 400);
  vgl_vector_2d<double > tangent_A(1, 0);
  double chord_length = 400;
  double param_m = 0.2;
  double phi_A = vnl_math::pi * 4/12;
  double phi_B = vnl_math::pi * 5/12;
  double radius_A = 300;

  dbsksp_shapelet_sptr source_frag = new dbsksp_shapelet;
  source_frag->set_from(point_A, radius_A, tangent_A, 
    phi_A, param_m, chord_length, phi_B);

  // draw left and right circular arcs
  dbgl_circ_arc bnd_arc_left = source_frag->bnd_arc_left();
  dbgl_circ_arc bnd_arc_right = source_frag->bnd_arc_right();
  
  easy2D->add(new dbgui_soview2D_circ_arc(bnd_arc_left));
  easy2D->add(new dbgui_soview2D_circ_arc(bnd_arc_right));


  // draw front and back circular arcs
  dbgl_circ_arc bnd_arc_front;
  dbgl_circ_arc bnd_arc_back;

  bnd_arc_front.set_from(bnd_arc_right.start(), source_frag->start()-bnd_arc_right.start(),
    bnd_arc_left.start());
  bnd_arc_back.set_from(bnd_arc_right.end(), source_frag->end()-bnd_arc_right.end(),
    bnd_arc_left.end());
  
  //easy2D->add(new dbgui_soview2D_circ_arc(bnd_arc_front));

  easy2D->add(new dbgui_soview2D_circ_arc(bnd_arc_back));

  // the front is a polyline
  vcl_vector<vgl_point_2d<double > > bnd_poly_front;
  bnd_poly_front.push_back(bnd_arc_right.start());
  bnd_poly_front.push_back(source_frag->start());
  bnd_poly_front.push_back(bnd_arc_left.start());

  dbsksp_draw_polyline(easy2D, bnd_poly_front);
  

  // draw the horizontal grid curves. They are Euler spirals
  vcl_vector<dbgl_eulerspiral > horz_grid_curves;
  int num_horz_segs = 100;

  int num_vert_segs = 100;
  vcl_vector<vcl_vector<vgl_point_2d<double > > > vert_grid_curves;

  dbsksp_compute_A13_frag_grid_curves(source_frag, num_horz_segs, num_vert_segs, 
    horz_grid_curves, vert_grid_curves);


  // draw the grid
  int num_skip_horz = 10;
  int num_skip_vert = 10;
  dbsksp_draw_grid_curves(easy2D, num_skip_horz, num_skip_vert,
    horz_grid_curves, vert_grid_curves);


  // Draw a curve inside the fragment.
  // The preimage of this curve is a line
  // v = 0.5*u - 0.25;

  // make this a curve of 51 points
  vcl_vector<int > u_index_list;
  vcl_vector<int > v_index_list;
  int num_curve_segs = 50;
  for (int i = 0; i <= 50; ++i)
  {
    int u_index = 2*i;
    int v_index = i + 25;

    u_index_list.push_back(u_index);
    v_index_list.push_back(v_index);
  }

  vcl_vector<vgl_point_2d<double > > source_curve;
  for (int i =0; i <= num_curve_segs; ++i)
  {
    int u_index = u_index_list[i];
    int v_index = v_index_list[i];

    vgl_point_2d<double > pt = vert_grid_curves[u_index][v_index];
    source_curve.push_back(pt);
  }

  // draw the curve
  easy2D->set_foreground(1, 1, 0);
  dbsksp_draw_polyline(easy2D, source_curve);

  // Now deform this fragment and draw the curve again
  // the deformed shape fragment
  dbsksp_shapelet_sptr deformed_frag = new dbsksp_shapelet();
  deformed_frag->set_from(point_A + vgl_vector_2d<double >(600, 0), radius_A, 
    tangent_A, phi_A, param_m + 0.3, chord_length + 200, phi_B + vnl_math::pi / 24);


  // draw the horizontal grid curves. They are Euler spirals
  vcl_vector<dbgl_eulerspiral > deformed_horz_grid_curves;
  vcl_vector<vcl_vector<vgl_point_2d<double > > > deformed_vert_grid_curves;

  dbsksp_compute_A13_frag_grid_curves(deformed_frag, num_horz_segs, num_vert_segs, 
    deformed_horz_grid_curves, deformed_vert_grid_curves);

  // draw the deformed grid
  dbsksp_draw_grid_curves(easy2D, num_skip_horz, num_skip_vert,
    deformed_horz_grid_curves, deformed_vert_grid_curves);

  // compute the deformed curve
  vcl_vector<vgl_point_2d<double > > deformed_curve;
  for (int i =0; i <= num_curve_segs; ++i)
  {
    int u_index = u_index_list[i];
    int v_index = v_index_list[i];

    vgl_point_2d<double > pt = deformed_vert_grid_curves[u_index][v_index];
    deformed_curve.push_back(pt);
  }

  // draw the curve
  easy2D->set_foreground(1, 1, 0);
  dbsksp_draw_polyline(easy2D, deformed_curve);

  return;
}





int main(int argc, char **argv) 
{
  vgui::init(argc, argv);

  // Use an easy2D tableau which allows us to draw 2D objects: 
  vgui_easy2D_tableau_new easy2D("unnamed");

  ///////////////////////////////////////////////////////
  //draw_fragment_grid_curves(easy2D);
  ///////////////////////////////////////////////////////


  //////////////////////////////////////////////////
  // draw_interp_circ_arc_spline(easy2D);
  //////////////////////////////////////////////////

  //////////////////////////////////////////////////
  //draw_shape_frags(easy2D);
  //////////////////////////////////////////////////
  

  ////////////////////////////////////////////////////
  // draw_frag_coord_system(easy2D);
  ////////////////////////////////////////////////////


  ////////////////////////////////////////////////////
  // draw_two_attached_frag_coord_system(easy2D);
  ////////////////////////////////////////////////////


  //////////////////////////////////////////////////
  draw_A13_frag_coord_system(easy2D);
  //////////////////////////////////////////////////
  

  
  

  vgui_viewer2D_tableau_new viewer(easy2D);
  
  vgui_shell_tableau_new shell(viewer);
  
  //dbgui_utils::render_to_ps(viewer);
  

  return vgui::run(shell, 512, 512);
}

