// This is shp/dbsksp/dbsksp_xshock_node.h
#ifndef dbsksp_xshock_node_h_
#define dbsksp_xshock_node_h_

//:
// \file
// \brief An extrinsic shock node
// \author Nhon Trinh ( ntrinh@lems.brown.edu)
// \date 2/10/2005
// \verbatim
//   Modifications:
//     Aug 2, 2008: added dbsksp_xshock_node class 
// \endverbatim


#include <vcl_map.h>
#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vbl/vbl_ref_count.h>
#include <vgl/vgl_point_2d.h>
#include <dbgrl/dbgrl_vertex.h>
#include <dbsksp/dbsksp_xshock_node_descriptor.h>


class dbsksp_xshock_edge;


//: Class for a node in an extrinsic shock graph
class dbsksp_xshock_node : public dbgrl_vertex<dbsksp_xshock_edge>
{
public:
  // CONSTRUCTORS / DESTRUCTORS / INITIALIZATION -------------------------------
  
  //: constructor
  dbsksp_xshock_node(unsigned int id): id_(id){};

  //: destructor
  virtual ~dbsksp_xshock_node(){};

  // DATA ACCESS ---------------------------------------------------------------

  //: Get id - unique for each element in the graph
  unsigned int id() const {return this->id_; }

  //: Set id
  void set_id(unsigned int id){this->id_ = id; }

  //: A platform-indenpendent name of the class
  vcl_string is_a() const {return "dbsksp_xshock_node"; }

  //: Return descriptor associated with an edge
  dbsksp_xshock_node_descriptor* descriptor(const E_sptr& e);

  //: Return descriptor associated with an edge - const
  const dbsksp_xshock_node_descriptor* descriptor(const E_sptr& e) const;


  //: Return extrinsic coordinates of the node
  vgl_point_2d<double > pt() const;

  //: Set coordinates of the node
  void set_pt(const vgl_point_2d<double >& pt);

  //: Return and set radius at the node
  double radius() const;
  void set_radius(double radius);

protected:
  //: Return the list of node descriptors
  vcl_map<E_sptr, dbsksp_xshock_node_descriptor >& descriptor_map()
  { return this->descriptor_map_;}

  //: Return the list of node descriptors
  const vcl_map<E_sptr, dbsksp_xshock_node_descriptor >& descriptor_map() const
  { return this->descriptor_map_;}

public:
  
  //: remove an edge, and its descriptor from the edge list
  // Return false if the edge is not found in the list
  bool remove_shock_edge(const E_sptr& e);

  //: Replace an edge with a different edge. Return the descriptor of the new edge
  // If new_xe == 0 then this is equivalent to remove an edge
  dbsksp_xshock_node_descriptor* replace_shock_edge(const E_sptr& old_xe, const E_sptr& new_xe);

  //: remove all edges and their descriptors
  void remove_all_shock_edges();

  //: add an edge right BEFORE a reference edge (in the edge list)
  // if ref_edge = 0 then add to the end of list
  dbsksp_xshock_node_descriptor* insert_shock_edge(const E_sptr& new_edge, 
    const E_sptr& ref_edge); 

  //: get and set depth of the node
  int depth() const {return this->depth_; }
  void set_depth(int depth) {this->depth_ = depth; }

  //: Return the parent edge (connecting to the parent node)
  E_sptr parent_edge();

  // UTILITY -------------------------------------------------------------------

  //: Rotate the tangent of one branch. Update descriptors of other branches
  // accordingly
  bool rotate_tangent(const E_sptr& xe, double ds);

  //: Order list of edges according to shock tangent
  void order_edge_list_by_shock_tangent();

  //: print info of the dbskpoint to an output stream
  vcl_ostream& print(vcl_ostream & os);

  //////////////////////////////////////////////////////////////////////////////
  // These functions should be moved to the graph vertex class
  //////////////////////////////////////////////////////////////////////////////

  //: list of edges
  const vcl_list<E_sptr >& edge_list() const
  { return this->in_edges_; }

  vcl_list<E_sptr >& edge_list()
  { return this->in_edges_; }


  // begin iterator 
  edge_iterator edges_begin(){ return this->in_edges_begin();}
  edge_iterator edges_end(){ return this->in_edges_end(); }
  //////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////




  // MEMBER VARIABLES ----------------------------------------------------------
protected:
  //: unique id of `this' node in the whole graph
  unsigned int id_;

  //: coordinates of the node
  vgl_point_2d< double > pt_;

  //: radius at the node
  double radius_;

  // parameters associated with each edge it is associated with
  vcl_map<E_sptr, dbsksp_xshock_node_descriptor > descriptor_map_;

  //: depth // shock graph is a tree
  int depth_;
};

#endif // shp/dbsksp/dbsksp_xshock_node.h
