// This is brcv/shp/dbsk2d/dbsk2d_shock_graph.h
#ifndef dbsk2d_shock_graph_h_
#define dbsk2d_shock_graph_h_
//:
// \file
// \brief Shock graph class
//        This class is temporary and is really supposed to be subclassed from dbgrl_graph class
// \author Amir Tamrakar
// \date 06/08/05
//
//
// \verbatim
//  Modifications
//      O.C.Ozcanli 05/31/2006 added box_ and related methods
// \endverbatim

#include <vcl_set.h>
#include <dbgrl/dbgrl_graph.h>

#include <dbsk2d/dbsk2d_shock_node.h>
#include <dbsk2d/dbsk2d_shock_edge.h>

//: The Shock graph class
//
// The shock graph is a directed graph. For each edge, source(e)=formation node
// target(e)=termination node. In general, termination nodes can be NULL if it goes 
// to infinity for external shock branches. But this is not allowed for extrinsic
// shock graphs because the graphs are sampled in a finite window. Thus the termination
// nodes can be TERMINAL type.
class dbsk2d_shock_graph : public dbgrl_graph<dbsk2d_shock_node , dbsk2d_shock_edge> 
{
public:

  //: Constructor
  dbsk2d_shock_graph() : bounding_box_(0) {}

  //: Destructor
  virtual ~dbsk2d_shock_graph(){}

  //: clear all the nodes and edges of this graph
  //  and also the other objects defined on the nodes and edges
  virtual void clear();

  //: delete the shock fragments defined on this shock graph
  void delete_shock_fragments();

  //: set the bounding box, the rest of the object stays uneffected (const)
  void set_bounding_box(vsol_box_2d_sptr const& box) const { bounding_box_ = new vsol_box_2d(*box); }

  //: get the bounding box
  vsol_box_2d_sptr get_bounding_box() const { return bounding_box_; }

  //: these methods are required when degree two nodes are not important in traversal
  dbsk2d_shock_node_sptr get_other_end_merging_degree_twos(dbsk2d_shock_node_sptr n, dbsk2d_shock_edge_sptr e);
  dbsk2d_shock_node_sptr get_other_end_merging_degree_twos(dbsk2d_shock_node_sptr n, dbsk2d_shock_edge_sptr e, vcl_vector<dbsk2d_shock_edge_sptr>& edges);

  dbsk2d_shock_node_sptr get_node(int id);
private:

  //: even if the object is a constant object its bounding box will be modifiable
  mutable vsol_box_2d_sptr bounding_box_;
};

#endif // dbsk2d_shock_graph_h_
