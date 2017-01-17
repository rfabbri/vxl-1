// This is file seg/dbsks/dbsks_xshock_ccm_likelihood.cxx

//:
// \file


#include "dbsks_xshock_ccm_likelihood.h"

#include <dbsksp/dbsksp_xshock_graph.h>
#include <dbsks/dbsks_biarc_sampler.h>
#include <dbsks/dbsks_subpix_ccm.h>
#include <dbsks/dbsks_xgraph_ccm_model.h>
#include <dbsksp/algo/dbsksp_trace_boundary.h>
#include <vcl_map.h>
#include <vnl/vnl_math.h>
#include <dbnl/dbnl_angle.h>


// ============================================================================
// dbsks_xshock_ccm_likelihood
// ============================================================================




// -----------------------------------------------------------------------------
//: destructor
dbsks_xshock_ccm_likelihood::
~dbsks_xshock_ccm_likelihood()
{
  if (this->ccm_subpix_)
  {
    delete (this->ccm_subpix_);
    this->ccm_subpix_ = 0;
  }
  return;
}








// -----------------------------------------------------------------------------
//: Prepare CCM cost calculator
bool dbsks_xshock_ccm_likelihood::
compute_internal_data(const vgl_box_2d<int >& roi)
{
  //> extract param values from ccm model
  dbsks_ccm_params params = this->xgraph_ccm()->ccm_params();

  //> Create an CCM object if necessary
  if (!this->ccm())
  {
    this->set_ccm(new dbsks_subpix_ccm());
  }  

  // apply the params to ccm
  this->ccm()->set_distance_params(params.distance_threshold_, params.distance_tol_near_zero_);
  this->ccm()->set_orient_params(params.orient_threshold_, params.orient_tol_near_zero_);
  this->ccm()->set_nbins_0topi(params.nbins_0topi_);
  this->ccm()->set_local_window_width(params.local_window_width_);
  this->ccm()->set_weights(params.weight_chamfer_, params.weight_edge_orient_, params.weight_contour_orient_);

  // set data source to ccm
  this->ccm()->set_edgemap(this->edgemap());

  // compute cost map of ccm for the region-of-interest
  this->ccm()->compute(roi);


  // compute lower and upper bounds of the cost for the ROI
  return this->estimate_loglike_bounds();

  //return true;
}




//------------------------------------------------------------------------------
//: Computer lower and upper bounds for likelihood for each edge
bool dbsks_xshock_ccm_likelihood::
estimate_loglike_bounds()
{
  // clear up existing estimates, if any
  this->map_eid_to_lowerbound_.clear();
  this->map_eid_to_upperbound_.clear();

  // Estimate lower bound and upper bounds of the likelihood function
  float xmin = this->ccm()->compute_f_lower_bound();
  float step = 0.01f;
  float xmax = this->ccm()->compute_f_upper_bound();

  //Check if sth is wrong
  if(xmin > xmax)
  {
      return false;
  }
  //

  vcl_cout << "\n> Range of ccm cost: [min, max] = [" << xmin << " , " << xmax << "]\n";

  // count number of elements from xmin to xmax
  int num_samples = 0;
  for (float x = xmin; x <= xmax; x += step)
  {
    ++num_samples;
  }
  // form a vector of samples
  vnl_vector<double > x(num_samples);
  for (int i =0; i < num_samples; ++i)
  {
    x[i] = xmin + i*step;
  }

  // Compute min and max likelihood for each edge using the computed list of samples
  typedef vcl_map<unsigned, dbsks_xfrag_ccm_model_sptr >::iterator xfrag_ccm_iter;
  for (xfrag_ccm_iter iter = this->xgraph_ccm()->map_edge2ccm().begin(); 
    iter != this->xgraph_ccm()->map_edge2ccm().end(); ++iter)
  {
    unsigned eid = iter->first;
    dbsks_xfrag_ccm_model_sptr xfrag_ccm = iter->second;
    vnl_vector<double > fx_left(x.size());
    vnl_vector<double > fx_right(x.size());

    for (unsigned i =0; i < x.size(); ++i)
    {
      fx_left[i] = xfrag_ccm->log_likelihood(0, x[i]);
      fx_right[i] = xfrag_ccm->log_likelihood(1, x[i]);
    }

    // since loglike of a fragment is (currently) computed as sum of loglike of
    // its two boundary fragments, we can estimate the min and max of loglike using
    // the min and max of loglike of each fragment
    this->map_eid_to_lowerbound_[eid] = fx_left.min_value() + fx_right.min_value();
    this->map_eid_to_upperbound_[eid] = fx_left.max_value() + fx_right.max_value();
  }
  return true;
}



//------------------------------------------------------------------------------
//: Likelihood of a xshock fragment
double dbsks_xshock_ccm_likelihood::
loglike(unsigned edge_id, const dbsksp_xshock_fragment& xfrag) const
{
  static vcl_vector<int > left_x_vec, left_y_vec, left_angle_vec;
  static vcl_vector<int > right_x_vec, right_y_vec, right_angle_vec;

  return this->loglike(edge_id, xfrag, left_x_vec, left_y_vec, left_angle_vec,
    right_x_vec, right_y_vec, right_angle_vec);

  //// update cache values with new edge
  //this->update_cache(edge_id);

  //// cost model for the fragment
  //dbsks_xfrag_ccm_model_sptr xfrag_ccm = this->active_xfrag_ccm_;
  //
  //// compute cost
  //float cost_left, cost_right;

  //static vcl_vector<int > x_vec, y_vec, angle_vec;
  //dbsksp_xshock_node_descriptor start = xfrag.start();
  //dbsksp_xshock_node_descriptor end = xfrag.end();

  //// left boundary
  //this->biarc_sampler_->compute_samples_using_cache(start.bnd_pt_left(), start.bnd_tangent_left(),
  //  end.bnd_pt_left(), end.bnd_tangent_left(), x_vec, y_vec, angle_vec);
  //
  //if (x_vec.empty())
  //{
  //  cost_left = 1.0f;
  //}
  //else
  //{
  //  cost_left = this->ccm()->f(x_vec, y_vec, angle_vec);
  //}
  //
  //// right boundary
  //this->biarc_sampler_->compute_samples_using_cache(start.bnd_pt_right(), start.bnd_tangent_right(), 
  //  end.bnd_pt_right(), end.bnd_tangent_right(),
  //  x_vec, y_vec, angle_vec);
  //if (x_vec.empty())
  //{
  //  cost_right = 1.0f;
  //}
  //else
  //{
  //  cost_right = this->ccm()->f(x_vec, y_vec, angle_vec);
  //}

  //
  //// overall cost of the fragment
  //double left_like = xfrag_ccm->log_likelihood(0, cost_left);
  //double right_like = xfrag_ccm->log_likelihood(1, cost_right);


  //// loglike of the fragment is the loglike of its two boundary
  //float sum_like = float( left_like + right_like );
  //
  //return sum_like;
}



//------------------------------------------------------------------------------
//: Return lower bounds of the likelihood function
double dbsks_xshock_ccm_likelihood::
loglike_lowerbound(unsigned edge_id) const
{
  this->update_cache(edge_id);
  return this->active_loglike_lowerbound_;
}


//------------------------------------------------------------------------------
//: Return upper bound of the likelihood function
double dbsks_xshock_ccm_likelihood::
loglike_upperbound(unsigned edge_id) const
{
  this->update_cache(edge_id);
  return this->active_loglike_upperbound_;
}


//------------------------------------------------------------------------------
//: Update cache values to a new edge
void dbsks_xshock_ccm_likelihood::
update_cache(unsigned edge_id) const
{
  if (this->active_edge_id_ == edge_id)
    return;

  // update active_xfrag_ccm
  this->active_xfrag_ccm_ = this->xgraph_ccm_->map_edge2ccm().find(edge_id)->second;

  // update lower and upper bounds of loglikelihood
  this->active_loglike_lowerbound_ = this->map_eid_to_lowerbound_.find(edge_id)->second;
  this->active_loglike_upperbound_ = this->map_eid_to_upperbound_.find(edge_id)->second;
   
  // finally, update edge id
  this->active_edge_id_ = edge_id;
  return;
}



//------------------------------------------------------------------------------
//: Likelihood of a xshock fragment, also return the samples that are used
double dbsks_xshock_ccm_likelihood::
loglike(unsigned edge_id, const dbsksp_xshock_fragment& xfrag, 
        vcl_vector<int >& left_x_vec, vcl_vector<int >& left_y_vec, vcl_vector<int >& left_angle_vec, 
        vcl_vector<int >& right_x_vec, vcl_vector<int >& right_y_vec, vcl_vector<int >& right_angle_vec) const
{
  // update cache values with new edge
  this->update_cache(edge_id);

  // cost model for the fragment
  dbsks_xfrag_ccm_model_sptr xfrag_ccm = this->active_xfrag_ccm_;
  
  // compute cost
  float cost_left, cost_right;
  dbsksp_xshock_node_descriptor start = xfrag.start();
  dbsksp_xshock_node_descriptor end = xfrag.end();

  // left boundary
  this->biarc_sampler_->compute_samples_using_cache(start.bnd_pt_left(), start.bnd_tangent_left(),
    end.bnd_pt_left(), end.bnd_tangent_left(), left_x_vec, left_y_vec, left_angle_vec);
  
  if (left_x_vec.empty())
  {
    cost_left = 1.0f;
  }
  else
  {
    cost_left = this->ccm()->f(left_x_vec, left_y_vec, left_angle_vec);
  }
  
  // right boundary
  this->biarc_sampler_->compute_samples_using_cache(start.bnd_pt_right(), start.bnd_tangent_right(), 
    end.bnd_pt_right(), end.bnd_tangent_right(),
    right_x_vec, right_y_vec, right_angle_vec);
  if (right_x_vec.empty())
  {
    cost_right = 1.0f;
  }
  else
  {
    cost_right = this->ccm()->f(right_x_vec, right_y_vec, right_angle_vec);
  }

  
  // overall cost of the fragment
  double left_like = xfrag_ccm->log_likelihood(0, cost_left);
  double right_like = xfrag_ccm->log_likelihood(1, cost_right);


  // loglike of the fragment is the loglike of its two boundary
  float sum_like = float( left_like + right_like );
  
  return sum_like;
}



