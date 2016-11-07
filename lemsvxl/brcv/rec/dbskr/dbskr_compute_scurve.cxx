// This is brcv/shp/dbsk2d/algo/dbsk2d_compute_scurve.cxx

//:
// \file

#include "dbskr_compute_scurve.h"
#include "dbskr_scurve.h"

#include <vcl_algorithm.h>
#include <dbsk2d/dbsk2d_xshock_edge.h>
#include <dbsk2d/dbsk2d_xshock_sample.h>

#include <vgl/vgl_distance.h>

//: \relates dbsk2d_shock_graph
dbskr_scurve_sptr dbskr_compute_scurve(dbsk2d_shock_node_sptr start_node,
                                       vcl_vector<dbsk2d_shock_edge_sptr> path,
                                       bool leaf_edge, 
                                       bool binterpolate, bool bsub_sample,
                                       double interpolate_ds, double subsample_ds,
                                       double scale_ratio)
{
  vcl_vector< vgl_point_2d<double> > sh_pt;
  vcl_vector<double> time, theta, phi;

  dbsk2d_shock_node_sptr cur_start_node = start_node;

  //traverse through the path, interpolating where necessary
  for (vcl_vector<dbsk2d_shock_edge_sptr>::iterator e_it = path.begin();
       e_it != path.end(); e_it++)
  {
    dbsk2d_xshock_edge* cur_edge = dynamic_cast<dbsk2d_xshock_edge*>(e_it->ptr());

    if (cur_start_node == cur_edge->source())
    {
      //this edge is to be reconstructed the right way

      //go through the edge samples and append it to the shock curve
      for (int i=0; i< cur_edge->num_samples(); i++)
      {
        dbsk2d_xshock_sample_sptr sample = cur_edge->sample(i);

        sh_pt.push_back(sample->pt);
        time.push_back(sample->radius); 
        if (sample->speed != 0 && sample->speed < 99990) //100000 signals infinity
          phi.push_back(vcl_acos(-1/sample->speed));
        else
          phi.push_back(vnl_math::pi/2);

        theta.push_back(sample->theta);//direction of flow is same as actual
      }
    }
    else 
    {
      //this edge is to be flipped

      //go through the edge samples in the reverse order and append it to the shock curve
      for (int i=cur_edge->num_samples()-1; i>=0 ; i--)
      {
        dbsk2d_xshock_sample_sptr sample = cur_edge->sample(i);

        sh_pt.push_back(sample->pt);
        time.push_back(sample->radius); 
        if (sample->speed != 0 && sample->speed < 99990) //100000 signals infinity
          phi.push_back(vnl_math::pi - vcl_acos(-1/sample->speed));
        else
          phi.push_back(vnl_math::pi/2);
        
        theta.push_back(angle0To2Pi(sample->theta+vnl_math::pi));//direction of flow is reversed
      }
    }

    //update the cur_start_node to the opposite node
    cur_start_node = cur_edge->opposite(cur_start_node);
  }

  //close off the end with a circular arc if it is an A3
  unsigned num_points = sh_pt.size();

  if (leaf_edge)
  {
    double dphi = 0-phi[num_points-1]; //phi at the end is 0
    double apprxds = vcl_abs(dphi)*time[num_points-1];
    
    //num of extra samples
    int num = int(apprxds/interpolate_ds); 
    
    //add the interpolated samples
    for(int j=1; j<=num; j++)
    {
      float ratio = (float)j/(float)num;

      sh_pt.push_back(sh_pt[num_points-1]); //at an A3, point remains stationary
      time.push_back(time[num_points-1]);
      theta.push_back(theta[num_points-1]);
      phi.push_back(phi[num_points-1]+ratio*dphi);  //phi needs to be interpolated
    }
  }

  //construct the shock curve from the compiled information without interpolation
  dbskr_scurve_sptr shock_curve = new dbskr_scurve(sh_pt.size(),
                                                   sh_pt, time, theta, phi,
                                                   binterpolate, interpolate_ds,
                                                   bsub_sample, subsample_ds,
                                                   leaf_edge,
                                                   scale_ratio);

  return shock_curve;
}

/*
//: \relates dbsk2d_shock_graph
dbskr_scurve_sptr dbskr_compute_scurve_no_A_inf_samples(dbsk2d_shock_node_sptr start_node,
                                       vcl_vector<dbsk2d_shock_edge_sptr> path,
                                       bool binterpolate, bool bsub_sample,
                                       double interpolate_ds, double subsample_ds)
{
  vcl_vector< vgl_point_2d<double> > sh_pt;
  vcl_vector<double> time, theta, phi;

  dbsk2d_shock_node_sptr cur_start_node = start_node;

  //traverse through the path, interpolating where necessary
  for (vcl_vector<dbsk2d_shock_edge_sptr>::iterator e_it = path.begin();
       e_it != path.end(); e_it++)
  {
    dbsk2d_xshock_edge* cur_edge = dynamic_cast<dbsk2d_xshock_edge*>(e_it->ptr());

    if (cur_start_node == cur_edge->source())
    {
      //this edge is to be reconstructed the right way

      //go through the edge samples and append it to the shock curve
      vgl_point_2d<double> prev_sh_pt;
      for (int i=0; i< cur_edge->num_samples(); i++)
      {
        dbsk2d_xshock_sample_sptr sample = cur_edge->sample(i);
  
        if (!(e_it == path.end()-1 && i > 0 && (vgl_distance(sample->pt, prev_sh_pt) < 0.01))) {
          sh_pt.push_back(sample->pt);
          time.push_back(sample->radius); 
          if (sample->speed != 0 && sample->speed < 99990) //100000 signals infinity
            phi.push_back(vcl_acos(-1/sample->speed));
          else
            phi.push_back(vnl_math::pi/2);

          theta.push_back(sample->theta);//direction of flow is same as actual
        }

        prev_sh_pt = sample->pt;
      }
    }
    else 
    {
      //this edge is to be flipped

      //go through the edge samples in the reverse order and append it to the shock curve
      vgl_point_2d<double> prev_sh_pt;
      for (int i=cur_edge->num_samples()-1; i>=0 ; i--)
      {
        dbsk2d_xshock_sample_sptr sample = cur_edge->sample(i);

        if (!(e_it == path.end()-1 && i < cur_edge->num_samples()-1 && (vgl_distance(sample->pt, prev_sh_pt) < 0.01))) {
          sh_pt.push_back(sample->pt);
          time.push_back(sample->radius); 
          if (sample->speed != 0 && sample->speed < 99990) //100000 signals infinity
            phi.push_back(vnl_math::pi - vcl_acos(-1/sample->speed));
          else
            phi.push_back(vnl_math::pi/2);
          
          theta.push_back(angle0To2Pi(sample->theta+vnl_math::pi));//direction of flow is reversed
        }

        prev_sh_pt = sample->pt;
      }
    }

    //update the cur_start_node to the opposite node
    cur_start_node = cur_edge->opposite(cur_start_node);
  }

  //construct the shock curve from the compiled information without interpolation
  dbskr_scurve_sptr shock_curve = new dbskr_scurve(sh_pt.size(),
                                                   sh_pt, time, theta, phi,
                                                   binterpolate, interpolate_ds,
                                                   bsub_sample, subsample_ds);

  return shock_curve;
}
*/

