// This is shp/dbsks/dbsks_bfrag_cost_model.h
#ifndef dbsks_bfrag_cost_model_h_
#define dbsks_bfrag_cost_model_h_

//:
// \file
// \brief A model for computing likelihood of an xfrag
//        
// \author Nhon Trinh ( ntrinh@lems.brown.edu )
// \date Dec 18, 2008
//
// \verbatim
//  Modifications
// \endverbatim

//
//#include <dbsks/dbsks_bfrag_cost_model_sptr.h>
//
//#include <dbsksp/dbsksp_xshock_graph_sptr.h>
//
#include <bsta/bsta_weibull.h>
#include <bsta/bsta_histogram.h>
//
//#include <vbl/vbl_ref_count.h>
//#include <vnl/vnl_vector.h>
//#include <vcl_iostream.h>
//#include <vcl_string.h>
//#include <vcl_vector.h>
//#include <vcl_map.h>
//


// -----------------------------------------------------------------------------
//: type of distribution to fit ccm cost
enum dbsks_ccm_dist_type
{
  CONSTANT = 0,
  WEIBULL = 1,
  NONPARAM = 2,
};


enum dbsks_ccm_data_type
{
  FOREGROUND = 0,
  BACKGROUND = 1,
};


// =============================================================================
// dbsks_bfrag_cost_model
// =============================================================================

class dbsks_bfrag_cost_model
{
public:
  //: Constructor
  dbsks_bfrag_cost_model();

  //: Destructor
  ~dbsks_bfrag_cost_model() {};

  //: Probability-density for a given cost value
  // cost_type = 0: foreground
  // cost_type = 1: background
  double prob_density(int cost_type, double x) const;

  //: Log likelihood ratio for foreground and background
  double log_likelihood_ratio(double x) const;

  // Distribution parameters for foreground and background costs
  // [0]: foreground
  // [1]: background

  // Active distribution type
  dbsks_ccm_dist_type active_dist_type_[2];

  // Various probability 
  // Weibull distribution
  bsta_weibull<double > weibull_dist_[2];

  // Non-parametric distribution
  bsta_histogram<double > nonparam_dist_[2];

  // Constant distribution
  double constant_dist_[2];
};




//// =============================================================================
//// dbsks_xfrag_ccm_model
//// =============================================================================
//
//
////: A statistical model for Contour Oriented Chamfer cost of a xshock fragment
//class dbsks_xfrag_ccm_model: public vbl_ref_count
//{
//public:
//
//  // Constructors / Destructors ------------------------------------------------
//  
//  //: constructor
//  dbsks_xfrag_ccm_model();
//
//  //: destructor
//  virtual ~dbsks_xfrag_ccm_model(){};
//  
//  // DATA ACCESS ---------------------------------------------------------------
//  
//
//  //: Get the distribution of boundary's contour ocm 
//  // [0]: left, [1]: right
//  bsta_weibull<double >& bnd_weibull_distribution(int bnd_side)
//  {
//    return this->bnd_weibull_distribution_[bnd_side];
//  }
//
//  //: Set the statistical distribution of boundary's contour ocm
//  // side_id = 0: left side
//  // side_id = 1: right side
//  void set_bnd_weibull_distribution(int side_id, const bsta_weibull<double >& weibull_dist)
//  {
//    assert(side_id == 0 || side_id == 1);
//    this->bnd_weibull_distribution_[side_id] = weibull_dist;
//    //this->weibull_cache_available_ = false;
//  }
//
//  //: Return average log-likelihood of fitting weibull distribution to boundary cost
//  double avg_logl_fit_weibull(int side_id)
//  {
//    return this->avg_logl_fit_weibull_[side_id];
//  }
//  
//  //: Set average log-likelihood of fitting weibull distribution to boundary cost
//  void set_avg_logl_fit_weibull(int side_id, double avg_logl)
//  {
//    assert(side_id == 0 || side_id == 1);
//    this->avg_logl_fit_weibull_[side_id] = avg_logl;
//  }
//
//
//
//  //: Set the active distribution used for each boundary contour of this fragment
//  void set_active_bnd_distribution(int side_id, dbsks_ccm_dist_type new_type)
//  {
//    this->active_bnd_distribution_[side_id] = new_type;
//  }
//
//
//  // UTILITY -------------------------------------------------------------------
//
//  //: Compute negative-log of probability 
//  double log_likelihood(int side_id, double ccm_cost);
//
//  //: Clear cache values for log-likelihood
//  void clear_cache_loglike();
//
//  //: Compute cache values of log-likelihood for weibull distribution
//  // max_snr_in_dB (maximum signal-to-noise-ration in decibel): 
//  // is the maximum ratio between the max likelihood and min likelihood
//  bool compute_cache_loglike(double lower_bound, double upper_bound, 
//    int num_intervals, double max_snr_in_dB = 20);
//
//  // I/O -----------------------------------------------------------------------
//
//  //: write info of the dbskbranch to an output stream
//  virtual vcl_ostream& print(vcl_ostream & os){return os; };
//
//protected:
//
//  // Cache values of loglikelihood
//  bool cache_loglike_available_; // true if cache values have been computed
//  double cache_loglike_xmin_; // min cost values whose likelihood have been cached
//  double cache_loglike_xmax_; // max cost values whose likelihood have been cached
//  double cache_loglike_step_;
//  vnl_vector<double > cache_loglike_[2]; // cache values of log-likelihood
//
//  // Active distribution
//  dbsks_ccm_dist_type active_bnd_distribution_[2];
//
//  // Keep distribution on two boundary contours separately
//  // [0] : left boundary
//  // [1] : right boundary
//  bsta_weibull<double > bnd_weibull_distribution_[2];
//  
//  //: log-likelihood of fitting weibull distribution to boundary cost
//  double avg_logl_fit_weibull_[2];
//};



#endif // shp/dbsksp/dbsks_bfrag_cost_model.h


