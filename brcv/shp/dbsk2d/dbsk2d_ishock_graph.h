// This is brcv/shp/dbsk2d/dbsk2d_ishock_graph.h
#ifndef dbsk2d_ishock_graph_h_
#define dbsk2d_ishock_graph_h_
//:
// \file
// \brief Intrinsic shock graph class
//        This class is temporary and is really supposed to be subclassed from dbgrl_graph class
// \author Amir Tamrakar
// \date 05/04/05
//
//
// \verbatim
//  Modifications
// \endverbatim

#include <vcl_list.h>
#include <vcl_iostream.h>
#include <vbl/vbl_ref_count.h>

#include <dbsk2d/dbsk2d_utils.h>
#include <dbsk2d/dbsk2d_boundary.h>
#include <dbsk2d/dbsk2d_boundary_sptr.h>
#include <dbsk2d/dbsk2d_ishock_elm.h>
#include <dbsk2d/dbsk2d_ishock_node.h>
#include <dbsk2d/dbsk2d_ishock_edge.h>


//: The Intrinsic shock graph class
class dbsk2d_ishock_graph : public vbl_ref_count
{
public:
  typedef vcl_list<dbsk2d_ishock_node*>::iterator vertex_iterator;
  typedef vcl_list<dbsk2d_ishock_edge*>::iterator edge_iterator;

protected:
  dbsk2d_boundary_sptr _boundary;  ///< pointer to associated boundary
  int _nextAvailableID;  ///< keeps the next available id, always increases

  //: The vector of vertices
  vcl_list<dbsk2d_ishock_node* > vertices_;

  //: The vector of edges
  vcl_list<dbsk2d_ishock_edge* > edges_;

  //: ob edges
  vcl_map<int,dbsk2d_ishock_edge*> ob_edges_;

public:

  //: Constructor
  dbsk2d_ishock_graph(dbsk2d_boundary_sptr boundary);

  //: Destructor
  virtual ~dbsk2d_ishock_graph();

  //: return the corresponding boundary object
  dbsk2d_boundary_sptr boundary() { return _boundary; }

  //: to remove circular link undo boundary 
  void remove_boundary(){_boundary = 0;}

  //: increment the id counter and return new id
  int nextAvailableID() { _nextAvailableID++; return _nextAvailableID; }

  //: get available id
  int getAvailableID() { return _nextAvailableID;}

  //-------------------------------------------------------------------
  // Standard Graph functions
  //-------------------------------------------------------------------

  //-------------------------------------------------------------------
  // Access operations
  //-------------------------------------------------------------------

  int number_of_nodes()  const; ///<  returns the number of nodes in the shock graph. 
  int number_of_edges() const; ///<  returns the number of edges in the shock graph.

  int outdeg(dbsk2d_ishock_node* v, bool exclude_hidden=true); ///<  returns the number of edges adjacent to node v (| adj_edges(v)|).
  int indeg(dbsk2d_ishock_node* v, bool exclude_hidden=true); ///< returns the number of edges ending at v (| in_edges(v)|)
  int degree(dbsk2d_ishock_node* v, bool exclude_hidden=true);  ///< returns outdeg(v) + indeg(v).

  //: returns the list V of all nodes of the shock graph
  vcl_list<dbsk2d_ishock_node*>& all_nodes() { return vertices_; } 

  //:  returns the list E of all edges of the shock graph.  
  vcl_list<dbsk2d_ishock_edge*>& all_edges() { return edges_; }

  ishock_edge_list adj_edges(dbsk2d_ishock_node* v); ///<  returns adj_edges(v). 
  ishock_node_list adj_nodes(dbsk2d_ishock_node* v); ///<  returns the list of all nodes adjacent to v. 

  //-------------------------------------------------------------------
  // Graph building functions
  //-------------------------------------------------------------------

  //: Adds a new vertex to the graph
  void add_vertex(dbsk2d_ishock_node* vertex);

  //: Deletes a vertex in the graph
  void remove_vertex(dbsk2d_ishock_node* vertex);

  //: Add an edge
  void add_edge(dbsk2d_ishock_edge* e1);
  
  //: Remove an edge
  void remove_edge(dbsk2d_ishock_edge* e1);

  //: clears all the nodes and edges of this graph
  void clear();

  //-------------------------------------------------------------------
  // Directed graph operations
  //-------------------------------------------------------------------
    
  dbsk2d_ishock_node* source(dbsk2d_ishock_edge* e); ///< returns the source node of edge e. 
  dbsk2d_ishock_node* dest(dbsk2d_ishock_edge* e); ///< returns the dest node of edge e. 
  dbsk2d_ishock_node* opposite(dbsk2d_ishock_node* v, dbsk2d_ishock_edge* e); ///<  returns dest(e) if v = source(e) and source(e) otherwise. 

  ishock_edge_list out_edges(dbsk2d_ishock_node* v); ///<  returns adj_edges(v) if the graph is directed and the empty list otherwise. 
  ishock_edge_list in_edges(dbsk2d_ishock_node* v); ///<  returns in_edges(v) if the graph is directed and the empty list otherwise. 

  dbsk2d_ishock_edge* first_adj_edge(dbsk2d_ishock_node* v); ///<  returns the first edge in the adjacency list of v (nil if this list is empty). 
  dbsk2d_ishock_edge* last_adj_edge(dbsk2d_ishock_node* v); ///<  returns the last edge in the adjacency list of v (nil if this list is empty). 
  dbsk2d_ishock_edge* adj_succ(dbsk2d_ishock_edge* e); ///<  returns the successor of edge e in the adjacency list of node source(e) (nil if it does not exist). 
  dbsk2d_ishock_edge* adj_pred(dbsk2d_ishock_edge* e); ///<  returns the predecessor of edge e in the adjacency list of node source(e) (nil if it does not exist). 
  dbsk2d_ishock_edge* cyclic_adj_succ(dbsk2d_ishock_edge* e); ///<  returns the cyclic successor of edge e in the adjacency list of node source(e). 
  dbsk2d_ishock_edge* cyclic_adj_pred(dbsk2d_ishock_edge* e); ///<  returns the cyclic predecessor of edge e in the adjacency list of node source(e). 
  dbsk2d_ishock_edge* first_in_edge(dbsk2d_ishock_node* v); ///<  returns the first edge of in_edges(v) (nil if this list is empty). 
  dbsk2d_ishock_edge* last_in_edge(dbsk2d_ishock_node* v); ///<  returns the last edge of in_edges(v) (nil if this list is empty). 
  dbsk2d_ishock_edge* in_succ(dbsk2d_ishock_edge* e); ///<  returns the successor of edge e in in_edges(target(e)) (nil if it does not exist). 
  dbsk2d_ishock_edge* in_pred(dbsk2d_ishock_edge* e); ///<  returns the predecessor of edge e in in_edges(target(e)) (nil if it does not exist). 
  dbsk2d_ishock_edge* cyclic_in_succ(dbsk2d_ishock_edge* e); ///<  returns the cyclic successor of edge e in in_edges(target(e)) (nil if it does not exist). 
  dbsk2d_ishock_edge* cyclic_in_pred(dbsk2d_ishock_edge* e); ///<  returns the cyclic predecessor of edge e in in_edges(target(e)) (nil if it does not exist). 

  //-------------------------------------------------------------------
  // Undirected graph operations   
  //-------------------------------------------------------------------
  
  dbsk2d_ishock_edge* adj_succ(dbsk2d_ishock_edge* e, dbsk2d_ishock_node* v); ///<  returns the successor of edge e in the adjacency list of v.
  dbsk2d_ishock_edge* adj_pred(dbsk2d_ishock_edge* e, dbsk2d_ishock_node* v);///<  returns the predecessor of edge e in the adjacency list of v.
  dbsk2d_ishock_edge* cyclic_adj_succ(dbsk2d_ishock_edge* e, dbsk2d_ishock_node* v, bool exclude_hidden = true); ///<  returns the cyclic successor of edge e in the adjacency list of v. 
  dbsk2d_ishock_edge* cyclic_adj_pred(dbsk2d_ishock_edge* e, dbsk2d_ishock_node* v, bool exclude_hidden = true); ///<  returns the cyclic predecessor of edge e in the adjacency list of v.

  // Returns all invalid shocks
  void invalid_shocks(vcl_vector<dbsk2d_ishock_edge*>& shocks);
 
  //: Checks whether shock is valid
  bool valid_shock_graph();

  //: recompute the extrinsic points of each shock edge if they have changed
  void update_shocks();

  //: Print an ascii summary to the stream
  void print_summary(vcl_ostream &os);

  //: Determine out of bound shocks
  void ob_shocks();

};

#endif // dbsk2d_ishock_graph_h_
