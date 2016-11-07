// This is seg/dbsks/dbsks_xshock_app_likelihood.h
#ifndef dbsks_xshock_app_likelihood_h_
#define dbsks_xshock_app_likelihood_h_

//:
// \file
// \brief A base class to compute likelihood of an xshock graph, taking into
// account the matching between the xgraph appearance and ground truth
// appearance model
//        
// \author Maruthi Narayanan (mnh@lems.brown.edu)
// \date Feb 7, 2008
//
// \verbatim
//  Modifications
// \endverbatim

#include <dbsks/dbsks_xshock_likelihood.h>
#include <dbsksp/dbsksp_xshock_edge_sptr.h>
#include <vil/vil_image_view.h>
#include <vcl_map.h>

// ============================================================================
// dbsks_xshock_app_likelihood_app
// ============================================================================

//: Compute likehood of an xshock fragment/graph using Appearance Costs
class dbsks_xshock_app_likelihood : public dbsks_xshock_likelihood
{

public:
  // Constructor / destructor -------------------------------------------------
  
  //: default constructor
  dbsks_xshock_app_likelihood(const vcl_string& queryImage );

  //: destructor
  /* virtual */ 
  ~dbsks_xshock_app_likelihood(){};

  // Access memeber variables --------------------------------------------------

  // Utilities -----------------------------------------------------------------

  //: Likelihood of an xshock fragment
  /* virtual */ 
  double loglike(unsigned edge_id, const dbsksp_xshock_fragment& xfrag);

  //: Likelihood of a xshock graph
  /* virtual */ 
  double loglike(const dbsksp_xshock_graph_sptr& xgraph, 
    const vcl_vector<unsigned >& ignored_edges, bool verbose);

  //: Likelihood of an xshock fragment given the whole shape's boundary
  double f_whole_contour(unsigned edge_id, const dbsksp_xshock_fragment& xfrag, 
    const vgl_polygon<double >& boundary);


  //: Likelihood of a xshock graph using whole contour matching
  double f_whole_contour(const dbsksp_xshock_graph_sptr& xgraph, 
    const vcl_vector<unsigned >& ignored_edges, bool verbose);

 
private:

  // Store query image
  vil_image_view<vxl_byte> queryView_;

  // Converts a shock edge to a polygon
  // shock_edge->shock_fragment->polygon
  vgl_polygon<double> convert_edge_to_poly(bool& status,
                                           dbsksp_xshock_edge_sptr shockEdge);

  // Converts a shock fragment to a polygon
  // shock_fragment->polygon
  vgl_polygon<double> convert_frag_to_poly(bool& status,
                                           const dbsksp_xshock_fragment& xfrag);

  //: Appearance model holds a appearance based region descriptor
  //  for each fragment of the model
  //  Each histogram of appearance is accessed by the unique shock edge
  //  of the model 
  vcl_map< unsigned int,vcl_vector<double> > appearance_model_;
  
  //: Metric: variant on earth mover distance
  double emd(vcl_vector<double>& model, vcl_vector<double>& query);

  
};




#endif // seg/dbsks/dbsks_xshock_app_likelihood.h


