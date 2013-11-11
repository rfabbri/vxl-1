// This is seg/dbsks/dbsks_xshock_wcm_likelihood.h
#ifndef dbsks_xshock_wcm_likelihood_h_
#define dbsks_xshock_wcm_likelihood_h_

//:
// \file
// \brief Whole-Contour-Matching likelihood calculator
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date Feb 7, 2008
// \verbatim
//  Modifications
// \endverbatim



#include <dbsks/dbsks_xshock_likelihood.h>
#include <dbsks/dbsks_xgraph_ccm_model_sptr.h>
#include <dbsksp/dbsksp_xshock_graph_sptr.h>


class dbsks_wcm;
class dbsks_biarc_sampler;


// ============================================================================
// dbsks_xshock_wcm_likelihood
// ============================================================================

//: Compute likehood of an xshock fragment/graph using Whole-Contour-Matching
class dbsks_xshock_wcm_likelihood : public dbsks_xshock_likelihood
{
public:
  // Constructor / destructor -------------------------------------------------
  
  //: default constructor
  dbsks_xshock_wcm_likelihood(): xgraph_ccm_(0), wcm_(0), biarc_sampler_(0),
    active_edge_id_(0), active_xfrag_ccm_(0){};

  //: constructor 2
  dbsks_xshock_wcm_likelihood(const dbsks_xgraph_ccm_model_sptr& xgraph_ccm, 
    dbsks_wcm* wcm, dbsks_biarc_sampler* biarc_sampler): xgraph_ccm_(xgraph_ccm),
    wcm_(wcm), biarc_sampler_(biarc_sampler), active_edge_id_(0), active_xfrag_ccm_(0){};

  //: destructor
  virtual ~dbsks_xshock_wcm_likelihood(){};

  // Access memeber variables --------------------------------------------------

  //: Set
  void set(const dbsks_xgraph_ccm_model_sptr& xgraph_ccm, dbsks_wcm* wcm, 
    dbsks_biarc_sampler* biarc_sampler);

  // Utilities -----------------------------------------------------------------

  //: Return platform-independent name of the class
  vcl_string is_a() const { return "dbsks_xshock_wcm_likelihood"; }

  //: Likelihood of an xshock fragment
  virtual double loglike(unsigned edge_id, const dbsksp_xshock_fragment& xfrag) const;


  //: Likelihood of an xshock fragment given the whole shape's boundary
  double f_whole_contour(unsigned edge_id, const dbsksp_xshock_fragment& xfrag, 
    const vgl_polygon<double >& boundary);


  
  //: Likelihood of a xshock graph using whole contour matching
  double f_whole_contour(const dbsksp_xshock_graph_sptr& xgraph, 
    const vcl_vector<unsigned >& ignored_edges, bool verbose);

protected:
  //: A model for CCM cost of the graph
  dbsks_xgraph_ccm_model_sptr xgraph_ccm_;

  //: Whole-Contour-Matching engine
  dbsks_wcm* wcm_;
  dbsks_biarc_sampler* biarc_sampler_;

  /////////////////////////////////////
  // cache variables for fast access
  mutable unsigned active_edge_id_;
  mutable dbsks_xfrag_ccm_model_sptr active_xfrag_ccm_;
};




#endif // seg/dbsks/dbsks_xshock_wcm_likelihood.h


