// This is file shp/dbsksp/algo/dbsksp_fit_shock_path.cxx

//:
// \file

#include "dbsksp_fit_shock_path.h"


//
//#include <dbsksp/dbsksp_shock_graph.h>
//#include <dbsksp/dbsksp_xshock_graph.h>
//#include <dbsksp/dbsksp_shapelet_sptr.h>
//#include <dbsksp/dbsksp_shapelet.h>
#include <dbsksp/dbsksp_xshock_fragment.h>
#include <dbsksp/algo/dbsksp_shock_path.h>
//
//#include <dbsksp/algo/dbsksp_compute_scurve.h>
//#include <dbsksp/algo/dbsksp_xgraph_algos.h>
#include <dbsksp/algo/dbsksp_fit_one_shock_branch_cost_function.h>
#include <dbgl/algo/dbgl_biarc.h>
//
//#include <dbskr/dbskr_scurve.h>
//
//#include <dbsk2d/dbsk2d_shock_graph.h>
//#include <dbsk2d/dbsk2d_xshock_edge.h>
//#include <dbsk2d/dbsk2d_xshock_edge_sptr.h>
//#include <dbgl/algo/dbgl_compute_symmetry_point.h>
//#include <dbgl/algo/dbgl_biarc.h>
#include <dbgl/algo/dbgl_closest_point.h>
#include <dbnl/dbnl_angle.h>

#include <vnl/algo/vnl_levenberg_marquardt.h>
#include <vnl/vnl_numeric_traits.h>
#include <vnl/vnl_math.h>
#include <vgl/vgl_distance.h>



//------------------------------------------------------------------------------
//: Fit a shock branch to a list of xshock samples
bool dbsksp_fit_one_shock_branch_with_power_of_2_intervals(const dbsksp_xshock_node_descriptor& start_desc, 
                            const dbsksp_xshock_node_descriptor& end_desc, 
                            const vcl_vector<dbsksp_xshock_node_descriptor >& xsamples, 
                            double error_threshold,
                            vcl_vector<dbsksp_xshock_node_descriptor >& list_middle_xdesc)
{
  list_middle_xdesc.clear();

  if (xsamples.empty())
    return false;

  

  // For a polyline connecting the mid-points of the boundary point pairs
  vcl_vector<vgl_point_2d<double > > list_bnd_centroid;
  list_bnd_centroid.reserve(xsamples.size());
  for (unsigned i =0; i < xsamples.size(); ++i)
  {
    dbsksp_xshock_node_descriptor xdesc = xsamples[i];
    list_bnd_centroid.push_back(xdesc.bnd_mid_pt());
  }


  // boundary length of the initial branch
  double total_left_bnd_length = 0;
  double total_right_bnd_length = 0;
  for (unsigned i =1; i < xsamples.size(); ++i)
  {
    dbsksp_xshock_node_descriptor start = xsamples[i-1];
    dbsksp_xshock_node_descriptor end = xsamples[i];

    total_left_bnd_length += vgl_distance(start.bnd_pt_left(), end.bnd_pt_left());
    total_right_bnd_length += vgl_distance(start.bnd_pt_right(), end.bnd_pt_right());
  }

  // \todo make sure the polyline does not self-intersect

  // Do optimization -----------------------------------------------------------

  //
  // The following should be a parametric polyline
  // But since the current code for parametric polyline is too cumbersome
  // we will manually code it for the polyline case

  // keep track of the arclength at the sample centroids
  vcl_vector<double > list_length;
  list_length.reserve(list_bnd_centroid.size());
  
  double total_length = 0;
  list_length.push_back(0);
  for (unsigned i =1; i < list_bnd_centroid.size(); ++i)
  {
    total_length += (list_bnd_centroid[i] - list_bnd_centroid[i-1]).length();
    list_length.push_back(total_length);
  }

  // Iteratively increasing the number of intermediate points until we're able
  // push the RMS error down below the preset threshold
  // For each iteration, we double the number of intervals
  int max_number_intervals = list_length.size()-1;

  int num_intervals = 1;
  double rms_error = vnl_numeric_traits<double >::maxval;
  
  vcl_vector<dbsksp_xshock_node_descriptor > list_all_xdesc;

  while (num_intervals <= max_number_intervals)
  {
    int num_shock_points = num_intervals - 1;

    // Determine position of the intermediate shock points
    vcl_vector<dbsksp_xshock_node_descriptor > list_init_xdesc;
    list_init_xdesc.reserve(num_shock_points);

    // Index of sample points just after the end of each interval
    vcl_vector<unsigned > list_interval_end_idx;
    list_interval_end_idx.reserve(num_intervals+1);
    list_interval_end_idx.push_back(0);
    

    unsigned cur_pos = 0;
    for (int i =0; i < num_shock_points; ++i)
    {
      double len = (i+1) * (total_length / num_intervals);

      // search for interval
      for (; cur_pos <list_length.size() && list_length[cur_pos] < len; ++cur_pos);

      // save position to index list
      list_interval_end_idx.push_back(cur_pos);

      
      // The shock point is between two sample shock points at positions [cur_pos-1] and [cur_pos]
      // Each point between these two points corresponds to a value of t \in [0, 1].
      double t = (len - list_length[cur_pos-1]) / (list_length[cur_pos] - list_length[cur_pos-1]);


      // Get the initial values for the shock points by linearly interpolating
      // between its two adjacent sample shock points
      dbsksp_xshock_node_descriptor prev_xdesc = xsamples[cur_pos-1];
      dbsksp_xshock_node_descriptor cur_xdesc = xsamples[cur_pos];

      // Pick init shock point
      dbsksp_xshock_node_descriptor mid_xdesc;


      // Method 1: Interpolation - Need improvements
      {
        dbgl_biarc shock_curve(prev_xdesc.pt(), prev_xdesc.shock_tangent(), cur_xdesc.pt(), cur_xdesc.shock_tangent());
        vgl_point_2d<double > shock_pt = shock_curve.point_at(t * shock_curve.len());

        // project the shock point to the two boundary biarcs
        dbsksp_xshock_fragment xfrag(prev_xdesc, cur_xdesc);
        dbgl_biarc left_bnd = xfrag.bnd_left_as_biarc();
        dbgl_biarc right_bnd = xfrag.bnd_right_as_biarc();

        // if something is wrong, then just take the first xdesc
        if (left_bnd.is_consistent() && right_bnd.is_consistent())
        {

          double left_s = 0;
          dbgl_closest_point::point_to_biarc(shock_pt, left_bnd, left_s);
          vgl_point_2d<double > left_pt = left_bnd.point_at(left_s);

          double right_s = 0;
          dbgl_closest_point::point_to_biarc(shock_pt, right_bnd, right_s);
          vgl_point_2d<double > right_pt = right_bnd.point_at(right_s);

          // compute phi
          //vgl_vector_2d<double > shock_tangent = rotated(right_pt-left_pt, vnl_math::pi_over_2);
          double twophi = signed_angle(right_pt-shock_pt, left_pt-shock_pt);
          double phi = dbnl_angle_0to2pi(twophi) / 2;

          mid_xdesc.set(left_pt, right_pt, phi);
        }
        else
        {
          mid_xdesc = prev_xdesc;
        }
      }
      list_init_xdesc.push_back(mid_xdesc);
    }
    list_interval_end_idx.push_back(list_length.size());


    // Group xsamples corresponding to each interval
    vcl_vector<vcl_vector<const dbsksp_xshock_node_descriptor* > > list_xsamples_per_interval(num_intervals);

    for (int k =0; k < num_intervals; ++k)
    {
      for (unsigned i = list_interval_end_idx[k]; i < list_interval_end_idx[k+1]; ++i)
      {
        list_xsamples_per_interval[k].push_back(& (xsamples[i]));
      }
    }

    // Now construct a cost function and optimize the variables
    // For each xnode descriptor we fix the position of the boundary-mid-point 
    // and vary the remaining three parameters

    //// Cost function
    //dbsksp_fit_one_shock_branch_cost_function cost_fitting_one_branch(start_desc, end_desc,
    //  list_init_xdesc, list_xsamples_per_interval,
    //  total_left_bnd_length, total_right_bnd_length);

    // Cost function
    dbsksp_fit_shock_branch_with_fixed_shock_points_cost_function cost_fitting_one_branch(start_desc, end_desc,
      list_init_xdesc, list_xsamples_per_interval,
      total_left_bnd_length, total_right_bnd_length);

    vnl_vector<double > x;
    cost_fitting_one_branch.cur_x(x);

    if (!x.empty())
    {
      // Use LM optimizer
      vnl_levenberg_marquardt lm(cost_fitting_one_branch);
      //lm.set_verbose(true);
      lm.minimize(x);
      rms_error = lm.get_end_error();      
    }
    else
    {
      vnl_vector<double > fx(cost_fitting_one_branch.get_number_of_residuals());
      cost_fitting_one_branch.f(x, fx);
      rms_error = fx.rms();
    }

    // Save solutions !
    cost_fitting_one_branch.x_to_xsamples(x, list_all_xdesc);
    
    if (rms_error <= error_threshold)
    {
      break;
    }
    else
    {
      // increase the number of intervals in the shock branch
      num_intervals *= 2;
    }
  }

  // output results
  for (unsigned i =1; (i+1) < list_all_xdesc.size(); ++i)
  {
    list_middle_xdesc.push_back(list_all_xdesc[i]);
  }
  
  return true;
}





//------------------------------------------------------------------------------
//: Fit a shock branch using minimum number of points
// `rms_tol' is tolerance for RMS of distance error
bool dbsksp_fit_shock_path_using_min_num_pts(const dbsksp_shock_path_sptr& in_path,
                                             const dbsksp_xshock_node_descriptor& start_xdesc, 
                                             const dbsksp_xshock_node_descriptor& end_xdesc, 
                                             double rms_error_tol,
                                             dbsksp_shock_path_sptr& out_path,
                                             int min_num_intervals,
                                             int max_num_intervals)
{
  //0) Sanitize output storage
  out_path = 0;

  //1) Preliminary checks
  if (in_path->num_points() == 0)
    return false;

  //2) Parametrize the descriptors by length along the mid-line

  vcl_vector<vgl_point_2d<double > > mid_line;
  in_path->get_mid_line(mid_line);

  // boundary length of the initial branch
  double total_left_bnd_length = 0;
  double total_right_bnd_length = 0;
  for (unsigned i =1; i < in_path->num_points(); ++i)
  {
    dbsksp_xshock_node_descriptor start = *in_path->xdesc(i-1);
    dbsksp_xshock_node_descriptor end   = *in_path->xdesc(i);

    total_left_bnd_length += vgl_distance(start.bnd_pt_left(), end.bnd_pt_left());
    total_right_bnd_length += vgl_distance(start.bnd_pt_right(), end.bnd_pt_right());
  }

  // keep track of the arclength at the sample centroids
  vcl_vector<double > list_length;
  in_path->get_mid_line_lengths(list_length);
  double total_length = list_length.back();

  //3) Do optimization
  // Iteratively increase number of intermediate points until RMS error < tolerance

  // Initial values
  max_num_intervals = vnl_math_min(max_num_intervals, int(list_length.size())-1);
  int num_intervals = min_num_intervals;
  double rms_error = vnl_numeric_traits<double >::maxval;

  // container for output
  vcl_vector<dbsksp_xshock_node_descriptor > list_all_xdesc;
  double min_rms_error = vnl_numeric_traits<double >::maxval;

  // In the worst case, we can always use the orignal xsamples to fit this path!
  while (num_intervals <= max_num_intervals)
  {
    int num_shock_points = num_intervals - 1;
    
    // Index of sample points just after the end of each interval
    vcl_vector<unsigned > list_interval_end_idx;
    list_interval_end_idx.reserve(num_intervals+1);
    list_interval_end_idx.push_back(0);

    // Approximate the intermiate descriptors
    vcl_vector<dbsksp_xshock_node_descriptor > list_init_xdesc;
    list_init_xdesc.reserve(num_shock_points);
    
    unsigned cur_pos = 0;
    for (int i =0; i < num_shock_points; ++i)
    {
      double len = (i+1) * (total_length / num_intervals);

      // search for interval
      for (; cur_pos <list_length.size() && list_length[cur_pos] < len; ++cur_pos);

      // save position to index list
      list_interval_end_idx.push_back(cur_pos);
     
      // The shock point is between descriptors at position [cur_pos-1] and [cur_pos]
      // Interpolate between these two points
      double prev_len = list_length[cur_pos-1];
      double cur_len  = list_length[cur_pos];
      double t        = (len - prev_len ) / (cur_len - prev_len);

      dbsksp_xshock_node_descriptor prev_xdesc = *in_path->xdesc(cur_pos-1);
      dbsksp_xshock_node_descriptor cur_xdesc  = *in_path->xdesc(cur_pos);

      // Pick init shock point
      dbsksp_xshock_node_descriptor mid_xdesc = dbsksp_coarse_interp_btw_xsamples(prev_xdesc, cur_xdesc, t);
      list_init_xdesc.push_back(mid_xdesc);
    }
    list_interval_end_idx.push_back(list_length.size());

    // Group xsamples corresponding to each interval
    vcl_vector<vcl_vector<const dbsksp_xshock_node_descriptor* > > list_xsamples_per_interval(num_intervals);
    for (int k =0; k < num_intervals; ++k)
    {
      for (unsigned i = list_interval_end_idx[k]; i < list_interval_end_idx[k+1]; ++i)
      {
        list_xsamples_per_interval[k].push_back(in_path->xdesc(i));
      }
    }
   
    // Fine-tune intermediate shock descriptors, keeping the two end-descriptors fixed
    dbsksp_fit_shock_branch_with_fixed_shock_points_cost_function 
      cost_fitting_one_branch(start_xdesc, end_xdesc,
                              list_init_xdesc, list_xsamples_per_interval,
                              total_left_bnd_length, total_right_bnd_length);

    // Initial state of the approximation
    vnl_vector<double > x;
    cost_fitting_one_branch.cur_x(x);

    if (!x.empty())
    {
      vnl_levenberg_marquardt lm(cost_fitting_one_branch);
      lm.minimize(x);
      rms_error = lm.get_end_error();      
    }
    else
    {
      vnl_vector<double > fx(cost_fitting_one_branch.get_number_of_residuals());
      cost_fitting_one_branch.f(x, fx);
      rms_error = fx.rms();
    }

    // Save solutions !
    if (min_rms_error > rms_error)
    {
      min_rms_error = rms_error;
      cost_fitting_one_branch.x_to_xsamples(x, list_all_xdesc);
    }
    
    if (rms_error <= rms_error_tol)
    {
      break;
    }
    else
    {
      // increase the number of intervals in the shock branch
      num_intervals += 1;
    }
  }

  // output results
  out_path = new dbsksp_shock_path(list_all_xdesc);

  // If fitted succeeded, the loop should terminate before reaching the max_num_intervals
  if (num_intervals > max_num_intervals)
  {
    return false;
  }

  return true;
}








//------------------------------------------------------------------------------
//: Coarse interpolation of between two xsamples
dbsksp_xshock_node_descriptor dbsksp_coarse_interp_btw_xsamples(const dbsksp_xshock_node_descriptor& xdesc0,
                                                                const dbsksp_xshock_node_descriptor& xdesc1,
                                                                double t)
{
  // Container for output
  dbsksp_xshock_node_descriptor mid_xdesc;

  // Find the shock point by parametrizing the shock curve with biarc
  dbgl_biarc shock_curve(xdesc0.pt(), xdesc0.shock_tangent(), 
                         xdesc1.pt(), xdesc1.shock_tangent());
  vgl_point_2d<double > shock_pt = shock_curve.point_at(t * shock_curve.len());

  // Find boundary points by projecting shock point to two boundary curves (biarcs)
  dbsksp_xshock_fragment xfrag(xdesc0, xdesc1);
  dbgl_biarc left_bnd = xfrag.bnd_left_as_biarc();
  dbgl_biarc right_bnd = xfrag.bnd_right_as_biarc();

  // if something is wrong, then just take the first xdesc
  if (left_bnd.is_consistent() && right_bnd.is_consistent())
  {
    double left_s = 0;
    dbgl_closest_point::point_to_biarc(shock_pt, left_bnd, left_s);
    vgl_point_2d<double > left_pt = left_bnd.point_at(left_s);

    double right_s = 0;
    dbgl_closest_point::point_to_biarc(shock_pt, right_bnd, right_s);
    vgl_point_2d<double > right_pt = right_bnd.point_at(right_s);

    // compute phi
    //vgl_vector_2d<double > shock_tangent = rotated(right_pt-left_pt, vnl_math::pi_over_2);
    double twophi = signed_angle(right_pt-shock_pt, left_pt-shock_pt);
    double phi = dbnl_angle_0to2pi(twophi) / 2;

    mid_xdesc.set(left_pt, right_pt, phi);
  }
  else
  {
    mid_xdesc = xdesc0;
  }
  return mid_xdesc;
}
