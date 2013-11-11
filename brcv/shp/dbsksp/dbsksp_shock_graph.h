// This is shp/dbsksp/dbsksp_shock_graph.h
#ifndef dbsksp_shock_graph_h_
#define dbsksp_shock_graph_h_

//:
// \file
// \brief A class for a shock graph
//        
// \author Nhon Trinh ( ntrinh@lems.brown.edu)
// \date Sep 28, 2006
//
// \verbatim
//  Modifications
// \endverbatim

#include <dbgrl/dbgrl_graph.h>

#include <dbsksp/dbsksp_shock_node.h>
#include <dbsksp/dbsksp_shock_node_descriptor.h>
#include <dbsksp/dbsksp_shock_edge.h>
#include <vsol/vsol_polyline_2d_sptr.h>
#include <vsol/vsol_spatial_object_2d_sptr.h>
#include <vsol/vsol_box_2d_sptr.h>

//: 
class dbsksp_shock_graph : public dbgrl_graph<dbsksp_shock_node, dbsksp_shock_edge>
{
public:

  // --------------------------------------------------------------------------
  // Constructors / Destructors
  // --------------------------------------------------------------------------
  //: Constructor
  dbsksp_shock_graph();

  //: Copy constructor
  // Need to fix this constructor so that it also works when the argument is
  // "const dbsksp_shock_graph&"
  dbsksp_shock_graph(dbsksp_shock_graph& that);
  
  //: Destructor
  virtual ~dbsksp_shock_graph(){};

  //: Initialization
  bool init(vsol_polyline_2d_sptr polyline);

  //: Init the graph with three branches meeting at an A_1^3 node
  void init_simple_a13_graph();

  

  // --------------------------------------------------------------------------
  // DATA ACCESS
  // --------------------------------------------------------------------------

  //: Return the type identifier string
  vcl_string is_a() const {return "dbsksp_shock_graph"; }

  //: unique id given to its element
  unsigned int next_available_id(){ return ++this->next_available_id_; }

  //: set the next_available_id variable
  void set_next_available_id(unsigned int new_next_id)
  { this->next_available_id_ = new_next_id; }

  //: Return the edge with the given id
  dbsksp_shock_edge_sptr edge_from_id(unsigned id);

  //: Return the node with the given id
  dbsksp_shock_node_sptr node_from_id(unsigned id);
  
  
  //: Return reference node
  dbsksp_shock_node_sptr ref_node() const { return this->ref_node_; }
  
  //: Set reference node
  void set_ref_node(const dbsksp_shock_node_sptr& ref_node);

  //: Return reference edge (incident to reference node)
  dbsksp_shock_edge_sptr ref_edge() const { return this->ref_edge_; }
  
  //: Set reference edge
  bool set_ref_edge( const dbsksp_shock_edge_sptr& ref_edge);

  //: Return coordinates of reference node
  vgl_point_2d<double > ref_origin() const { return this->ref_origin_; }
  
  //: Set the coordinates of reference node
  void set_ref_origin(const vgl_point_2d<double >& ref_origin)
  { this->ref_origin_ = ref_origin; }

  //: Return the direction of reference edge
  vgl_vector_2d<double > ref_direction() const {return this->ref_direction_; }
  
  //: Set the direction of the reference edge
  void set_ref_direction(const vgl_vector_2d<double >& ref_direction )
  { this->ref_direction_ = normalized(ref_direction); }

  //: Return and set the radius of ref_node
  double ref_node_radius() const {return this->ref_node_radius_; }
  void set_ref_node_radius(double radius){ this->ref_node_radius_ = radius; }

  //: Set all the reference parameters from an edge and a node
  void set_all_ref(const dbsksp_shock_node_sptr& ref_node,
    const dbsksp_shock_edge_sptr& ref_edge);


  // --------------------------------------------------------------------------
  // SHOCK GRAPH PROPERTIES
  // --------------------------------------------------------------------------
  
  //: Approximate the bounding box of the graph
  // Assume all extrinsic properties have been computed.
  // Run compute_all_dependent_params() if not sure.
  vsol_box_2d_sptr compute_approx_bounding_box();

  //: Enlarge the graph by a given scale
  // scale > 1, the resulting shapelet is bigger
  // scale < 1, the resulting shapelet is smaller
  void scale_up(double scale);


  
  // --------------------------------------------------------------------------
  // BASIC GRAPH FUNCTIONS
  // --------------------------------------------------------------------------

  //: Remove a shock edge from the graph
  bool remove_shock_edge(const E_sptr& e);


  


  // --------------------------------------------------------------------------
  // HIGH-LEVEL GRAPH FUNCTIONS
  // --------------------------------------------------------------------------

  //: Insert a node in the middle of an edge without modifying the boundary 
  // geometric position of the new node is indicated by t, t \in [0,1]
  // with t = 0: source node
  // t = 1: target node
  dbsksp_shock_node_sptr insert_shock_node(const dbsksp_shock_edge_sptr& e,
    double t);

  //: Insert a shock edge to the end of a given edge
  // The new edge preserves the angle phi at the node
  dbsksp_shock_edge_sptr insert_shock_edge(dbsksp_shock_edge_sptr& e,
    double chord_length,
    bool at_source = true,
    double param_m = 0
    );

  //: Add an A_infty branch to an existing A_1^2 node and convert it to A_1^3
  // the new edge is inserted right BEFORE the reference edge ``e''
  dbsksp_shock_edge_sptr insert_A_infty_branch(const dbsksp_shock_node_sptr& node,
    const dbsksp_shock_edge_sptr& ref_edge);

  //: Remove a terminal edge, A_infty, (with very short boundary) from a node with 
  // degree n >= 3
  // Use when the angle phi of the branch is very small, the boundary is simply a point
  // Hence the edge is degenerate
  // Return the opposite node to the terminal (degree-one) node
  dbsksp_shock_node_sptr remove_A_infty_edge(const dbsksp_shock_node_sptr&
    degree_one_node);

  
  //: Squeeze a (short) shock edge to a point and convert it to two terminal edges
  // Return the node that the edge is converted to
  dbsksp_shock_node_sptr squeeze_shock_edge(const dbsksp_shock_edge_sptr& e,
    dbsksp_shock_edge_sptr& new_terminal_edge_left,
    dbsksp_shock_edge_sptr& new_terminal_edge_right);

  //: Squeeze a (short) shock edge, but no interest in the new terminal edges created
  dbsksp_shock_node_sptr squeeze_shock_edge(const dbsksp_shock_edge_sptr& e);


  //: Remove a leaf A_1^2 edge, the A_1^2 edge connected to only terminal edges
  dbsksp_shock_node_sptr remove_leaf_A_1_2_edge(const dbsksp_shock_edge_sptr& leaf_edge);

  //: Remove internal edge
  // The phi's at two sides will be averaged and distributed evenly
  dbsksp_shock_node_sptr remove_internal_edge(const dbsksp_shock_edge_sptr& edge);

  //: Split an (degree > 3) node into two nodes and insert an edge between them
  // Typical used in A_1^4 transitions
  // The edges incident to the node will be divided into two groups, each
  // stays with one node. The user gives and start and end edge of one group,
  // inclusively.
  // Return a pointer to the new edge
  dbsksp_shock_edge_sptr split_shock_node(const dbsksp_shock_node_sptr& node,
    double chord_length,
    const dbsksp_shock_edge_sptr& start_edge,
    const dbsksp_shock_edge_sptr& end_edge);


  


  // -------------------- LEGALITY CHECK --------------------
  
  //: Check the legality (generating a simple smooth contour) of the shock graph
  bool is_legal();



  // -------------------- Graph Traverse Functions --------------------
  unsigned int i_traverse_flag() const 
  {
    return i_traverse_flag_;
  }
  
  //: increase the search flag, so that every element has lower value than the
  // flag(unvisited) once visited, it's i_visited_ will be set to i_traverse_flag_ (visited)
  void init_traversal() 
  {
    this->i_traverse_flag_++;
  }


  // -------------- ROOTED TREE functions -------------------------------------

  dbsksp_shock_node_sptr root() const {return this->ref_node(); }
    //: Compute depths and parent-child relationship
  void compute_vertex_depths();
  
protected:

  //: Compute vertex depths for a subtree whose root is root_depth
  void compute_subtree_vertex_depths(const dbsksp_shock_node_sptr& root, int root_depth);

public:
  


  // -------------------- UTILITY --------------------

  //: Using the reference node and reference edge to compute coodinates of the 
  // nodes
  //void compute_graph_explicit_geometry();
  void compute_coordinates_for_all_nodes();

  //: Compute dependent parameters at A12 nodes
  void compute_dependent_params_at_A12_nodes();

  //: Compute dependent parameters at A13 nodes
  void compute_dependent_params_at_A13_nodes();

  //: Compute dependent parameters at A1n, n>3, nodes
  void compute_dependent_params_at_A1nGT3_nodes();

  //: Compute radius increment at all edges
  void compute_radius_increment_for_all_edges();

  //: Using the reference node and reference edge to compute radius at all nodes
  void compute_radius_for_all_nodes();

  //: Form visual fragments at all edges
  void form_fragment_for_all_edges();

  //: Compute all dependent params
  void compute_all_dependent_params();





  // ============= MISCELLANEOUS =============================================
  
  //: print info of the shock graph to an output stream
  void print(vcl_ostream & os);

  //: Trace out the boundary of the shock graph
  vcl_vector<vsol_spatial_object_2d_sptr > trace_boundary();


  
protected:
  
  //: Recursive function to propagate extrinsic geometry from a node and an edge
  // Requirement: edge is incident to node
  // Note: run this->init_traversal() before this function
  // and then set i_traverse_flag to this->i_traverse_flag();
  void propage_extrinsic_geometry_from(const dbsksp_shock_edge_sptr& ref_edge,
    const dbsksp_shock_node_sptr& ref_node,
    unsigned int i_traverse_flag);

  //: propagate radius from a node
  void propage_radius_from(const dbsksp_shock_node_sptr& ref_node,
    unsigned int i_traverse_flag);

  //: insert a shock node to a terminal edge
  dbsksp_shock_node_sptr  insert_shock_node_at_terminal_edge(
    const dbsksp_shock_edge_sptr& e);


  // depreciated
  //: Default (simple) shock graph
  bool init_default();



  //: unique id given to its element
  unsigned int next_available_id_;


  //: Graph traversing counter
  unsigned int i_traverse_flag_; 

  //: reference node to compute explicit coordinate
  dbsksp_shock_node_sptr ref_node_;

  //: reference edge (incident to ref_node) to compute explicit direction
  // the direction is from the ref_node to the other end of the edge
  dbsksp_shock_edge_sptr ref_edge_;

  // extrinsic paramters
  vgl_point_2d<double > ref_origin_;
  vgl_vector_2d<double > ref_direction_;
  double ref_node_radius_;

private:

};

#endif // shp/dbsksp/dbsksp_shock_graph.h


