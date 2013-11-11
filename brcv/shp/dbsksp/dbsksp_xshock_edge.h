// This is shp/dbsksp/dbsksp_xshock_edge.h

#ifndef dbsksp_xshock_edge_h_
#define dbsksp_xshock_edge_h_

//:
// \file
// \brief A class for extrinsic shock edge
//        
// \author Nhon Trinh ( ntrinh@lems.brown.edu)
// \date Aug 9, 2008
//
// \verbatim
//  Modifications
// \endverbatim

#include <dbgrl/dbgrl_edge.h>

#include <dbsksp/dbsksp_xshock_fragment_sptr.h>

#include <vgl/vgl_vector_2d.h>
#include <vcl_string.h>

// forward declaration
class dbsksp_xshock_node;

//: A class to represent an extrinsic shock edge
class dbsksp_xshock_edge : public dbgrl_edge<dbsksp_xshock_node> 
{
public:
  // CONSTRUCTORS/DESTRUCTORS/INITIALIZATION -----------------------------------

  //: constructor
  dbsksp_xshock_edge() : dbgrl_edge<dbsksp_xshock_node>(), id_(0){};

  //: constructor with two nodes
  dbsksp_xshock_edge(const V_sptr& source, const V_sptr& target, unsigned id = 0);

  //: destructor
  virtual ~dbsksp_xshock_edge(){};
  
  // DATA ACCESS ---------------------------------------------------------------
  unsigned int id() const {return this->id_; }
  void set_id(unsigned int id){ this->id_ = id; }

  //: Return name of the class
  vcl_string is_a() const{ return "dbsksp_xshock_edge"; }
  
  //: Return true if `this' is a terminal edge (A_^1-A_\infty)
  bool is_terminal_edge() const;

  // GEOMETRY-------------------------------------------------------------------

  //: Return a fragment (influence zone) associated with this edge
  dbsksp_xshock_fragment_sptr fragment();


  // GRAPH FUNCTIONS -----------------------------------------------------------
  // These should be moved to graph library

  //: Check whether a node is one of the two end nodes of ``this" edge
  bool is_vertex(const V_sptr& node)
  { return (node == this->source()) || (node == this->target()); }


  // TREE FUNCTIONS -----------------------------------------------------------

  //: Return parent node
  V_sptr parent_node() const;

  //: Return child node
  V_sptr child_node() const;


  // UTILITY FUNCTIONS ---------------------------------------------------------

  //: write info of the dbskbranch to an output stream
  virtual vcl_ostream& print(vcl_ostream& os);

  // MEMBER VARIABLES ----------------------------------------------------------
protected:

  //: id of the edge, unique for each element of the graph
  unsigned int id_;
};

#endif // shp/dbsksp/dbsksp_xshock_edge.h


