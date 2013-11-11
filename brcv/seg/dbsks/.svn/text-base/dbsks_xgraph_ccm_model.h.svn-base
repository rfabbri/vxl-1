// This is shp/dbsks/dbsks_xgraph_ccm_model.h

#ifndef dbsks_xgraph_ccm_model_h_
#define dbsks_xgraph_ccm_model_h_

//:
// \file
// \brief A model for contour ocm response of an xshock graph
//        
// \author Nhon Trinh ( ntrinh@lems.brown.edu )
// \date Dec 18, 2008
//
// \verbatim
//  Modifications
// \endverbatim


#include <dbsks/dbsks_bfrag_cost_model.h>
#include <dbsks/dbsks_xgraph_ccm_model_sptr.h>
#include <dbsksp/dbsksp_xshock_graph_sptr.h>

#include <vbl/vbl_ref_count.h>
#include <vnl/vnl_vector.h>

#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vcl_vector.h>
#include <vcl_map.h>



// =============================================================================
// dbsks_xfrag_ccm_model
// =============================================================================


//: A statistical model for Contour Oriented Chamfer cost of a xshock fragment
class dbsks_xfrag_ccm_model: public vbl_ref_count
{
public:

  // Constructors / Destructors ------------------------------------------------
  
  //: constructor
  dbsks_xfrag_ccm_model();

  //: destructor
  virtual ~dbsks_xfrag_ccm_model(){};
  
  // DATA ACCESS ---------------------------------------------------------------

  dbsks_bfrag_cost_model& bfrag_model(int side_id)
  { return this->bfrag_model_[side_id]; };

  const dbsks_bfrag_cost_model& bfrag_model(int side_id) const
  { return this->bfrag_model_[side_id]; };

  void set_bfrag_model(int side_id, const dbsks_bfrag_cost_model& bfrag_model)
  { this->bfrag_model_[side_id] = bfrag_model; }

  

  // Depreciated functions /////////////////////////////////////////////////////
  //: Get the distribution of boundary's contour ocm 
  // [0]: left, [1]: right
  bsta_weibull<double >& bnd_weibull_distribution(int bnd_side)
  {
    //return this->bnd_weibull_distribution_[bnd_side];
    return this->bfrag_model_[bnd_side].weibull_dist_[0];
  }

  //: Set the statistical distribution of boundary's contour ocm
  // side_id = 0: left side
  // side_id = 1: right side
  void set_bnd_weibull_distribution(int side_id, const bsta_weibull<double >& weibull_dist)
  {
    assert(side_id == 0 || side_id == 1);
    //this->bnd_weibull_distribution_[side_id] = weibull_dist;
    this->bfrag_model_[side_id].weibull_dist_[0] = weibull_dist;
  }
  
  //: Set the active distribution used for each boundary contour of this fragment
  void set_active_bnd_distribution(int side_id, dbsks_ccm_dist_type new_type)
  {
    //this->active_bnd_distribution_[side_id] = new_type;
    this->bfrag_model_[side_id].active_dist_type_[0] = new_type;
  }
  //////////////////////////////////////////////////////////////////////////////


  // UTILITY -------------------------------------------------------------------

  //: Compute negative-log of probability 
  double log_likelihood(int side_id, double ccm_cost);

  //: Clear cache values for log-likelihood
  void clear_cache_loglike();

  //: Compute cache values of log-likelihood for weibull distribution
  // max_snr_in_dB (maximum signal-to-noise-ration in decibel): 
  // is the maximum ratio between the max likelihood and min likelihood
  bool compute_cache_loglike(double lower_bound, double upper_bound, 
    int num_intervals, double max_snr_in_dB = 20);

  // I/O -----------------------------------------------------------------------

  //: write info of the dbskbranch to an output stream
  virtual vcl_ostream& print(vcl_ostream & os){return os; };

protected:

  // Cache values of loglikelihood
  bool cache_loglike_available_; // true if cache values have been computed
  double cache_loglike_xmin_; // min cost values whose likelihood have been cached
  double cache_loglike_xmax_; // max cost values whose likelihood have been cached
  double cache_loglike_dx_;
  vnl_vector<double > cache_loglike_[2]; // cache values of log-likelihood

  //: 
  dbsks_bfrag_cost_model bfrag_model_[2];


  //// Active distribution
  //dbsks_ccm_dist_type active_bnd_distribution_[2];

  //// Keep distribution on two boundary contours separately
  //// [0] : left boundary
  //// [1] : right boundary
  //bsta_weibull<double > bnd_weibull_distribution_[2];
};




// =============================================================================
// dbsks_ccm_params
// =============================================================================

//: Parameters to construct a Contour-Chamfer-Matching cost calculator
struct dbsks_ccm_params
{
  //: Normalization constant for Chamfer distance. All edges farther than this 
  // will be ignored
  float distance_threshold_; 

  //: distance tolerance due to discretization
  float distance_tol_near_zero_;

  //: Normalization constant for orientation. 
  float orient_threshold_;

  // orientation tolerance due to discretization
  float orient_tol_near_zero_;

  //: weight of Chamfer cost
  float weight_chamfer_; 

  //: weight of edge orientation cost
  float weight_edge_orient_;

  //: weight of contour orientation cost
  float weight_contour_orient_; 

  //: number of angle bins from 0 to pi
  int nbins_0topi_;

  //: Width of window around each oriented point to search for supporting edge
  float local_window_width_;
};




// =============================================================================
// dbsks_xgraph_ccm_model
// =============================================================================

class dbsks_biarc_sampler;

//: A generative model for generating xshock_fragment
class dbsks_xgraph_ccm_model: public vbl_ref_count
{
public:
  
  // Constructors / Destructors ------------------------------------------------
  
  //: constructor
  dbsks_xgraph_ccm_model(){};

  //: destructor
  virtual ~dbsks_xgraph_ccm_model(){};
  
  // DATA ACCESS ---------------------------------------------------------------

  //: Return id of root node
  unsigned int root_vid() const {return this->root_vid_;}

  //: Set id of root node
  void set_root_vid(unsigned root_vid){ this->root_vid_ = root_vid; }

  //: Return the scale of xgraph where all models are based upon
  double base_xgraph_size() const {return this->base_xgraph_size_; }

  //: Set base xgraph scale
  void set_base_xgraph_size(double new_scale)
  { this->base_xgraph_size_ = new_scale; }

  //: Return parameter set to build CCM cost calculator
  dbsks_ccm_params ccm_params() const
  { return this->ccm_params_; }

  //: Set the parameter set to build CCM cost calculator
  void set_ccm_params(const dbsks_ccm_params& ccm_params)
  { this->ccm_params_ = ccm_params; }

  //: Return model for xfrag ccm cost
  dbsks_xfrag_ccm_model_sptr xfrag_model(unsigned edge_id)
  { return this->map_edge2ccm_[edge_id]; }

  //: Set xfrag_ccm_model for an edge
  void set_xfrag_model(unsigned edge_id, const dbsks_xfrag_ccm_model_sptr& new_model)
  { this->map_edge2ccm_[edge_id] = new_model; }

  //: Build a biarc sampler compatible with parameters of this model
  void build_biarc_sampler(dbsks_biarc_sampler& biarc_sampler) const;

  




  // BEGIN - TO BE DEPRECIATED FUNCTIONS -----------------------------------------------

  //: Get and set parameters necessary to compute CCM cost
  void get_ccm_params(float& edge_threshold, float& tol_near_zero, float& distance_threshold,
    float& ccm_gamma, float& ccm_lambda, int& nbins_0topi);
  

  void set_ccm_params(float edge_threshold, float tol_near_zero, float distance_threshold,
    float ccm_gamma, float ccm_lambda, int nbins_0topi);
  // END - TO BE DEPRECIATED FUNCTIONS -----------------------------------------------

  vcl_map<unsigned, dbsks_xfrag_ccm_model_sptr >& map_edge2ccm()
  { return this->map_edge2ccm_; }

  const vcl_map<unsigned, dbsks_xfrag_ccm_model_sptr >& map_edge2ccm() const
  { return this->map_edge2ccm_; }


  // UTILITIES -----------------------------------------------------------------

  //: Override the probability distribution in the xfrag's boundary with constant distribution
  // each boundary fragment should be written in format "edgeid-sideid". E.g. "13-R" means
  // the right boundary fragment of edge 13.
  bool override_cfrag_with_constant_distribution(const vcl_vector<vcl_string >& cfrag_list);

  //: Override the probability distribution in the xfrag's boundary with constant distribution
  // list of bfrags is concatenated in a list, with a delimitor of choice. Avoid 0-9, L, R, -.
  // Example: ...("13-R,13-L,4-L", ',');
  bool override_cfrag_with_constant_distribution(const vcl_string& bfrag_list, char delimiter = ',');

  //: Return true if this model is compatible with a given xgraph topology
  bool is_compatible(const dbsksp_xshock_graph_sptr& xgraph);

  //: Compute cache values for the likelihood of all fragments
  bool compute_cache_loglike_for_all_edges();
  
  // I/O -----------------------------------------------------------------------

  //: write info of the dbskbranch to an output stream
  virtual vcl_ostream& print(vcl_ostream & os){ return os; };

  // MEMBER VARIABLES ----------------------------------------------------------
protected:
  unsigned int root_vid_;
  double base_xgraph_size_;
  
  //: Parameters to compute a CCM cost calculator
  dbsks_ccm_params ccm_params_;

  vcl_map<unsigned, dbsks_xfrag_ccm_model_sptr > map_edge2ccm_;
};

#endif // shp/dbsksp/dbsks_xgraph_ccm_model.h


