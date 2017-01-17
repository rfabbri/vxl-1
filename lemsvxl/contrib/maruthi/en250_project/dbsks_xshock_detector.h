// This is seg/dbsks/dbsks_xshock_detector.h
#ifndef dbsks_xshock_detector_h_
#define dbsks_xshock_detector_h_

//:
// \file
// \brief Detecting an extrinsic shock graph in an image
//        
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date Oct 12, 2008
//
// \verbatim
//  Modifications
// \endverbatim



#include <en250_project/dbsks_xshock_app_likelihood.h>
#include <dbsks/dbsks_xnode_grid.h>
#include <dbsks/dbsks_ocm_image_cost_sptr.h>
#include <dbsks/dbsks_xfrag_geom_model_sptr.h>
#include <dbsks/dbsks_xgraph_geom_model_sptr.h>
#include <dbsks/dbsks_xgraph_ccm_model_sptr.h>
#include <dbsksp/dbsksp_xshock_graph_sptr.h>


#include <vgl/vgl_box_2d.h>
#include <vbl/vbl_ref_count.h>
#include <vil/vil_image_view.h>
#include <vcl_iostream.h>
#include <vcl_map.h>

// ============================================================================
// dbsks_xshock_detector
// ============================================================================

class dbsks_biarc_sampler;
class dbsks_shotton_ocm;
class dbsks_gray_ocm;
class dbsks_ccm;
class dbsks_xshock_wcm_likelihood;

//: A class for detecting existence of a shock graph in an image
class dbsks_xshock_detector : public vbl_ref_count
{
public:
  // Constructor / destructor -------------------------------------------------
  //: constructor
  dbsks_xshock_detector():root_vid_(17), major_child_eid_(0){};

  //: destructor
  virtual ~dbsks_xshock_detector(){};

  // User-interface functions -------------------------------------------------

  //: Detect the existence of the given extrinsic shock graph in the given image
  void detect(const vgl_box_2d<int >& window); 

  // Access memeber variables -------------------------------------------------

  //: Get and Set the prototype shock graph
  dbsksp_xshock_graph_sptr xgraph() const { return this->xgraph_; }
  void set_xgraph(const dbsksp_xshock_graph_sptr& xgraph)
  {
    this->xgraph_ = xgraph;
  }

  // Utility functions ---------------------------------------------------------

  //: Build the xnode grid
  void build_xnode_grid(const vgl_box_2d<int >& window);

protected:
  //: Build xnode grid using input xgraph as the only available info
  // All parameters are heuristic, hard-coded.
  void build_xnode_grid_using_only_input_xgraph(const vgl_box_2d<int >& window);

  //: Build xnode grid using xgraph geometric model (assume available)
  void build_xnode_grid_using_xgraph_geom_model(const vgl_box_2d<int >& window);

public:

  //: Reconstruct an xshock graph from a graph configuration
  dbsksp_xshock_graph_sptr reconstruct_xgraph(const vcl_map<unsigned, int >& xgraph_state);

  

  // I/O functions -------------------------------------------------------------

  //: Print Optimization results to a stream
  vcl_ostream& print_summary(vcl_ostream& str) const;

  // Intermediate data ---------------------------------------------------------

  //: Map of id of a vertex to its grid of states
  vcl_map<unsigned int, dbsks_xnode_grid > map_xnode_grid_;

  //: Image
  //vil_image_view<float > image_src_;

  //: Chamfer matching cost
  dbsks_ocm_image_cost_sptr ocm_;

  //: Biarc sampler
  dbsks_biarc_sampler* biarc_sampler_;

  //: Shotton OCM calculator
  dbsks_shotton_ocm* shotton_ocm_;

  //: Gray-value OCM calculator
  dbsks_gray_ocm* gray_ocm_;

  //: Contour OCM calculator
  dbsks_ccm* ccm_;

  // Grab generic cost function thing
  dbsks_xshock_app_likelihood* app_like_;

  // Grab generic cost function thing
  dbsks_xshock_wcm_likelihood* wcm_like_;

  // Geometric model of the fragments
  dbsks_xgraph_geom_model_sptr xgraph_geom_;

  //: Image cost model for the fragments
  dbsks_xgraph_ccm_model_sptr xgraph_ccm_;


  //: Solutions
  vcl_vector<dbsksp_xshock_graph_sptr > list_solutions_;
  vcl_vector<float > list_solution_costs_;
  vcl_vector<float > list_solution_real_costs_;


  // Graph-tree info
  unsigned root_vid_;
  unsigned major_child_eid_;

protected:
  // Working graph
  dbsksp_xshock_graph_sptr xgraph_;
};


#endif // seg/dbsks/dbsks_xshock_detector.h


