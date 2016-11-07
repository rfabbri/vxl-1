// This is seg/dbsks/dbsks_local_match.h
#ifndef dbsks_local_match_h_
#define dbsks_local_match_h_

//:
// \file
// \brief Match shock graph to image by varying locally
//        
// \author Nhon Trinh ( ntrinh@lems.brown.edu)
// \date Feb 11, 2008
//
// \verbatim
//  Modifications
// \endverbatim


#include <vbl/vbl_ref_count.h>
#include <vcl_map.h>
#include <vnl/vnl_vector.h>
#include <dbsksp/dbsksp_shock_graph_sptr.h>
#include <dbsksp/dbsksp_shock_edge_sptr.h>
#include <dbsksp/dbsksp_shapelet_sptr.h>
#include <dbsks/dbsks_ocm_image_cost_sptr.h>
#include <dbsks/dbsks_local_match_cost.h>
#include <vsol/vsol_box_2d_sptr.h>



// ============================================================================
class dbsks_local_match : public vbl_ref_count
{
public:
  // Constructor / destructor -------------------------------------------------
  //: constructor
  dbsks_local_match(){};

  //: destructor
  virtual ~dbsks_local_match(){};

  // User-interface functions -------------------------------------------------

  // Access memeber variables -------------------------------------------------

  //// Local cost match function
  //dbsks_local_match_cost* lm_cost() { return &this->lm_cost_; }


  //: Instance shock graph - the one we're computing energy of
  dbsksp_shock_graph_sptr graph() const {return this->graph_; };
  void set_graph(const dbsksp_shock_graph_sptr& graph);
  
  //: Set cost function parameters
  void set_cost_params(const dbsks_ocm_image_cost_sptr& ocm, float ds, float lambda);

  //: Set initial states of xnodes using the shapelet results from DP engine
  void set_xnode_states(const vcl_map<dbsksp_shock_edge_sptr, dbsksp_shapelet_sptr >& shapelet_map);

  //: Get object boundary from the cur_xnode_map
  vcl_vector<dbgl_circ_arc > get_cur_bnd_arc_list();

  //: Set initial states (fragments) of the edges
  void set_init_states_of_edges(
    const vcl_map<dbsksp_shock_edge_sptr, dbsksp_shapelet_sptr >& shapelet_map);

  //: Set ref_shapelet_map
  void set_ref_shapelet_map(
    const vcl_map<dbsksp_shock_edge_sptr, dbsksp_shapelet_sptr >& ref_shapelet_map);


  // Optimization functions----------------------------------------------------

  ////: Optimize energy of shock graph
  //void optimize();

  //: Optimize one node, keeping other node constant
  void optimize(const dbsksp_shock_node_sptr& v);

  //: Optimize the nodes in sequences
  void optimize_nodes_in_sequence(int num_node_visits);

  const vcl_map<dbsksp_shock_node_sptr, dbsksp_xshock_node_descriptor >& get_cur_xnode_map()
    const {return this->cur_xnode_map_; }

  //: graph cost, using shock-edit distance cost as regularization
  double f_graph_w_shock_edit_shape_cost(const vcl_map<dbsksp_shock_node_sptr, dbsksp_xshock_node_descriptor >& xnode_map,
    double& image_cost,
    double& shape_cost);


  //: graph cost, using shock-edit distance cost as regularization
  double f_graph_w_shock_edit_shape_cost(const vcl_map<dbsksp_shock_node_sptr, 
    dbsksp_xshock_node_descriptor >& xnode_map,
    double& image_cost,
    double& shape_cost,
    vcl_map<dbsksp_shock_edge_sptr, double >& image_cost_map,
    vcl_map<dbsksp_shock_edge_sptr, double >& shape_cost_map);

  


  //: graph cost, using shock-edit distance cost as regularization
  double f_graph_w_bnd_length_shape_cost(const vcl_map<dbsksp_shock_node_sptr, dbsksp_xshock_node_descriptor >& xnode_map);



  ////: Return the final twoshapelets associated with the edges
  //void get_final_twoshapelets(
  //  vcl_map<dbsksp_shock_edge_sptr, dbsksp_twoshapelet_sptr >& twoshapelet_map);
  
  ////: Return the final xnode associated with the vertices
  //void get_final_xnodes(
  //  vcl_map<dbsksp_shock_node_sptr, dbsksp_xshock_node_descriptor >& xnode_map);

  ////: Return the final list boundary arcs
  //void get_bnd_arc_list(vcl_vector<dbgl_circ_arc >& arc_list);


  // UTILITY functions ---------------------------------------------------------

  //: Print Optimization results to a stream
  void print_summary(vcl_ostream& str) const;

  // Global & final results ---------------------------------------------------
  double init_graph_cost_;
  double init_graph_image_cost_;
  double init_graph_shape_cost_;
  vcl_map<dbsksp_shock_edge_sptr, double > init_image_cost_map_;
  vcl_map<dbsksp_shock_edge_sptr, double > init_shape_cost_map_;


  double final_graph_cost_;
  double final_graph_image_cost_;
  double final_graph_shape_cost_;
  vcl_map<dbsksp_shock_edge_sptr, double > final_image_cost_map_;
  vcl_map<dbsksp_shock_edge_sptr, double > final_shape_cost_map_;

protected:

  // the graph
  dbsksp_shock_graph_sptr graph_;

  // Oriented chamfer matching cost
  dbsks_ocm_image_cost_sptr ocm_;

  //: Sampling rate when computing arc costs
  float ds_;

  //: ratio between image cost and intrinsic cost
  float lambda_;

  ////: Cost function to optimize
  //dbsks_local_match_cost lm_cost_;



  // initial states of the graph's edges
  vcl_map<dbsksp_shock_node_sptr, dbsksp_xshock_node_descriptor > cur_xnode_map_;

  //////////////////////////////////////////////////////////////////////////////
  // initial states of the graph's edges
  vcl_map<dbsksp_shock_edge_sptr, dbsksp_shapelet_sptr > shapelet_map_;

  // shapelets corresponding to the model graph
  vcl_map<dbsksp_shock_edge_sptr, dbsksp_shapelet_sptr > ref_shapelet_map_;

  //// final configuration
  //vnl_vector<double > final_x_;
};


#endif // seg/dbsks/dbsks_local_match.h


