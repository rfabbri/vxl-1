// This is shp/dbsksp/dbsksp_shock_node.h
#ifndef dbsksp_shock_node_h_
#define dbsksp_shock_node_h_

//:
// \file
// \brief A class for a shock node
// \author Nhon Trinh ( ntrinh@lems.brown.edu)
// \date 2/10/2005
// \verbatim
//  Modifications
// \endverbatim

#include <vbl/vbl_ref_count.h>
#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_vector_2d.h>

#include <dbgrl/dbgrl_vertex.h>

#include <dbsksp/dbsksp_shock_node_sptr.h>
#include <dbsksp/dbsksp_shock_edge_sptr.h>
#include <dbsksp/dbsksp_shock_node_descriptor_sptr.h>


class dbsksp_shock_edge;

//: Class for a shock node, meant to be on a planar graph
class dbsksp_shock_node : public dbgrl_vertex<dbsksp_shock_edge>
{
public:
  // =============== CONSTRUCTORS / DESTRUCTORS / INITIALIZATION ==============
  //: constructor
  dbsksp_shock_node(unsigned int id):i_visited_(0), id_(id){};

  //: destructor
  virtual ~dbsksp_shock_node(){};

  // ============== DATA ACCESS ===============================
  unsigned int id() const {return this->id_; }
  void set_id(unsigned int id){this->id_ = id; }


  //: A platform-indenpendent name of the class
  vcl_string is_a() const {return "dbsksp_shock_node"; }

  //: Return descriptor associated with an edge
  dbsksp_shock_node_descriptor_sptr descriptor(const dbsksp_shock_edge_sptr& e);

  //: Return extrinsic coordinates of the node
  vgl_point_2d<double > pt() const {return this->pt_; }

  //: Set coordinates of the node
  void set_pt(const vgl_point_2d<double >& pt)
  { this->pt_ = pt; }

  //: Return and set radius at the node
  double radius() const {return this->radius_; }
  void set_radius(double radius){ this->radius_ = radius; }
  
  //: Return the list of node descriptors
  const vcl_list<dbsksp_shock_node_descriptor_sptr >& descriptor_list()
  {return this->descriptor_list_;}


  // ------------------ Rooted tree functions --------------------
  // return and set depth of the vertex
  int depth() const {return this->depth_; }
  void set_depth(int depth) {this->depth_ = depth; }

  //: Return and set the parent edge
  E_sptr parent_edge() const {return this->parent_edge_; }
  void set_parent_edge(const E_sptr& e) {this->parent_edge_ = e; }





  // =============== TRAVERSAL =====================================
  //: if i_visited_ less than i_traverse_flag, it's not visited
  bool is_visited (unsigned int traverse_value) const 
  {
    return (this->i_visited_ >= traverse_value);
  }
  
  void set_i_visited (unsigned int traverse_value) 
  {
    this->i_visited_ = traverse_value;
  }

  const vcl_list<dbsksp_shock_edge_sptr >& edge_list() const
  {return this->in_edges_; }
  edge_iterator edges_begin(){ return this->in_edges_begin();}
  edge_iterator edges_end(){ return this->in_edges_end(); }

  // =========== UTILITY ====================================

  //: print info of the dbskpoint to an output stream
  void print(vcl_ostream & os);

  // These functions should be moved to the graph vertex class
  
  //: add an edge to the end of the list
  void add_edge(const E_sptr& e);

  //: remove an edge, and its descriptor from the edge list
  // Return false if the edge is not found in the list
  bool remove_shock_edge(const E_sptr& e);

  //: remove all edges and their descriptors
  void remove_all_shock_edges();

  //: add an edge right BEFORE a reference edge (in the edge list)
  dbsksp_shock_node_descriptor_sptr insert_shock_edge(
    const E_sptr& new_edge, const E_sptr& ref_edge); 

  //: Check if this node is connected to a terminal edge
  bool connected_to_terminal_edge() const;

  //: Change the phi angle of one edge and uniformly distribute the difference 
  // it causes
  void change_phi_distribute_difference_uniformly(const dbsksp_shock_edge_sptr& e, 
    double new_phi);

  

  // ============ MEMBER VARIABLES =========================
protected:

  //: unique id of `this' node in the whole graph
  unsigned int id_;

  //: visited flag for graph traversal
  unsigned int i_visited_;

  // ------------ graph parameters ------------------------

  // parameters associated with each edge it is associated with
  vcl_list< dbsksp_shock_node_descriptor_sptr > descriptor_list_;
  
  //: coordinates of the node
  vgl_point_2d< double > pt_;

  //: radius at the node
  double radius_;

  // ------------ The following parameters are with respect to the root node
  // they are created for use in DP optimization
  //: 
  int depth_;
  E_sptr parent_edge_;
};



// ============================================================================
// dbsksp_shock_node_algos
// ============================================================================


//: Algorithms that operates on shock nodes
// list of static functions
class dbsksp_shock_node_algos
{
public:
  ~dbsksp_shock_node_algos(){}

  //: solve system of equations at A_1^2 node, with
  // parameters:
  // - m0
  // - m2
  // - theta
  // - shock flow direction: 1 : e0 --> e2, -1 : e2 --> e0
  // variables:
  // - alpha0
  // - alpha2
  // - phi0
  // - phi2
  // Check Nhon's notes for notations and details of the algorithm
  static bool solve_equations_at_A12_node(
    double* alpha0,
    double* alpha2,
    double* phi0,
    double* phi2,
    double m0,
    double m2,
    double theta,
    int shock_flow_direction);

  static bool compute_params_at_A12_node_given_2_m_and_1_phi(
    const dbsksp_shock_node_sptr& node);

  static bool compute_params_at_A13_node_given_3_m_and_2_phi(
    const dbsksp_shock_node_sptr& node);

  static bool compute_params_at_A1nGT3_node_given_n_m_and_n_minus_1_phi(
    const dbsksp_shock_node_sptr& node);



protected:
  dbsksp_shock_node_algos(){};

  // depreciated

  //: compute dependent parameters at a A_1^2 node,
  // assuming the following free variables
  // - edge parameter:                m0
  // - edge parameter:                m2
  // - rotation between two edges:    theta
  static bool compute_params_at_A12_node_given_2_m_and_rot_angle(
    dbsksp_shock_node_sptr& node);

};








#endif // shp/dbsksp/dbsksp_shock_node.h
