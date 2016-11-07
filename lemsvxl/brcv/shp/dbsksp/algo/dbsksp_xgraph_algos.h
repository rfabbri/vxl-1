// This is shp/dbsksp/algo/dbsksp_xgraph_algos.h
#ifndef dbsksp_xgraph_algos_h_
#define dbsksp_xgraph_algos_h_

//:
// \file
// \brief Place holder for various algorithms on an extrinsic graph
//        
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date Nov 10, 2009
//
// \verbatim
//  Modifications
// \endverbatim



#include <dbsksp/dbsksp_xshock_graph_sptr.h>
#include <dbsksp/dbsksp_xshock_node_sptr.h>
#include <dbsksp/dbsksp_xshock_edge_sptr.h>
#include <dbsksp/dbsksp_xshock_node_descriptor.h>
#include <dbsksp/dbsksp_shapelet_sptr.h>

//: A class to hold various xgraph algorithms
// All member functions are static
class dbsksp_xgraph_algos
{
public:
  //: Destructor
  ~dbsksp_xgraph_algos(){};

  //: Close all the gaps between branches by splitting the error equally between
  // its two adjacent branches, except when one of the branches is a terminal edge. 
  // In that case the terminal edge absorbs all the errors.
  static void close_angle_gaps_at_nodes(const dbsksp_xshock_graph_sptr& xgraph);


  //: Update descriptors around a degree-2 node, given the descriptor of an edge
  static void update_degree2_node(const dbsksp_xshock_node_sptr& xv, 
    const dbsksp_xshock_edge_sptr& xe_child, 
    const dbsksp_xshock_node_descriptor& xdesc);

  //: Update descriptors around a degree-3 node
  static void update_degree3_node(const dbsksp_xshock_node_sptr& xv, 
                               const dbsksp_xshock_edge_sptr& xe_parent,
                               const dbsksp_xshock_edge_sptr& xe_child1,
                               const dbsksp_xshock_edge_sptr& xe_child2,
                               const dbsksp_xshock_node_descriptor& xdesc_parent,
                               double phi_child1);

  //: Compute a list of shapelet to fit in a chain of shock edges
  static bool fit_shapelet_chain(const dbsksp_xshock_node_sptr& start_node,
    const vcl_vector<dbsksp_xshock_edge_sptr>& path,
    vcl_vector<dbsksp_shapelet_sptr >& list_shapelet);

  //: Compute extrinsic samples of a shock fragment
  // The samples are equally spaced along the chordal curve
  static void compute_xsamples(int num_intervals,
    const dbsksp_xshock_node_sptr& start_node,
    const vcl_vector<dbsksp_xshock_edge_sptr>& path,
    vcl_vector<dbsksp_xshock_node_descriptor >& list_xsample);


  //: Sample a shock path
  // sample_ds is the sampling rate along the chordal curve
  static void compute_xsamples(const dbsksp_xshock_node_sptr& start_node,
    const vcl_vector<dbsksp_xshock_edge_sptr>& path,
    double sample_ds,
    vcl_vector<dbsksp_xshock_node_descriptor >& list_xsample);

protected:

  //: Compute extrinsic samples of a shock fragment by first fitting shapelets to the path
  static void compute_xsamples_by_fitting_shapelets(int num_intervals,
    const dbsksp_xshock_node_sptr& start_node,
    const vcl_vector<dbsksp_xshock_edge_sptr>& path,
    vcl_vector<dbsksp_xshock_node_descriptor >& list_xsample);


  //: Compute extrinsic samples of a shock fragment by moving along the boundary
  // biarc and compute the symmetry points on the opposite boundary curve
  static void compute_xsamples_by_sampling_longer_bnd_arc(int num_intervals,
    const dbsksp_xshock_node_sptr& start_node,
    const vcl_vector<dbsksp_xshock_edge_sptr>& path,
    vcl_vector<dbsksp_xshock_node_descriptor >& list_xsample);



  //----------------------------------------------------------------------------
private:
  // Private constructor - No instance of this class should ever be created
  dbsksp_xgraph_algos(){};
};




#endif // shp/dbsksp/dbsksp_xgraph_algos.h









