// This is seg/dbsks/dbsks_xshock_dp_new.h
#ifndef dbsks_xshock_dp_h_new_
#define dbsks_xshock_dp_h_new_

//:
// \file
// \brief Dynamic Programming on extrinsic shock graph for detection
//        
// \author Yuliang Guo (yuliang_guo@brown.edu)
// \date Oct 22, 2014
//
// \verbatim
//  Modifications
// \endverbatim




#include <dbsks/dbsks_xnode_grid.h>
#include <dbsks/dbsks_xfrag_geom_model_sptr.h>
#include <dbsks/dbsks_xgraph_geom_model_sptr.h>
#include <dbsksp/dbsksp_xshock_graph_sptr.h>
#include <dbsksp/dbsksp_xshock_node_sptr.h>
#include <dbsksp/dbsksp_xshock_edge_sptr.h>
#include <dbsksp/dbsksp_xshock_fragment.h>


#include <vcl_map.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>
#include <vbl/vbl_array_1d.h>
#include <vbl/vbl_array_2d.h>
#include <vbl/vbl_ref_count.h>

// ============================================================================
// dbsks_xshock_dp_new
// ============================================================================

class dbsks_xshock_likelihood;


//: 
class dbsks_xshock_dp_new : public vbl_ref_count
{
public:
  //typedef vbl_array_1d<vnl_matrix<float > > grid_float;
  typedef vbl_array_2d<vnl_vector<float > > grid2d_float;
  typedef vbl_array_2d<vnl_vector<int > > grid2d_int;

  // Constructor / destructor -------------------------------------------------
  
  //: constructor
  dbsks_xshock_dp_new(){};

  //: destructor
  virtual ~dbsks_xshock_dp_new(){};
  
  // Access memeber variables -------------------------------------------------

  //: Return the working graph
  dbsksp_xshock_graph_sptr graph() const { return this->graph_; }

  //: Return size of the working graph
  double graph_size() const {return this->graph_size_; }

  //: Set the working graph (and compute its size)
  void set_graph(const dbsksp_xshock_graph_sptr& graph);

//: Set the working graph (and compute its size)
  void set_prev_graph(const dbsksp_xshock_graph_sptr& prev_graph);

  //: Return likelihood calculator
  dbsks_xshock_likelihood* xshock_likelihood() const 
  {
    return this->xshock_likelihood_; 
  }

  //: Set the likelihood calculator
  void set_xshock_likelihood(dbsks_xshock_likelihood* xshock_likelihood)
  {
    this->xshock_likelihood_ = xshock_likelihood;
  }

  //: Set minimum acceptable confidence
  void set_min_acceptable_confidence(float min_confidence)
  {
    this->min_acceptable_xgraph_loglike_ = min_confidence;
  }


  // Utility -------------------------------------------------------------------

  //: Initialize all the cost grids
  bool init_cost_grids();

  //: Compute node visisting schedule
  void compute_node_visit_schedule(const dbsksp_xshock_graph_sptr& xgraph,
    unsigned root_vid,
    vcl_vector<dbsksp_xshock_node_sptr >& node_visit_schedule);
  

  //: Run DP on the given graph
  bool optimize();

protected:

  //: Compute maximum acceptable cost during DP, given the min acceptable level
  // and the dynamic of the likelihood function
  void compute_max_acceptable_xgraph_cost();

  // Optimization routines -----------------------------------------------------

  //: Compute node visit schedule for a subtree
  void compute_subtree_node_visit_schedule_recursive(
    const dbsksp_xshock_node_sptr& subtree_root,
    vcl_vector<dbsksp_xshock_node_sptr >& node_visit_schedule);

  //: Optimize a degree-2 node
  void optimize_degree_2_node(const dbsksp_xshock_node_sptr& xv, 
    const dbsksp_xshock_edge_sptr& xe);

  //: Optimize a degree-3 node
  void optimize_degree_3_node(const dbsksp_xshock_node_sptr xv,
    const dbsksp_xshock_edge_sptr& parent_edge,
    const dbsksp_xshock_edge_sptr& child_edge1,
    const dbsksp_xshock_edge_sptr& child_edge2);


  //: Optimize child node of an edge given a state of parent node
  void optimize_child_node_given_parent_node(const dbsksp_xshock_edge_sptr& xe,
    const dbsks_xnode_grid& grid_p, //> grid containing states of parent node
    float max_acceptable_subtree_cost, //> upper bound for cost
    grid2d_float& opt_cost_p, //> min cost of parent node, given its state
    grid2d_int& opt_child);  //> state of child node, given parent node's state

  //: Optimize child node of an edge given a state of parent node
  void optimize_child_node_given_parent_node2(const dbsksp_xshock_edge_sptr& xe,
    const dbsks_xnode_grid& grid_p, //> grid containing states of parent node
    float max_acceptable_subtree_cost, //> upper bound for cost
    grid2d_float& opt_cost_p, //> min cost of parent node, given its state
    grid2d_int& opt_child);  //> state of child node, given parent node's state

  //: Optimize the root node (should be a degree-2 node)
  // "root_vid" is id of the root node, should have degree-2
  // "major_child_eid" is id of the main edge (bigger branch)
  bool optimize_root_node(unsigned root_vid, unsigned major_child_eid);


  //: Optimize the root node (should be a degree-2 node), considering the cost 
  // of both branches
  // "root_vid" is id of the root node, should have degree-2
  // "major_child_eid" is id of the main edge (bigger branch)
  bool optimize_degree2_root_node_use_both_branches(unsigned root_vid, unsigned major_child_eid);

  bool optimize_degree2_root_node_use_both_branches2(unsigned root_vid, unsigned major_child_eid);
  //: Optimize the degree-3 root node using all three branches
  // "root_vid" is id of the root node, should have degree-3
  // "major_child_eid" is id of the first child
  // "pseudo_parent" edge  = predecessor of major_child
  // "minor_child" edge = successor of major_child
  bool optimize_degree3_root_node_use_all_branches(unsigned root_vid, unsigned major_child_eid);
  
  //: Find the optimal state of the child given a specific state of the parent
  bool find_optimal_child_node_given_parent_state(unsigned edge_id, 
    const dbsksp_xshock_node_descriptor& xd_p,
    int degree_child_node,
    const dbsks_xfrag_geom_model_sptr& geom_model,
      int num_samples_c,
      const dbsks_xnode_grid& grid_c,
      const grid2d_float& opt_cost_c,
      float& min_branch_cost,
      int& child_state_at_min_branch_cost,
      float max_acceptable_cost); // = 3e38);

  bool update_parent_node_cost_given_child_state(unsigned edge_id,
                                            const dbsksp_xshock_node_descriptor& xd_c,
                                            int degree_child_node,
                                            const dbsks_xfrag_geom_model_sptr& geom_model,
                                            int num_samples_c,
                                            const dbsks_xnode_grid& grid_p,
                                            float subtree_cost,
											int ic_idx,
                                            float max_acceptable_cost,
                                            grid2d_float& opt_cost_p,
											grid2d_int& opt_child);

  // Cost functions-------------------------------------------------------------

  //: Compute cost of a fragment
  float fragment_cost(unsigned edge_id, const dbsksp_xshock_fragment& xfrag);


  // Memory allocation ---------------------------------------------------------
  
  //: Allocate memory for a grid2d (templated) and fill it with a default value
  template <class T>
  bool allocate_grid2d(vbl_array_2d<vnl_vector<T> >& grid2d, 
    int num_x, int num_y, int size_per_cell, T val)
  {
    grid2d.resize(num_x, num_y);
    for (typename vbl_array_2d<vnl_vector<T > >::iterator it = grid2d.begin(); 
      it != grid2d.end(); ++it)
    {
      it->set_size(size_per_cell);
      it->fill(val);
    }
    return true;
  }


public:  
  // Optimization at root node -----------------------------------------------------------------

  //: compute globally optimal graph
  bool find_xgraph_state_global_optimum(unsigned root_vid, 
    vcl_map<unsigned, int >& xgraph_state, float& opt_cost);

  //: Compute locally optimal xgraphs
  bool find_xgraph_state_local_optimum(unsigned root_vid, 
    vcl_vector<vcl_map<unsigned, int> >& list_xgraph_state,
    vcl_vector<float >& list_cost);

  //: Trace the state of the root node that correspond to the global optimum
  bool compute_root_state_global_optimum_(unsigned root_vid,
    int& opt_root_state, float& opt_cost);

  //: Trace the states of the root node that correspond to optima with a spatial cell grid
  bool compute_root_state_local_optimum_(unsigned root_vid, 
    vcl_vector<int >& opt_state, vcl_vector<float >& opt_cost);

  //: Find the root node's states that are local optimum wrt to all params of the root node
  bool compute_root_state_of_local_min_all_root_params(vcl_vector<int >& list_root_state, 
    vcl_vector<float >& list_graph_cost,
    int kernel_radius = 1);


  // Trace solution ------------------------------------------------------------------

  //: Trace the graph configuration given specific state of the root node
  bool trace_solution_from_root_state(int root_state, vcl_map<unsigned, int >& sol_node2state);

protected:
  //: Recursively trace out the optimal states of the nodes in a branch given 
  // the state of the branch's root node (this must not be the graph's root)
  // xv_root is the root node of the branch
  // xe_parent is the parent edge of the root node
  // root_state is the state of the root node
  // map_node2state is where optimal nodes of this branch will be APPENDED to
  bool trace_optimal_child_state_recursive(const dbsksp_xshock_node_sptr& xv_root,
    const dbsksp_xshock_edge_sptr& xe_parent, int root_state,
    vcl_map<unsigned, int >& map_node2state);

  // Low-level optimization ----------------------------------------------------
  
  //: Compute min value in a window of a grid2d_float function
  bool compute_kernel_min(const grid2d_float& F,
    const dbsks_xnode_grid& grid,
    int min_i_x, int max_i_x,
    int min_i_y, int max_i_y,
    int min_i_psi, int max_i_psi,
    int min_i_phi0, int max_i_phi0,
    int min_i_r, int max_i_r,
    float& min_val) const;

  //: Compute min value of a grid2d_float function
  float compute_min_value(const grid2d_float& F) const;

  //: Compute min value for position of a grid2d_float function
  void compute_min_value_matrix(const grid2d_float& F, vnl_matrix<float >& min_value) const;

  // Utilities------------------------------------------------------------------

  //: Compute maximum acceptable cost for a sub-tree based on the min-costs
  // of other sub-trees that have been computed.
  // except_vid1, except_vid2, and except_vid3 are id's of the nodes whose subtrees
  // we want to ignore during the calculation
  // Typically, for an A12 subtree, we only have except_vid0. For an A13 node,
  // we have "except_vid0" and "except_vid1". "except_vid2" is there for any future use
  float compute_max_acceptable_subtree_cost(unsigned except_vid0, 
    unsigned except_vid1 = 0,
    unsigned except_vid2 = 0);

  ////: Propagate min-cost from children to parent to determine the lower bound cost
  //// of a parent node before the optimization
  //void propagate_min_cost_from_children_to_parent_node(vnl_matrix<float >& min_cost,
  //    unsigned parent_id);


public:

  // I/O functions -------------------------------------------------------------

  //: Print Optimization results to a stream
  vcl_ostream& print_summary(vcl_ostream& str) const;


  // DP data structure ---------------------------------------------------------
// protected:
  unsigned int root_vid_;
  unsigned int major_child_eid_;
  
  //: a grid associated with each node
  vcl_map<unsigned int, dbsks_xnode_grid >* map_xnode_grid;

  //: a geometric model for the graph
  dbsks_xgraph_geom_model_sptr xgraph_geom;

  
  //: A mapping from the id of a node (key, unsigned int) to a grid (grid2d_float)
  // containing the optimal cost for each state of that node
  vcl_map<unsigned int, grid2d_float > map_opt_cost; // for both degree-2 and degree-3 node

  //: A mapping from the id of a degree-2, non-root node (key, unsigned int) to a
  // grid (grid2d_int) containing its (only) child's state associated with the optimal cost
  // at that degree-2 node.
  vcl_map<unsigned int, grid2d_int > map_opt_child; // only for degree-2 node

  //: A mapping from the id of a degree-3, non-root node (key, unsigned int) to a
  // grid (grid2d_int) containing the states of its 1st child (map_opt_child1) and
  // 2nd child (map_opt_child2) associated with the optimal cost at that degree-3 node
  vcl_map<unsigned int, grid2d_int > map_opt_child1; // only for degree-3 node
  vcl_map<unsigned int, grid2d_int > map_opt_child2; // only for degree-3 node

  //: A mapping from the id of a degree-3 node (key, unsigned int) to a
  // grid (grid2d_int) containing indices of the angle phi1 that correspond to the
  // optimal cost at that degree-3 node
  vcl_map<unsigned int, grid2d_int > map_opt_phi1; // only for degree-3 node
  
  //: This mapping is for the root node. It maps from the ids of the root's children
  // to the optimal states of those children that correspond to the optimal cost
  // at the root. The "key" of this map has different meaning from the other maps in this class.
  // It is the id of the CHILD, NOT the parent (as in the other maps). The id of
  // the root node is unique, saved in root_vid;
  vcl_map<unsigned int, grid2d_int > map_opt_child_of_root;

  //: a map of minimum costs corresponding to nodes that have been processed but 
  // whose parent nodes have not been processed
  vcl_map<unsigned int, float > map_node_to_min_cost_;

  ////: a map to look up minimum sub-tree cost for each position of the selected node
  //// key: id of node
  //// value: an matrix of sub-tree costs for each position node
  //vcl_map<unsigned int, vnl_matrix<float > > map_node_to_min_cost_matrix_;

  //: a list of solutions
  vcl_vector<vcl_map<unsigned int, int> > list_opt_xgraph_state;
  vcl_vector<float > list_opt_cost;

public:
  //: Likelihood calculator for each fragment
  dbsks_xshock_likelihood* xshock_likelihood_;

  //: Minimum accepted likelihood for a shock graph
  float min_acceptable_xgraph_loglike_;

  
protected:

  //: Maximum acceptable cost during DP
  float max_acceptable_xgraph_cost_;

  // Working graph
  dbsksp_xshock_graph_sptr graph_;

  // Prev detected graph
  dbsksp_xshock_graph_sptr prev_graph_;

  // cache the size of the working raph
  double graph_size_;

  double prev_graph_size_;

  // Internal parameters
  int num_samples_c_; // degree-2 node: number of samples for child
};


#endif // seg/dbsks/dbsks_xshock_dp_new.h


