// This is seg/dbsks/dbsks_xshock_likelihood.h
#ifndef dbsks_xshock_likelihood_h_
#define dbsks_xshock_likelihood_h_

//:
// \file
// \brief A base class to compute likelihood of an xshock graph/fragment, taking into
// account the matching between the xgraph and the image and the distribution of the cost
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date Feb 7, 2008
// \verbatim
//    Modifications
//  
// \endverbatim



#include <dbsksp/dbsksp_xshock_fragment.h>
#include <dbsksp/dbsksp_xshock_graph_sptr.h>

// ============================================================================
// dbsks_xshock_likelihood
// ============================================================================

//: A base class for computing likehood of an xshock fragment/graph for a given
// object class
class dbsks_xshock_likelihood
{
public:
  // Constructor / destructor -------------------------------------------------
  
  //: constructor
  dbsks_xshock_likelihood(){};

  //: destructor
  virtual ~dbsks_xshock_likelihood(){};

  // Access memeber variables -------------------------------------------------

  // Utilities ----------------------------------------------------------------

  //: Return platform-independent name of the class
  virtual vcl_string is_a() const { return "dbsks_xshock_likelihood"; }

  //: Likelihood of a xshock fragment
  virtual double loglike(unsigned edge_id, const dbsksp_xshock_fragment& xfrag) const = 0;

  //: Return lower bounds of the likelihood function
  virtual double loglike_lowerbound(unsigned edge_id) const;

  //: Return upper bound of the likelihood function
  virtual double loglike_upperbound(unsigned edge_id) const;

  //: Likelihood of a xshock graph
  virtual double loglike_xgraph(const dbsksp_xshock_graph_sptr& xgraph, 
    const vcl_vector<unsigned >& ignored_edges, bool verbose) const;

protected:
};


#endif // seg/dbsks/dbsks_xshock_likelihood.h


