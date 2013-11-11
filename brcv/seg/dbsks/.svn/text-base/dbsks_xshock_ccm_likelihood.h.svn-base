// This is seg/dbsks/dbsks_xshock_ccm_likelihood.h
#ifndef dbsks_xshock_ccm_likelihood_h_
#define dbsks_xshock_ccm_likelihood_h_

//:
// \file
// \brief A class to compute likelihood of an xshock_graph using a
// a Contour Chamfer Matching (CCM) cost model.
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date June 20, 2009
//
// \verbatim
//    Nhon Trinh  June 20, 2009   initial version
//  Modifications
// \endverbatim



#include <dbsks/dbsks_xshock_likelihood.h>
#include <dbsks/dbsks_xgraph_ccm_model_sptr.h>
#include <dbsks/dbsks_biarc_sampler.h>
#include <dbsks/dbsks_subpix_ccm.h>
#include <dbsksp/dbsksp_xshock_graph_sptr.h>
#include <dbdet/edge/dbdet_edgemap_sptr.h>
#include <vcl_map.h>


// ============================================================================
// dbsks_xshock_ccm_likelihood_wcm
// ============================================================================

//: Compute likehood of an xshock fragment/graph using Whole-Contour-Matching
class dbsks_xshock_ccm_likelihood : public dbsks_xshock_likelihood
{
public:
  // Constructor / destructor -------------------------------------------------
  
  //: default constructor
  dbsks_xshock_ccm_likelihood(): xgraph_ccm_(0), ccm_subpix_(0), biarc_sampler_(0), 
    active_edge_id_(0), active_xfrag_ccm_(0){};

  //: destructor
  virtual ~dbsks_xshock_ccm_likelihood();

  // Access memeber variables --------------------------------------------------

  //: Return ccm model for xgraph
  dbsks_xgraph_ccm_model_sptr xgraph_ccm() const
  { return this->xgraph_ccm_; }

  //: Set ccm model
  void set_ccm_model(const dbsks_xgraph_ccm_model_sptr& xgraph_ccm)
  {
    this->xgraph_ccm_ = xgraph_ccm;
  }

  //: Return edgemap
  dbdet_edgemap_sptr edgemap() const
  {
    return this->edgemap_; 
  }

  //: Set edge map
  void set_edgemap(const dbdet_edgemap_sptr& edgemap)
  {
    this->edgemap_ = edgemap;
  }

  //: Return pointer to the biarc sampler (should be smart pointer)
  dbsks_biarc_sampler* biarc_sampler() const
  { return this->biarc_sampler_; }

  //: Set biarc sampler
  void set_biarc_sampler(dbsks_biarc_sampler* biarc_sampler)
  {
    this->biarc_sampler_ = biarc_sampler;
  }

  // Inherited functions -------------------------------------------------------

  //: Return platform-independent name of the class
  virtual vcl_string is_a() const { return "dbsks_xshock_ccm_likelihood"; }
  
  //: Likelihood of an xshock fragment
  virtual double loglike(unsigned edge_id, const dbsksp_xshock_fragment& xfrag) const;

  //: Return lower bounds of the likelihood function
  virtual double loglike_lowerbound(unsigned edge_id) const;

  //: Return upper bound of the likelihood function
  virtual double loglike_upperbound(unsigned edge_id) const;

  //: Update cache values to a new edge
  void update_cache(unsigned edge_id) const;

  //: Likelihood of a xshock fragment, also return the samples that are used
  double loglike(unsigned edge_id, const dbsksp_xshock_fragment& xfrag, 
    vcl_vector<int >& left_x_vec, vcl_vector<int >& left_y_vec, vcl_vector<int >& left_angle_vec, 
    vcl_vector<int >& right_x_vec, vcl_vector<int >& right_y_vec, vcl_vector<int >& right_angle_vec) const;
  

  // Utilities -----------------------------------------------------------------

  //: Prepare CCM cost calculator
  bool compute_internal_data(const vgl_box_2d<int >& roi);


  // Supporting functions ------------------------------------------------------
  
  //: Contour-Chamfer-Matching (CCM) calculator
  dbsks_subpix_ccm* ccm() const { return this->ccm_subpix_; }

  //: Set the CCM calculator
  void set_ccm(dbsks_subpix_ccm* ccm) { this->ccm_subpix_ = ccm; }

  //: Computer lower and upper bounds for likelihood for each edge
  bool estimate_loglike_bounds();

protected:

  
  
  // Member variables ----------------------------------------------------------

  //>> Input data >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  
  //: Edge map of the image
  dbdet_edgemap_sptr edgemap_;

  //: A model to compute likelihood of a CCM cost
  dbsks_xgraph_ccm_model_sptr xgraph_ccm_;

  //: A biarc sampler to compute sample points of a biarc quickly
  dbsks_biarc_sampler* biarc_sampler_;

  //>> Internal data to assist likelihood computation >>>>>>>>>>>>>>>>>>>>>>>>>>

  //: Contour-Chamfer-Matching cost calculator
  dbsks_subpix_ccm* ccm_subpix_;

  //: Map to store lower bound and upper bound of likelihood functions of each edge
  vcl_map<unsigned, double > map_eid_to_lowerbound_;
  vcl_map<unsigned, double > map_eid_to_upperbound_;


  // Cache values

  //: Id of last-used edge
  mutable unsigned active_edge_id_;

  //: CCM model of last-used edge
  mutable dbsks_xfrag_ccm_model_sptr active_xfrag_ccm_;

  mutable double active_loglike_lowerbound_;
  mutable double active_loglike_upperbound_;
};




#endif // seg/dbsks/dbsks_xshock_ccm_likelihood.h


