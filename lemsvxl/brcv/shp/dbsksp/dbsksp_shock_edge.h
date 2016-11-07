// This is shp/dbsksp/dbsksp_shock_edge.h
#ifndef dbsksp_shock_edge_h_
#define dbsksp_shock_edge_h_

//:
// \file
// \brief A class for shock edge
//        
// \author Nhon Trinh ( ntrinh@lems.brown.edu)
// \date Sep 28, 2006
//
// \verbatim
//  Modifications
// \endverbatim

#include <dbgrl/dbgrl_edge.h>
#include <dbsksp/dbsksp_shock_node_sptr.h>
#include <dbsksp/dbsksp_shock_fragment_sptr.h>
#include <vgl/vgl_vector_2d.h>
#include <vcl_string.h>

// forward declaration
class dbsksp_shock_node;

//: A class to represent a shock branch - both geometry and dynamics
class dbsksp_shock_edge : public dbgrl_edge<dbsksp_shock_node> 
{
public:
  // CONSTRUCTORS/DESTRUCTORS/INITIALIZATION -----------------------------------
  //: constructor
  dbsksp_shock_edge() : dbgrl_edge<dbsksp_shock_node>(), i_visited_(0), 
    fragment_(0), id_(0){};

  //: constructor
  dbsksp_shock_edge(const dbsksp_shock_node_sptr& source, 
    const dbsksp_shock_node_sptr& target, unsigned int id=0);

  //: destructor
  virtual ~dbsksp_shock_edge(){};

  
  // DATA ACCESS ---------------------------------------------------------------
  unsigned int id() const {return this->id_; }
  void set_id(unsigned int id){ this->id_ = id; }

  //: Return name of the class
  vcl_string is_a() const{ return "dbsksp_shock_edge"; }

  
  //: Return and set the parameter m
  double param_m(const dbsksp_shock_node_sptr& starting_node = 0) const;
  
  void set_param_m(double m, const dbsksp_shock_node_sptr& starting_node = 0); 

  //: Return and set length of the edge
  double chord_length() const {return this->chord_length_; }
  void set_chord_length(double length){ this->chord_length_ = length; }

  //: Return and set delta_r
  double radius_increment() const {return delta_r_; }
  void set_radius_increment(double delta_r) {this->delta_r_ = delta_r; }
  

  //: Return direction of the shock chord
  // Default is starting from source of the edge
  vgl_vector_2d<double > chord_dir(const dbsksp_shock_node_sptr& starting_node = 0) const;

  //: Set direction of the shock chord
  // Default is starting from source of the edge
  void set_chord_dir(const vgl_vector_2d<double >& direction, 
    const dbsksp_shock_node_sptr& starting_node = 0 );


  //: Return and set shock fragment
  dbsksp_shock_fragment_sptr fragment() const { return this->fragment_; }
  void set_fragment(const dbsksp_shock_fragment_sptr& fragment)
  { this->fragment_ = fragment; }

  //: Return true if `this' is a terminal edge (A_^1-A_\infty)
  bool is_terminal_edge() const;

  // GRAPH FUNCTIONS -----------------------------------------------------------
  // These should be moved to graph library

  //: Check whether a node is one of the two end nodes of ``this" edge
  bool is_vertex(const dbsksp_shock_node_sptr& node)
  { return (node == this->source()) || (node == this->target()); }

  //: Determine if ``this'' edge shares a vertex with other edge
  // Return the shared node if so, otherwise return 0
  dbsksp_shock_node_sptr shared_vertex(const dbsksp_shock_edge& other) const;

  // ROOTED TREE functions ----------------------------------------------------

  //: Return the ``child'' wrt of this edge
  dbsksp_shock_node_sptr child_node() const;

  //: Return ``parent'' node wrt this edge
  dbsksp_shock_node_sptr parent_node() const;
  

  // TRAVERSAL -----------------------------------------------------------------
  //: if i_visited_ less than i_traverse_flag, it's not visited
  bool is_visited (unsigned int traverse_value) const 
  {
    return (this->i_visited_ >= traverse_value);
  }
  
  void set_i_visited (unsigned int traverse_value) 
  {
    this->i_visited_ = traverse_value;
  }
  

  // UTILITY FUNCTIONS ---------------------------------------------------------

  //: Set values of delta_r from info of the source and target nodes
  void compute_radius_increment_from_nodes();

  //: Form a fragment for this edge
  void form_fragment();
  




  // MISCELLANEOUS -------------------------------------------------------------

  //: write info of the dbskbranch to an output stream
  virtual void print(vcl_ostream & os);

  // MEMBER VARIABLES ----------------------------------------------------------
protected:

  //: id of the edge, unique for each element of the graph
  unsigned int id_;

  //: visited flag for graph traversal
  unsigned int i_visited_;

  //: ratio between sine's of tangent angle and shock angle, w.r.t edge line
  double param_m_;  

  //: length of the line segment connecting two nodes
  double chord_length_;

  //: Radius (time) difference between target node and source node
  // delta_r = r(target) - r(source)
  double delta_r_;

  //: direction of the edge (unit vector), starting from source node to target node
  vgl_vector_2d<double > chord_dir_;

  //: shock fragment associated with this edge
  dbsksp_shock_fragment_sptr fragment_;

};
#endif // shp/dbsksp/dbsksp_shock_edge.h


