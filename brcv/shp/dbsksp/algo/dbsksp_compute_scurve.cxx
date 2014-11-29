// This is brcv/shp/dbsk2d/algo/dbsksp_compute_scurve.cxx

//:
// \file

#include "dbsksp_compute_scurve.h"
#include <dbskr/dbskr_scurve.h>

#include <dbsksp/dbsksp_xshock_node.h>
#include <dbsksp/dbsksp_xshock_edge.h>
#include <dbsksp/dbsksp_xshock_fragment.h>
#include <dbsksp/algo/dbsksp_interp_two_xnodes.h>
#include <dbsksp/algo/dbsksp_xgraph_algos.h>
#include <dbsksp/dbsksp_shapelet.h>
#include <vgl/vgl_distance.h>
#include <vnl/vnl_math.h>
#include <vcl_algorithm.h>

#include <dbsksp/algo/dbsksp_shock_path.h>


//: \relates dbsksp_xshock_graph
//: Compute a shock curve from a shock graph path
dbsksp_shock_path_sptr dbsksp_compute_uniform_shock_path(const dbsksp_xshock_node_sptr& start_node,
                                       const vcl_vector<dbsksp_xshock_edge_sptr>& path,
                                       double sample_ds)
{
    // compute samples from the given path
  vcl_vector<dbsksp_xshock_node_descriptor > xsamples;
  dbsksp_xgraph_algos::compute_xsamples(start_node, path, sample_ds/4, xsamples);

  // Resample xsample list to uniform sampling
  vcl_vector<double > run_lengths;
  run_lengths.clear();
  run_lengths.reserve(xsamples.size());
  
  ////
  //vcl_vector<dbgl_biarc > chord_curves;
  //chord_curves.clear();
  //chord_curves.reserve(xsamples.size());

  {
    // Init
    
    dbsksp_xshock_node_descriptor cur_xsample = xsamples.front();
    
    // Increment the arc length as we move along the points
    // Remove sample points that cause zig-zaging on the mid-line

    // forward direction
    {
      vcl_vector<dbsksp_xshock_node_descriptor > ordered_xsamples;
      ordered_xsamples.reserve(xsamples.size());
      ordered_xsamples.push_back(cur_xsample);
      for (unsigned i =1; (i+1) < xsamples.size(); ++i)
      {
        dbsksp_xshock_node_descriptor prev_xsample = cur_xsample;
        cur_xsample = xsamples[i];

        // detecting zig-zaging on the shock point
        vgl_vector_2d<double > v = cur_xsample.pt() - prev_xsample.pt();
        bool zigzaging = inner_product(v, prev_xsample.shock_tangent()) < 0;

        // skip sample that cause zigzaging
        if (zigzaging)
        {
          cur_xsample = prev_xsample;
          continue;
        }

        ordered_xsamples.push_back(cur_xsample);
      }
      ordered_xsamples.push_back(xsamples.back());

      // re-set
      xsamples = ordered_xsamples;
    }

    // Reverse direction
    {
      vcl_vector<dbsksp_xshock_node_descriptor > rev_ordered_xsamples;
      rev_ordered_xsamples.reserve(xsamples.size());
      cur_xsample = xsamples.back();
      rev_ordered_xsamples.push_back(cur_xsample);
      for (int i = int(xsamples.size())-2; i > 0; --i)
      {
        dbsksp_xshock_node_descriptor prev_xsample = cur_xsample;
        cur_xsample = xsamples[i];

        // detecting zig-zaging on the shock point
        vgl_vector_2d<double > v = cur_xsample.pt() - prev_xsample.pt();
        // is the shock curve zig-zaging?
        bool zigzaging = inner_product(v, -prev_xsample.shock_tangent()) < 0;

        // skip sample that cause zigzaging
        if (zigzaging)
        {
          cur_xsample = prev_xsample;
          continue;
        }

        rev_ordered_xsamples.push_back(cur_xsample);
      }
      rev_ordered_xsamples.push_back(xsamples.front());
    
      // copy rev_ordered_samples back to xsamples,
      xsamples.clear();
      xsamples.insert(xsamples.begin(), rev_ordered_xsamples.rbegin(), rev_ordered_xsamples.rend());
    }
    
    
    double cur_length = 0;
    run_lengths.push_back(cur_length);
    for (unsigned i =1; i < xsamples.size(); ++i)
    {
      dbsksp_xshock_node_descriptor prev_xsample = xsamples[i-1];
      cur_xsample = xsamples[i];

      //// Approximate the length of the chordal curve (connecting mid-point of symmetric point-pairs)
      //// with the length of the biarc interpolating between two end chordal points
      //dbgl_biarc chord_biarc(prev_xsample.bnd_mid_pt(), prev_xsample.shock_tangent(), 
      //                       cur_xsample.bnd_mid_pt(),  cur_xsample.shock_tangent());

      //chord_curves.push_back(chord_biarc); // save for later use

      //double ds = chord_biarc.len();

      double ds = 0.5 * ( vgl_distance(prev_xsample.bnd_pt_left(), cur_xsample.bnd_pt_left()) + 
                        vgl_distance(prev_xsample.bnd_pt_right(), cur_xsample.bnd_pt_right()) );
      cur_length += ds;

      run_lengths.push_back(cur_length);
    }
    
  }

  // debuging
  //return new dbsksp_shock_path(xsamples);


  // Down samples so that points are uniform
  int num_points = vnl_math::rnd(run_lengths.back() / sample_ds) + 1;

  // we want at least 2 points
  num_points = vnl_math::max(num_points, 2);

  // Downsample to get uniformly distributed samples
  vcl_vector<dbsksp_xshock_node_descriptor > uniform_xsamples;
  {
    double ds_per_interval = run_lengths.back() / (num_points-1);


    int cur_idx = 0;

    vcl_vector<unsigned > downsample_index;
    downsample_index.reserve(num_points);
    downsample_index.push_back(0);

    // For every target length, choose the closest sample
    for (int i =1; (i+1) < num_points; ++i)
    {
      double s = i * ds_per_interval;

      //a) Find index of the point `just past' the target point
      for (; run_lengths[cur_idx] < s; ++cur_idx);

      //b) Choose between current sample and the previous one - pick the closer one
      int prev_idx  = cur_idx -1;
      double cur_s  = run_lengths[cur_idx];
      double prev_s = run_lengths[prev_idx];

      // choose the closer sample
      double ratio  = (s - prev_s) / (cur_s - prev_s);
      unsigned xsample_idx = (ratio < 0.5) ? prev_idx : cur_idx;

      // only add if this sample has not been added
      if (xsample_idx != downsample_index.back())
      {
        downsample_index.push_back(xsample_idx);
      }  
    }
    downsample_index.push_back(xsamples.size()-1);

    

    for (unsigned i =0; i < downsample_index.size(); ++i)
    {
      uniform_xsamples.push_back(xsamples[downsample_index[i]]);
    }
  }


  // The followin strategory of interpolating using shock chord doesn't always work
  // Since it creates self-intersecting samples


  //// down-sample by 2
  ////sample_ds *= 2;
  //int num_points = vnl_math::rnd(chord_run_lengths.back() / sample_ds) + 1;

  //// we want at least 2 points
  //num_points = vnl_math::max(num_points, 2);

  //// Interpolate to get uniformly distributed samples
  //vcl_vector<dbsksp_xshock_node_descriptor > uniform_xsamples;
  //{
  //  double total_chordal_length = chord_run_lengths.back();
  //  double ds_per_interval = total_chordal_length / (num_points-1);

  //  int cur_idx = 0;
  //  uniform_xsamples.push_back(xsamples.front());

  //  // interpolate every point except for the last point
  //  for (int i =1; (i+1) < num_points; ++i)
  //  {
  //    double s = i * ds_per_interval;

  //    //a) Find index of the point `just past' the target point
  //    for (; chord_run_lengths[cur_idx] < s; ++cur_idx);

  //    //b) Linear interpolation
  //    int prev_idx  = cur_idx -1;
  //    double cur_s  = chord_run_lengths[cur_idx];
  //    double prev_s = chord_run_lengths[prev_idx];
  //    double ratio  = (s - prev_s) / (cur_s - prev_s);

  //    // chordal point
  //    dbgl_biarc& chord_curve        = chord_curves[prev_idx];
  //    vgl_point_2d<double > chord_pt = chord_curve.point_at(s-prev_s);

  //    // chord/shock tangent
  //    vgl_vector_2d<double > chord_orient = chord_curve.tangent_at(s-prev_s);

  //    // linear interpolate phi
  //    double phi = (1-ratio) * xsamples[prev_idx].phi() + 
  //                    ratio  * xsamples[cur_idx].phi();

  //    // chordal width requires quadratic interpolation
  //    // x \in [0, l], l = cur_s - prev_s
  //    // y        =  ax^2 + bx + c
  //    // dy/dx    = 2ax   + b      = -1/tan(phi)
  //    
  //    // y(0)     = c              = chordal width at prev_s
  //    // dy/dx(0) = b              = -1/tan( phi(prev_s) )
  //    // y(l)     = a*l^2 + b*l + c = chordal width at cur_s
  //    // a = (y(l) - c - b*l) / l^2

  //    double y0 = xsamples[prev_idx].chordal_radius();
  //    double y1 = xsamples[cur_idx].chordal_radius();
  //    double len = cur_s - prev_s;
  //    double c = y0;
  //    double prev_phi = xsamples[prev_idx].phi();
  //    double b = -vcl_cos(prev_phi) / vcl_sin(prev_phi);
  //    double a = (y1 - b*len - c) / (len*len);

  //    
  //    double chord_radius = a* (s-prev_s)*(s-prev_s) + b * (s-prev_s) + c;

  //    // Now we are ready to construct the shock point
  //    double radius = chord_radius / vcl_sin(phi);
  //    vgl_point_2d<double > shock_pt = chord_pt - radius * vcl_cos(phi) * chord_orient;

  //    dbsksp_xshock_node_descriptor xsample(shock_pt, chord_orient, phi, radius);
  //    uniform_xsamples.push_back(xsample);
  //  }
  //  uniform_xsamples.push_back(xsamples.back());
  //}

  return new dbsksp_shock_path(uniform_xsamples);
}





//: Compute a shock curve from a shock graph path
// \relates dbsksp_xshock_graph
dbskr_scurve_sptr dbsksp_compute_scurve(const dbsksp_xshock_node_sptr& start_node,
                                       const vcl_vector<dbsksp_xshock_edge_sptr>& path,
                                       double sample_ds)
{

  dbsksp_shock_path_sptr shock_path = dbsksp_compute_uniform_shock_path(start_node, path, sample_ds);

  // Feed the samples to the scurve constructor
  vcl_vector< vgl_point_2d<double> > sh_pt;
  vcl_vector<double> time, theta, phi;

  unsigned num_pts = shock_path->num_points();
  sh_pt.reserve(num_pts);
  time.reserve(num_pts);
  theta.reserve(num_pts);
  phi.reserve(num_pts);

  for (unsigned k =0; k < num_pts; ++k)
  {
    dbsksp_xshock_node_descriptor xdesc = *shock_path->xdesc(k);
    sh_pt.push_back(xdesc.pt());
    time.push_back(xdesc.radius());
    phi.push_back(xdesc.phi());
    theta.push_back(xdesc.shock_tangent_angle());    
  }



  bool binterpolate = false;
  bool bsub_sample = false;
  double interpolate_ds = sample_ds; //1;
  double subsample_ds = sample_ds; //1;


  //construct the shock curve from the compiled information without interpolation
  dbskr_scurve_sptr shock_curve = new dbskr_scurve(sh_pt.size(),
                                                   sh_pt, time, theta, phi,
                                                   binterpolate, interpolate_ds,
                                                   bsub_sample, subsample_ds);

  return shock_curve;
}




