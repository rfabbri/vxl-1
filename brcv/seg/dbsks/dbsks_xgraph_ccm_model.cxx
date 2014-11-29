// This is file shp/dbsks/dbsks_xgraph_ccm_model.cxx

//:
// \file

#include "dbsks_xgraph_ccm_model.h"
#include <dbsksp/dbsksp_xshock_graph_sptr.h>
#include <dbsksp/dbsksp_xshock_graph.h>
#include <vcl_cmath.h>
#include <vcl_sstream.h>
#include <vcl_utility.h>
#include <vnl/vnl_math.h>
#include <dbsks/dbsks_biarc_sampler.h>


// =============================================================================
// dbsks_xfrag_ccm_model
// =============================================================================

//------------------------------------------------------------------------------
//: constructor
dbsks_xfrag_ccm_model::
dbsks_xfrag_ccm_model()
{
  this->cache_loglike_available_ = false;
}



// -----------------------------------------------------------------------------
//: Compute log of probability 
double dbsks_xfrag_ccm_model::
log_likelihood(int side_id, double ccm_cost)
{
  if (this->cache_loglike_available_)
  {
    // push the query cost into the allowed range
    ccm_cost = vnl_math::max(ccm_cost, this->cache_loglike_xmin_);
    ccm_cost = vnl_math::min(ccm_cost, this->cache_loglike_xmax_);
    int idx = vnl_math::floor((ccm_cost-this->cache_loglike_xmin_) / this->cache_loglike_dx_);
    return this->cache_loglike_[side_id][idx];  
  }
  else 
  {
    return this->bfrag_model_[side_id].log_likelihood_ratio(ccm_cost);
  }
}




//------------------------------------------------------------------------------
//: Clear cache values for log-likelihood
void dbsks_xfrag_ccm_model::
clear_cache_loglike()
{
  this->cache_loglike_available_ = false;
  this->cache_loglike_xmin_ = this->cache_loglike_xmax_ = 0;
  this->cache_loglike_[0].clear();
  this->cache_loglike_[1].clear();
  return;
}



// -----------------------------------------------------------------------------
//: Compute cache values of log-likelihood
// max_snr_in_dB (maximum signal-to-noise-ration in decibel): 
// is the maximum ratio between the max likelihood and min likelihood
bool dbsks_xfrag_ccm_model::
compute_cache_loglike(double xmin, double xmax, int nbins, double max_snr_in_dB)
{
  // clean up
  this->cache_loglike_available_ = false;
  this->cache_loglike_[0].clear();
  this->cache_loglike_[1].clear();

  // preliminary checks
  if (xmin > xmax)
    return false;

  if (nbins <= 0)
    return false;

  // save parameters of cache values
  this->cache_loglike_xmin_ = xmin;
  this->cache_loglike_xmax_ = xmax;
  this->cache_loglike_dx_ = (xmax==xmin) ? 1 : (xmax-xmin) / nbins;

  for (int side_id = 0; side_id < 2; ++side_id)
  {
    // compute function values for the center points of the intervals
    double dx = this->cache_loglike_dx_;
    double x0 = xmin + dx / 2;
      
    // computer log-likelihood for each point between upper and lower bounds
    this->cache_loglike_[side_id].set_size(nbins+1);
    for (int i =0; i < nbins; ++i)
    {
      double x = x0 + i*dx;
      double fx = this->bfrag_model(side_id).log_likelihood_ratio(x);
      this->cache_loglike_[side_id][i] = fx;
    }

    // duplicate the last value to avoid the need to adjust the index when ccm_cost = xmax
    this->cache_loglike_[side_id][nbins] = this->cache_loglike_[side_id][nbins-1];

    // enforce that maximum SNR is within a pre-determined bound
    double max_loglike = this->cache_loglike_[side_id].max_value();
    double min_loglike = max_loglike - vnl_math::ln10 * (max_snr_in_dB / 10);

    for (int i =0; i <= nbins; ++i)
    {
      double cur_loglike = this->cache_loglike_[side_id][i];
      this->cache_loglike_[side_id][i] = vnl_math::max(min_loglike, cur_loglike);
    }
  } // side_id

  this->cache_loglike_available_ = true;
  return true;  
}






//==============================================================================
// dbsks_xgraph_ccm_model
//==============================================================================


//------------------------------------------------------------------------------
//: Build a biarc sampler compatible with parameters of this model
void dbsks_xgraph_ccm_model::
build_biarc_sampler(dbsks_biarc_sampler& biarc_sampler) const
{
  biarc_sampler.clear();

  //> Construct a biarc sampler ................................................
  vcl_cout << "\nConstructing a biarc sampler ...";

  // Set parameters of biarc sampler
  dbsks_biarc_sampler_params bsp;
  
  // max dx = 32
  bsp.step_dx = this->base_xgraph_size_ / 32; // heuristic
  bsp.num_dx = 33;
  bsp.min_dx = -bsp.step_dx * (bsp.num_dx-1)/2;

  // max dy = 32
  bsp.step_dy = this->base_xgraph_size_ / 32; // heuristic
  bsp.num_dy = 33;
  bsp.min_dy = -bsp.step_dy * (bsp.num_dy-1)/2;

  bsp.step_alpha0 = vnl_math::pi/17;
  bsp.num_alpha0 = 13;
  bsp.min_alpha0 = -bsp.step_alpha0*(bsp.num_alpha0-1)/2;

  bsp.step_alpha2 = vnl_math::pi/17;
  bsp.num_alpha2 = 13;
  bsp.min_alpha2 = -bsp.step_alpha2*(bsp.num_alpha2-1)/2;

  // compute coordinates of the grid points
  biarc_sampler.set_sampling_params(2*this->ccm_params_.nbins_0topi_, 1.5, 51);
  biarc_sampler.set_grid(bsp);
  return;
}


//------------------------------------------------------------------------------
//: Get and set parameters necessary to compute CCM cost
void dbsks_xgraph_ccm_model::
get_ccm_params(float& edge_threshold, float& tol_near_zero, float& distance_threshold,
               float& ccm_gamma, float& ccm_lambda, int& nbins_0topi)
{
  edge_threshold = 15.0f; // arbitrary, edges are meant to be binary
  tol_near_zero = this->ccm_params_.distance_tol_near_zero_; 
  distance_threshold = this->ccm_params_.distance_threshold_;
  ccm_gamma = this->ccm_params_.weight_contour_orient_;
  ccm_lambda = this->ccm_params_.weight_edge_orient_;
  nbins_0topi = this->ccm_params_.nbins_0topi_;
}

//------------------------------------------------------------------------------
//:
void dbsks_xgraph_ccm_model::
set_ccm_params(float edge_threshold, float tol_near_zero, float distance_threshold,
               float ccm_gamma, float ccm_lambda, int nbins_0topi)
{
  this->ccm_params_.distance_threshold_ = distance_threshold;
  this->ccm_params_.distance_tol_near_zero_ = tol_near_zero;
  this->ccm_params_.orient_threshold_ = float(vnl_math::pi_over_4);
  this->ccm_params_.orient_tol_near_zero_ =  float(vnl_math::pi / nbins_0topi) / 2;
  this->ccm_params_.weight_chamfer_ = 1.0f - ccm_lambda - ccm_gamma;
  this->ccm_params_.weight_edge_orient_ = ccm_lambda;
  this->ccm_params_.weight_contour_orient_ = ccm_gamma;
  this->ccm_params_.nbins_0topi_ = nbins_0topi;
  this->ccm_params_.local_window_width_ = 3.0f;
  return;
}


// -----------------------------------------------------------------------------
//: Override the probability distribution in the xfrag's boundary with constant distribution
// each boundary fragment should be written in format "edgeid-sideid". E.g. "13-R" means
// the right boundary fragment of edge 13.
bool dbsks_xgraph_ccm_model::
override_cfrag_with_constant_distribution(const vcl_vector<vcl_string >& cfrag_desc_list)
{
  // 1. Parse the string description
  vcl_vector<vcl_pair<unsigned int, int > > cfrag_list;
  cfrag_list.reserve(cfrag_desc_list.size());

  // parse each description one by one
  for (unsigned i =0; i < cfrag_desc_list.size(); ++i)
  {
    vcl_string cfrag_desc = cfrag_desc_list[i];
    
    // replace all the dashes "-" with space for ease of parsing
    for (unsigned j =0; j < cfrag_desc.size(); ++j)
    {
      if (cfrag_desc[j] == '-')
      {
        cfrag_desc[j] = ' ';
      }
    }

    // parse the description
    vcl_stringstream ss(cfrag_desc);
    unsigned edge_id;
    int side_id;
    
    // edge id
    ss >> edge_id;

    // side id
    vcl_string temp;
    ss >> temp;
    if (temp == "L")
    {
      side_id = 0;
    }
    else if (temp == "R")
    {
      side_id = 1;
    }
    else
    {
      return false;
    }
    cfrag_list.push_back(vcl_make_pair(edge_id, side_id));  
  }

  // now modify the distribution of each bnd contour fragment
  for (unsigned i =0; i < cfrag_list.size(); ++i)
  {
    unsigned edge_id = cfrag_list[i].first;

    // if not valid edge, move on
    if (this->map_edge2ccm().find(edge_id) == this->map_edge2ccm().end())
      continue;

    int side_id = cfrag_list[i].second;
    dbsks_xfrag_ccm_model_sptr xfrag_model = this->xfrag_model(edge_id);

    if (!xfrag_model)
      continue;
    
    // foreground
    xfrag_model->bfrag_model(side_id).active_dist_type_[FOREGROUND] = CONSTANT;
    xfrag_model->bfrag_model(side_id).constant_dist_[FOREGROUND] = 1;

    // background
    xfrag_model->bfrag_model(side_id).active_dist_type_[BACKGROUND] = CONSTANT;
    xfrag_model->bfrag_model(side_id).constant_dist_[BACKGROUND] = 1;

    

  }
  return true;
}



//------------------------------------------------------------------------------
//: Override the probability distribution in the xfrag's boundary with constant distribution
// list of bfrags is concatenated in a list, with a delimitor of choice. Avoid 0-9, L, R, -.
// Example: ...("13-R,13-L,4-L", ',');
bool dbsks_xgraph_ccm_model::
override_cfrag_with_constant_distribution(const vcl_string& concatenated_bfrag_descs, char delimiter)
{
  // parse the concatenated bfrag descriptions into a vector
  vcl_vector<vcl_string > cfrag_list_to_ignore;
  cfrag_list_to_ignore.clear();

  // parse the corresponding parameter
  vcl_string buffer = concatenated_bfrag_descs;
  
  // replace 'delimiter' by space so that we can use standard parser
  for (unsigned i =0; i < buffer.length(); ++i)
  {
    if (buffer[i] == delimiter)
    {
      buffer[i] =' ';
    }
  }
  
  // parse the string using stringstream
  vcl_stringstream ss;
  ss << buffer;

  vcl_string cfrag_desc;
  while (ss >> cfrag_desc)
  {
    cfrag_list_to_ignore.push_back(cfrag_desc);
  }
  
  return this->override_cfrag_with_constant_distribution(cfrag_list_to_ignore);
}




//------------------------------------------------------------------------------
//: Return true if this model is compatible with a given xgraph topology
bool dbsks_xgraph_ccm_model::
is_compatible(const dbsksp_xshock_graph_sptr& xgraph)
{
  bool has_all_edges = true;
  for (dbsksp_xshock_graph::edge_iterator eit = xgraph->edges_begin(); eit != 
    xgraph->edges_end(); ++eit)
  {
    vcl_map<unsigned, dbsks_xfrag_ccm_model_sptr >::iterator mit = 
      this->map_edge2ccm().find((*eit)->id());
    if (mit == this->map_edge2ccm().end())
    {
      has_all_edges = false;
      break;
    }
  }
  return has_all_edges;
}


//------------------------------------------------------------------------------
//: Compute cache values for the likelihood of all fragments
bool dbsks_xgraph_ccm_model::
compute_cache_loglike_for_all_edges()
{
  for (vcl_map<unsigned, dbsks_xfrag_ccm_model_sptr >::iterator mit = 
    this->map_edge2ccm_.begin(); mit != this->map_edge2ccm_.end(); ++mit)
  {
    dbsks_xfrag_ccm_model_sptr ccm = mit->second;
    //ccm->compute_cache_loglike(0, 1, 100, 20);
    ccm->compute_cache_loglike(0, 1, 100, 50);
  }
  return true;
}











