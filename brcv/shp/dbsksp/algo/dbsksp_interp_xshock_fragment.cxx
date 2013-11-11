// This is file shp/dbsksp/algo/dbsksp_interp_xshock_fragment.cxx

//:
// \file

#include "dbsksp_interp_xshock_fragment.h"

#include <dbsksp/dbsksp_shapelet.h>
#include <dbsksp/dbsksp_xshock_fragment.h>
#include <dbgl/algo/dbgl_compute_symmetry_point.h>
#include <vgl/vgl_distance.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_cost_function.h>
#include <vnl/algo/vnl_brent_minimizer.h>

#include <vnl/vnl_least_squares_function.h>
#include <vnl/vnl_matrix.h>
#include <vnl/algo/vnl_levenberg_marquardt.h>
#include <dbnl/dbnl_math.h>
#include <dbnl/dbnl_angle.h>

#include <dbsksp/algo/dbsksp_interp_xfrag_cost_function.h>



//------------------------------------------------------------------------------
//: Interpolate an extrinsic fragment using at most 3 shapelets
void dbsksp_interp_xfrag_with_max_three_shapelets(const dbsksp_xshock_node_descriptor& start,
                                        const dbsksp_xshock_node_descriptor& end,
                                        vcl_vector<dbsksp_shapelet_sptr >& list_shapelet)
{
  // sanitize output storage
  list_shapelet.clear();

  // Interpolate each boundary with a biarc
  // Then form shapelets from the mid-points of the biarcs. There will be at most 3 shapelets formed.
  dbsksp_xshock_fragment xfrag(start, end);
  dbgl_biarc left_bnd = xfrag.bnd_left_as_biarc();
  dbgl_biarc right_bnd = xfrag.bnd_right_as_biarc();

  // determine the end point of the first fragment
  dbgl_circ_arc left_arc1;
  left_arc1.set_from(left_bnd.start(), left_bnd.tangent_at(0), left_bnd.k1(), left_bnd.len1());

  dbgl_circ_arc left_arc2;
  left_arc2.set_from(left_arc1.end(), left_arc1.tangent_at_end(), left_bnd.k2(), left_bnd.len2());

  dbgl_circ_arc right_arc1;
  right_arc1.set_from(right_bnd.start(), right_bnd.tangent_at(0), right_bnd.k1(), right_bnd.len1());
  
  dbgl_circ_arc right_arc2;
  right_arc2.set_from(right_arc1.end(), right_arc1.tangent_at_end(), right_bnd.k2(), right_bnd.len2());

  //re-estimate the biarc if each arc does not satisfy the max-curvature constraint
  if (right_arc1.k() > 1/start.radius())
  {
    // re-compute biarc, forcing right_arc1.k == 1 / start.radius
    double k1 = 1 / start.radius() * (1-1e-3); // avoid border values
    dbgl_circ_arc arc1, arc2;
    bool success = dbsksp_compute_biarc_given_k1(right_arc1.start(), right_arc1.tangent_at_start(),
      right_arc2.end(), right_arc2.tangent_at_end(),
      k1, arc1, arc2);
    if (!success)
    {
      vcl_cout << "\nERROR: can't enforce max-curvature constraint for right boundary.\n";
      return;
    }
    right_arc1 = arc1;
    right_arc2 = arc2;
  }
  else if (right_arc2.k() > 1 / end.radius())
  {
    // re-compute biarc, forcing right_arc2.k == 1 / end.radius
    double k2 = 1 / end.radius() * (1-1e-3); // avoid bordering values
    // reverse the biarc, for now
    dbgl_circ_arc temp_arc1, temp_arc2;
    bool success = dbsksp_compute_biarc_given_k1(right_arc2.end(), -right_arc2.tangent_at_end(), 
      right_arc1.start(), -right_arc1.tangent_at_start(),
      -k2, temp_arc2, temp_arc1);
    if (!success)
    {
      vcl_cout << "\nERROR: can't enforce max-curvature constraint for right boundary.\n";
      return;
    }
    else
    {
      // reverse the arcs
      right_arc1.set(temp_arc1.end(), temp_arc1.start(), -temp_arc1.k());
      right_arc2.set(temp_arc2.end(), temp_arc2.start(), -temp_arc2.k());
    }
  }

  if ( (-left_arc1.k()) > 1/start.radius() )
  {
    // recompute left biarc, forcing curvature to be -1/start.radius
    double k1 = -1/start.radius()* (1-1e-3);
    dbgl_circ_arc arc1, arc2;
    bool success = dbsksp_compute_biarc_given_k1(left_arc1.start(), left_arc1.tangent_at_start(),
      left_arc2.end(), left_arc2.tangent_at_end(),
      k1, arc1, arc2); 
    if (!success)
    {
      vcl_cout << "\nERROR: can't enforce max-curvature constraint for left boundary.\n";
      return;
    }
    left_arc1 = arc1;
    left_arc2 = arc2;

  }
  else if ( (-left_arc2.k()) > 1/end.radius() )
  {
    // re-compute biarc, forcing right_arc2.k == -1 / end.radius
    double k2 = -1 / end.radius()* (1-1e-3);
    // reverse the biarc, for now
    dbgl_circ_arc arc1, arc2;
    bool success = dbsksp_compute_biarc_given_k1(left_arc2.end(), -left_arc2.tangent_at_end(), 
      left_arc1.start(), -left_arc1.tangent_at_start(),
      -k2, arc2, arc1);
    if (!success)
    {
      vgl_vector_2d<double > v_left = normalized(end.bnd_pt_left() - start.bnd_pt_left());
      vgl_vector_2d<double > v_right = normalized(end.bnd_pt_right() - start.bnd_pt_right());
      vgl_vector_2d<double > v = normalized(end.pt() - start.pt());
      vcl_cout << "\nERROR: can't enforce max-curvature constraint for left boundary.\n";
      return;
    }
    else
    {
      // reverse the arcs
      left_arc1.set(arc1.end(), arc1.start(), -arc1.k());
      left_arc2.set(arc2.end(), arc2.start(), -arc2.k());
    }
  }

  // Final check. If max-curvature still cannot be satisfy then quit.
  if (right_arc1.k() > 1/start.radius() ||
    right_arc2.k() > 1 / end.radius() ||
    (-left_arc1.k()) > 1/start.radius() ||
    (-left_arc2.k()) > 1/end.radius() )
  {
    vcl_cout << "\nIn dbsksp_interp_xfrag_with_max_three_shapelets: "
      << "max-curvature constraint is not satisfied.\n";
    return;
  }


  vcl_vector<double > s_along_right_arc1;
  dbgl_compute_symmetry_point_on_circ_arc(right_arc1, 
    left_arc1.end(), -left_arc1.tangent_at_end(), // mid-point of left boundary
    s_along_right_arc1);

  // something is wrong if the number of solution is not 1
  if (s_along_right_arc1.size() != 1)
  {
    vcl_cerr << "\nERROR: In dbsksp_interp_xfrag_with_max_three_shapelets: "
      << "Number of symmetry point is not 1.\n";
    return;
  }

  vcl_vector<dbsksp_xshock_node_descriptor > list_xdesc;
  list_xdesc.push_back(start);

  double right_s1 = s_along_right_arc1[0];
  //if (right_s1 < 0) // something is wrong
  //{
  //  vcl_cerr << "\nERROR: In dbsksp_fit_xgraph::fit_shapelets_to_extrinsic_fragment - negative length!.\n";
  //  return;
  //}
  if (right_s1 >= 0 && right_s1 <= right_arc1.length()) // need to cut right_arc1 into two pieces
  {
    // descriptor 1: end point of left_arc1 and a middle point of right_arc1
    {
      vgl_point_2d<double > left_pt1 = left_arc1.end();
      vgl_point_2d<double > right_pt1 = right_arc1.point_at_length(right_s1);
      vgl_vector_2d<double > right_tangent1 = right_arc1.tangent_at_length(right_s1);
      double phi1 = signed_angle(right_tangent1, left_pt1-right_pt1);
      dbsksp_xshock_node_descriptor xdesc1;
      xdesc1.set(left_pt1, right_pt1, phi1);
      list_xdesc.push_back(xdesc1);



      //// \debug ////////////////////////////////////////////////////////////
      //{
      //  vgl_vector_2d<double > left_tangent1 = -left_arc1.tangent_at_end();
      //  double symmetry_measure = dot_product(left_pt1-right_pt1, left_tangent1-right_tangent1);

      //  double dist_left = (xdesc1.bnd_pt_left() - left_pt1).length();
      //  double dist_right = (xdesc1.bnd_pt_right() - right_pt1).length();
      //  double angle_left = signed_angle(-xdesc1.bnd_tangent_left(), left_tangent1);
      //  double angle_right = signed_angle(xdesc1.bnd_tangent_right(), right_tangent1);

      //  vcl_cout << "\nSymmetry measure = " << symmetry_measure << "\n"
      //    << "dist_left = " << dist_left << "\n"
      //    << "dist_right = " << dist_right << "\n"
      //    << "angle_left = " << angle_left << "\n"
      //    << "angl_right = " << angle_right << "\n";

      //}
      ////////////////////////////////////////
    }


    // descriptor 2: end point of right_arc1 and a middle point of left_arc2
    
    // only compute if the 1st descriptor does not fall exactly on the end point of right_arc1
    if ((right_arc1.length() - right_s1) > dbgl_circ_arc::epsilon)
    {
      vgl_point_2d<double > right_pt2 = right_arc1.end();
      vgl_vector_2d<double > right_tangent2 = right_arc1.tangent_at_end();

      // (Imagine) a reverse shock direction, the left becomes right boundary and vice versa
      dbgl_circ_arc left_arc2_inverted(left_arc2.end(), left_arc2.start(), -left_arc2.k());
      vcl_vector<double > temp;
      dbgl_compute_symmetry_point_on_circ_arc(left_arc2_inverted,
        right_pt2, right_tangent2, // junction point of right boundary
        temp);
      if (temp.empty())
      {
        vcl_cerr << "\nERROR: In dbsksp_interp_xfrag_with_max_three_shapelets: "
          << "Couldn't find symmetry point on left_arc2.\n";
        return;
      }
      assert(temp.size() == 1);
      vgl_point_2d<double > left_pt2 = left_arc2_inverted.point_at_length(temp[0]);
      double phi2 = signed_angle(right_tangent2, left_pt2 - right_pt2);
      dbsksp_xshock_node_descriptor xdesc2;
      xdesc2.set(left_pt2, right_pt2, phi2);
      list_xdesc.push_back(xdesc2);

      //// \debug ////////////////////////////////////////////////////////////
      //{
      //  double s_on_left_arc2 = left_arc2.length() - temp[0];
      //  vgl_vector_2d<double > left_tangent2 = -left_arc2.tangent_at_length(s_on_left_arc2);
      //  vgl_point_2d<double > left_pt2 = left_arc2.point_at_length(s_on_left_arc2);
      //  double symmetry_measure = dot_product(left_pt2-right_pt2, left_tangent2-right_tangent2);

      //  double dist_left = (xdesc2.bnd_pt_left() - left_pt2).length();
      //  double dist_right = (xdesc2.bnd_pt_right() - right_pt2).length();
      //  double angle_left = signed_angle(-xdesc2.bnd_tangent_left(), left_tangent2);
      //  double angle_right = signed_angle(xdesc2.bnd_tangent_right(), right_tangent2);

      //  vcl_cout << "\nSymmetry measure = " << symmetry_measure << "\n"
      //    << "dist_left = " << dist_left << "\n"
      //    << "dist_right = " << dist_right << "\n"
      //    << "angle_left = " << angle_left << "\n"
      //    << "angl_right = " << angle_right << "\n";

      //}
      ////////////////////////////////////////
    }
  }
  else // need to cut left_arc1 into two pieces
  {
    // descriptor 1: end point of right_arc1 and a middle point of left_arc1
    double s_along_left_arc1 = vnl_numeric_traits<double >::maxval;
    {
    vgl_point_2d<double > right_pt1 = right_arc1.end();
    vgl_vector_2d<double > right_tangent1 = right_arc1.tangent_at_end();

    // (Imagine) rotate the fragment 180 degree
    dbgl_circ_arc left_arc1_inverted(left_arc1.end(), left_arc1.start(), -left_arc1.k());
    vcl_vector<double > temp;
    dbgl_compute_symmetry_point_on_circ_arc(left_arc1_inverted, right_pt1, right_tangent1,
      temp);

    assert(temp.size() == 1);
    assert(temp[0] < left_arc1.length());

    
    vgl_point_2d<double > left_pt1 = left_arc1_inverted.point_at_length(temp[0]);
    double phi1 = signed_angle(right_tangent1, left_pt1 - right_pt1);
    dbsksp_xshock_node_descriptor xdesc1;
    xdesc1.set(left_pt1, right_pt1, phi1);
    list_xdesc.push_back(xdesc1);


    s_along_left_arc1 = left_arc1.length() - temp[0];




    //// \debug ////////////////////////////////////////////////////////////
    //{
    //  double len = left_arc1.length();
    //  double s = len - temp[0];
    //  vgl_vector_2d<double > left_tangent1 = -left_arc1.tangent_at_length(s);
    //  vgl_point_2d<double > left_pt1 = left_arc1.point_at_length(s);


    //  double symmetry_measure = dot_product(left_pt1-right_pt1, left_tangent1-right_tangent1);

    //  double dist_left = (xdesc1.bnd_pt_left() - left_pt1).length();
    //  double dist_right = (xdesc1.bnd_pt_right() - right_pt1).length();
    //  double angle_left = signed_angle(-xdesc1.bnd_tangent_left(), left_tangent1);
    //  double angle_right = signed_angle(xdesc1.bnd_tangent_right(), right_tangent1);

    //  vcl_cout << "\nSymmetry measure = " << symmetry_measure << "\n"
    //    << "dist_left = " << dist_left << "\n"
    //    << "dist_right = " << dist_right << "\n"
    //    << "angle_left = " << angle_left << "\n"
    //    << "angl_right = " << angle_right << "\n";

    //}
    ////////////////////////////////////////



    }

    if (s_along_left_arc1 < 0) // something is wrong
    {
      vcl_cerr << "\nERROR: In dbsksp_fit_xgraph::fit_shapelets_to_extrinsic_fragment - negative length!.\n";
      return;
    }



    // descriptor 2: end point of left_arc1 and a middle point of right_arc2
    if (s_along_left_arc1 > dbgl_circ_arc::epsilon)
    {
      vgl_point_2d<double > left_pt2 = left_arc1.end();
      vgl_vector_2d<double > left_tangent2 = -left_arc1.tangent_at_end();
      vcl_vector<double > temp;
      dbgl_compute_symmetry_point_on_circ_arc(right_arc2, left_pt2, left_tangent2, temp);

      assert(temp.size() == 1);
      vgl_point_2d<double > right_pt2 = right_arc2.point_at_length(temp[0]);
      vgl_vector_2d<double > right_tangent2 = right_arc2.tangent_at_length(temp[0]);
      double phi2 = signed_angle(right_tangent2, left_pt2 - right_pt2);

      dbsksp_xshock_node_descriptor xdesc2;
      xdesc2.set(left_pt2, right_pt2, phi2);
      list_xdesc.push_back(xdesc2);


      //// \debug ////////////////////////////////////////////////////////////
      //{
      //  double symmetry_measure = dot_product(left_pt2-right_pt2, left_tangent2-right_tangent2);

      //  double dist_left = (xdesc2.bnd_pt_left() - left_pt2).length();
      //  double dist_right = (xdesc2.bnd_pt_right() - right_pt2).length();
      //  double angle_left = signed_angle(-xdesc2.bnd_tangent_left(), left_tangent2);
      //  double angle_right = signed_angle(xdesc2.bnd_tangent_right(), right_tangent2);

      //  vcl_cout << "\nSymmetry measure = " << symmetry_measure << "\n"
      //    << "dist_left = " << dist_left << "\n"
      //    << "dist_right = " << dist_right << "\n"
      //    << "angle_left = " << angle_left << "\n"
      //    << "angl_right = " << angle_right << "\n";

      //}
      ////////////////////////////////////////
    }  
  }
  list_xdesc.push_back(end);

  // Now construct a shapelet between every pair of consecutive xdesc
  for (unsigned i =1; i < list_xdesc.size(); ++i)
  {
    dbsksp_xshock_node_descriptor xd0 = list_xdesc[i-1];
    dbsksp_xshock_node_descriptor xd1 = list_xdesc[i];

    vgl_vector_2d<double > chord = xd1.pt() - xd0.pt();
    double alpha0 = signed_angle(chord, xd0.shock_tangent());
    double m0 = vcl_sin(alpha0) / vcl_sin(xd0.phi());

    dbsksp_shapelet_sptr sh = new dbsksp_shapelet();
    sh->set_from(xd0.pt(), xd0.radius(), xd0.shock_tangent(), xd0.phi(), 
      m0, chord.length(), xd1.phi());

    list_shapelet.push_back(sh);
  }
  return;
}



//------------------------------------------------------------------------------
//: Compute a biarc from a two point-tangents. Assume the curvature of the first arc is known.
bool dbsksp_compute_biarc_given_k1(const vgl_point_2d<double >& pt1, 
                                   const vgl_vector_2d<double >& t1,
                                   const vgl_point_2d<double >& pt2,
                                   const vgl_vector_2d<double >& t2,
                                   double k1, // curvature of the first arc
                                   dbgl_circ_arc& arc1,  // returned pair of circular arc
                                   dbgl_circ_arc& arc2)
{
  double eps = 1e-8;

  // Algorithm: let A be the middle point connecting the two circular arcs of the biarc
  // Let tA be tangent at A.
  // Then (A, tA) and (pt2, t2) is a pair of symmetry points
  // Therefore, we can find A by finding the symmetry point of pt2 on the
  // circular arc that agrees with pt1, t1, and k1.
  
  // Temporary arc length for the first arc
  double temp_s1 = vgl_distance(pt1, pt2) * (1-1e-5)*vnl_math::pi;

  // if this is more than half a circle, reduce it to half a circle
  if (vnl_math_abs(k1 * temp_s1) > (1-1e-5)*vnl_math::pi) // this also confirms k1 is non-zero
  {
    temp_s1 = vnl_math_abs((1-1e-5)*vnl_math::pi / k1);
  }
  
  // construct a temporary arc
  dbgl_circ_arc temp_arc;
  temp_arc.set_from(pt1, t1, k1, temp_s1);

  // the biarc-midpoint is the symmetry point of pt2 on temp_arc
  vcl_vector<double > s_along_temp_arc;
  dbgl_compute_symmetry_point_on_circ_arc(temp_arc, pt2, t2, s_along_temp_arc);

  if (s_along_temp_arc.size() == 1)
  {
    double s1 = s_along_temp_arc.front();
    if (s1 < 0 || s1 > temp_s1)
      return false;
    arc1.set_from(pt1, t1, k1, s1);

    dbgl_circ_arc temp_arc2;
    temp_arc2.set_from(pt2, -t2, arc1.end());

    arc2.set(temp_arc2.end(), temp_arc2.start(), -temp_arc2.k());
    //arc2.set_from(arc1.end(), arc1.tangent_at_end(), pt2);

    return vnl_math_abs(signed_angle(arc2.tangent_at_end(), t2)) < eps;
  }
  else if (s_along_temp_arc.empty())
  {
    // check to see if this is indeed a straight line
    if (vnl_math_abs(k1) > eps)
      return false;

    vgl_vector_2d<double > v = normalized(pt2 - pt1);
    if (vnl_math_abs(signed_angle(v, t1)) < eps &&
      vnl_math_abs(signed_angle(v, t2)) < eps) // condition for a straight line
    {
      // the biarc is two line segments
      double len1 = v.length() / 2;
      double len2 = len1;
      arc1.set_from(pt1, t1, 0, len1);
      arc2.set_from(arc1.end(), arc1.tangent_at_end(), 0, len2);
      return true;
    }
    else
    {
      return false;
    }
  
  }

  return false;
}




//------------------------------------------------------------------------------
//: Fit a shapelet between two descriptors with the same shock point
// This is a degenerate case, often find is A1-A_\infty node
dbsksp_shapelet_sptr dbsksp_interp_xfrag_with_zero_chord_using_one_shapelet(const dbsksp_xshock_node_descriptor& start,
                                        const dbsksp_xshock_node_descriptor& end)
{
  // The key problem is to find the chord orientation
  double x0 = start.x();
  double y0 = start.y();
  double r0 = start.radius();
  double phi0 = start.phi();
  double phi1 = end.phi();
  double len = 1e-5;

  // compute theta
  

  dbsksp_compute_shapelet_chord_cost_function f1(start.shock_tangent_angle(), phi0,
    end.shock_tangent_angle(), phi1);

  vnl_brent_minimizer brent(f1);
  // estimate theta to be between the two shock tangents
  double gap = signed_angle(start.shock_tangent(), end.shock_tangent());
  double init_theta = start.shock_tangent_angle() + gap/2;

  double theta0 = brent.minimize(init_theta);

  double mdiff = brent.f_at_last_minimum();

  double m0 = vcl_sin(start.shock_tangent_angle() - theta0) / vcl_sin(phi0);
  // we ignore radius of the end descriptor (assume == start.radius())
  dbsksp_shapelet_sptr sh = new dbsksp_shapelet(x0, y0, theta0, r0, phi0, phi1, m0, len);
  return sh;
}



//------------------------------------------------------------------------------
//: Compute extrinsic samples of a shock fragment
// The samples are equally spaced along the left boundary biarc
void dbsksp_compute_middle_xsamples_by_sampling_longer_bnd_biarc(int num_intervals,
                                                               const dbsksp_xshock_node_descriptor& start_xdesc,
                                                               const dbsksp_xshock_node_descriptor& end_xdesc,
                                                               vcl_vector<dbsksp_xshock_node_descriptor >& list_xsample)
{
  list_xsample.clear();

  dbsksp_xshock_fragment xfrag(start_xdesc, end_xdesc);
  dbgl_biarc left_bnd = xfrag.bnd_left_as_biarc();
  dbgl_biarc right_bnd = xfrag.bnd_right_as_biarc();

  if (left_bnd.len() >= right_bnd.len())
  {
    dbsksp_compute_middle_xsamples_by_sampling_left_bnd_biarc(num_intervals,
      start_xdesc, end_xdesc, list_xsample);
  }
  else
  {
    // reverse the situation
    dbsksp_xshock_node_descriptor temp_start = end_xdesc.opposite_xnode();
    dbsksp_xshock_node_descriptor temp_end = start_xdesc.opposite_xnode();
    vcl_vector<dbsksp_xshock_node_descriptor > temp_list_xsample;
    dbsksp_compute_middle_xsamples_by_sampling_left_bnd_biarc(num_intervals,
      temp_start, temp_end, temp_list_xsample);

    // copy back to the list, in the reverse order
    list_xsample.reserve(temp_list_xsample.size());
    while(!temp_list_xsample.empty())
    {
      dbsksp_xshock_node_descriptor xdesc = temp_list_xsample.back();
      temp_list_xsample.pop_back();

      list_xsample.push_back(xdesc.opposite_xnode());
    }
  }

  return;
}




//------------------------------------------------------------------------------
//: Compute extrinsic samples of a shock fragment
// The samples are equally spaced along the left boundary biarc
void dbsksp_compute_middle_xsamples_by_sampling_left_bnd_biarc(int num_intervals,
                                                               const dbsksp_xshock_node_descriptor& start_xdesc,
                                                               const dbsksp_xshock_node_descriptor& end_xdesc,
                                                               vcl_vector<dbsksp_xshock_node_descriptor >& list_xsample)
{
  list_xsample.clear();

  if (num_intervals < 2) // nothing to compute
    return;

  dbsksp_xshock_fragment xfrag(start_xdesc, end_xdesc);
  dbgl_biarc left_bnd = xfrag.bnd_left_as_biarc();
  dbgl_biarc right_bnd = xfrag.bnd_right_as_biarc();

  // extract the two circular arcs of the biarc
  dbgl_circ_arc right_arc1;
  right_arc1.set_from(right_bnd.start(), right_bnd.tangent_at(0), right_bnd.k1(), right_bnd.len1());

  dbgl_circ_arc right_arc2(right_arc1.end(), right_bnd.end(), right_bnd.k2());

  // special care when righ_bnd is degenerate
  if (vgl_distance(right_bnd.start(), right_bnd.end()) < 1e-8)
  {
    vgl_vector_2d<double > t1 = start_xdesc.bnd_tangent_right();
    double ds = 1e-6;
    vgl_point_2d<double > pt = centre(right_bnd.start(), right_bnd.end());
    right_arc1.set(pt-ds*t1, pt, 0);
    right_arc2.set(pt, pt+ds*t1, 0);
  }

  // cache the arc length values
  double right_len1 = right_arc1.length();
  double right_len2 = right_arc2.length();


  double total_length = left_bnd.len();
  double sample_ds = total_length / num_intervals;
  for (int i =1; i < num_intervals; ++i)
  {
    double s = i * sample_ds;
    vgl_point_2d<double > left_pt = left_bnd.point_at(s);
    vgl_vector_2d<double > left_tangent = left_bnd.tangent_at(s);

    // compute symmetry point on the right boundary for this left point-tangent pair

    // first, check right arc1
    vcl_vector<double > s_along_right_arc1;
    dbgl_compute_symmetry_point_on_circ_arc(right_arc1, left_pt, -left_tangent, s_along_right_arc1);
    
    if (s_along_right_arc1.size() != 1)
      continue;

    double s1 = s_along_right_arc1.front();
    if (s1 >= 0 && s1 <= right_len1) // the symmetry point is on the first arc
    {
      vgl_point_2d<double > right_pt = right_arc1.point_at_length(s1);
      vgl_vector_2d<double > right_tangent = right_arc1.tangent_at_length(s1);
      double phi = signed_angle(right_tangent, left_pt - right_pt);

      dbsksp_xshock_node_descriptor xdesc;
      xdesc.set(left_pt, right_pt, phi);
      list_xsample.push_back(xdesc);
    }
    else // now, check right arc2
    {
      // compute symmetry point on the right arc2 for this left point-tangent pair
      vcl_vector<double > s_along_right_arc2;
      dbgl_compute_symmetry_point_on_circ_arc(right_arc2, left_pt, -left_tangent, s_along_right_arc2);
    
      if (s_along_right_arc2.size() != 1)
        continue;

      double s2 = s_along_right_arc2.front();
      if (s2 >= 0 && s2 <= right_len2)
      {
        vgl_point_2d<double > right_pt = right_arc2.point_at_length(s2);
        vgl_vector_2d<double > right_tangent = right_arc2.tangent_at_length(s2);
        double phi = signed_angle(right_tangent, left_pt - right_pt);

        dbsksp_xshock_node_descriptor xdesc;
        xdesc.set(left_pt, right_pt, phi);
        list_xsample.push_back(xdesc);
      } // if s2
    }// else
  } // for
  return;
}




//------------------------------------------------------------------------------
//: Compute sample of an xshock fragment given a ratio parameter [0, 1]
// The sample is picked to minimize sum of curvature difference on the boundary
bool dbsksp_compute_xfrag_sample_with_min_kdiff(const dbsksp_xshock_fragment& xfrag, double t,
                                                dbsksp_xshock_node_descriptor& xsample)
{
  assert(t >=0 && t <= 1);

  //dbsksp_xfrag_sample_kdiff_cost_function kdiff_cost(xfrag, t);
  dbsksp_xfrag_sample_along_shock_biarc_cost_function kdiff_cost(xfrag, t);
  vnl_vector<double > x = kdiff_cost.initial_x();

  // Use LM optimizer
  vnl_levenberg_marquardt lm(kdiff_cost);
  //lm.set_verbose(true);
  lm.minimize(x);
  lm.diagnose_outcome(vcl_cout);

  //lm.get_end_error();

  xsample = kdiff_cost.x_to_xsample(x);

  
  return true;
}






//------------------------------------------------------------------------------
//: Divide an xshock fragment into 2^n fragments
// require n > 0
// the return sample list includes start and end-samples
bool dbsksp_divide_xfrag_into_2_power_n_fragments(const dbsksp_xshock_fragment& xfrag, int n,
                                                vcl_vector<dbsksp_xshock_node_descriptor >& list_xsample)
{
  list_xsample.clear();
  if (n < 0)
    return false;

  // temporary storage for sample list
  vcl_vector<dbsksp_xshock_node_descriptor > list_0;
  vcl_vector<dbsksp_xshock_node_descriptor > list_1;
  int num_pts = dbnl_math_pow(2, n) + 1;
  list_0.reserve(num_pts);
  list_1.reserve(num_pts);

  vcl_vector<dbsksp_xshock_node_descriptor >* prev_list = &list_0;
  vcl_vector<dbsksp_xshock_node_descriptor >* cur_list = &list_1;
  vcl_vector<dbsksp_xshock_node_descriptor >* temp;

  // initialization
  cur_list->clear();
  cur_list->push_back(xfrag.start());
  cur_list->push_back(xfrag.end());

  for (int k =0; k < n; ++k)
  {
    // swap prev_list and cur_list
    temp = prev_list;
    prev_list = cur_list;
    cur_list = temp;

    // split each segment in prev_list into 2
    cur_list->clear();
    cur_list->push_back(prev_list->front());
    
    for (unsigned i =1; i < prev_list->size(); ++i)
    {
      dbsksp_xshock_fragment xfrag(prev_list->at(i-1), prev_list->at(i));
      dbsksp_xshock_node_descriptor xsample;
      dbsksp_compute_xfrag_sample_with_min_kdiff(xfrag, 0.5, xsample );
      
      cur_list->push_back(xsample);
      cur_list->push_back(prev_list->at(i));
    }    
  }

  list_xsample = *cur_list;

  return true;
}



















