// This is shp/dbsksp/dbsksp_xshock_graph.h
#ifndef dbsksp_xshock_graph_h_
#define dbsksp_xshock_graph_h_

//:
// \file
// \brief A class for generative extrinsic shock graph
//        
// \author Nhon Trinh ( ntrinh@lems.brown.edu)
// \date August 9, 2008
//
// \verbatim
//  Modifications
// \endverbatim

#include <dbgrl/dbgrl_graph.h>
#include <vgl/vgl_box_2d.h>
#include <dbsksp/dbsksp_xshock_node.h>
#include <dbsksp/dbsksp_xshock_node_sptr.h>
#include <dbsksp/dbsksp_xshock_edge.h>
#include <dbsksp/dbsksp_xshock_edge_sptr.h>

#include <vsol/vsol_box_2d_sptr.h>



//: A class for a generative extrinsic shock graph
class dbsksp_xshock_graph : public dbgrl_graph<dbsksp_xshock_node, dbsksp_xshock_edge>
{
public:
  // Constructors / Destructors ------------------------------------------------

  //: Constructor
  dbsksp_xshock_graph();

  //: Copy constructor
  dbsksp_xshock_graph(dbsksp_xshock_graph& that);
  
  //: Destructor
  virtual ~dbsksp_xshock_graph(){};
  
  // DATA ACCESS ---------------------------------------------------------------

  //: Return the type identifier string
  vcl_string is_a() const {return "dbsksp_xshock_graph"; }

  //: unique id given to its element
  unsigned int next_available_id(){ return ++this->next_available_id_; }

  //: set the next_available_id variable
  void set_next_available_id(unsigned int new_next_id)
  { this->next_available_id_ = new_next_id; }

  //: Return the edge with the given id
  dbsksp_xshock_edge_sptr edge_from_id(unsigned id);

  //: Return the node with the given id
  dbsksp_xshock_node_sptr node_from_id(unsigned id);

  //: id of root vertex
  unsigned root_vertex_id() const {return this->root_vertex_id_; }
  
  // SHOCK GRAPH GEOMETRY PROPERTIES -------------------------------------------

  //: similarity transform
  bool similarity_transform(const vgl_point_2d<double >& ref_pt, double dx, 
    double dy, double dtheta, double scale);

  //: similarity transform (compatible with matlab's similarity transform)
    bool similarity_transform2(const vgl_point_2d<double >& ref_pt, double dx,
      double dy, double dtheta, double scale);

  //: Translate the xshock graph
  void translate(double dx, double dy);

  //: Scale up the xshock graph around the center (cx, cy)
  void scale_up(double cx, double cy, double scale_factor);

  //: Mirror an xgraph around a y-axis
  void mirror_around_y_axis(double axis_x);

  //: area of the shock graph
  double area();

  //: Return bounding box of the xgraph. Automatically compute if it has never been computed.
  vsol_box_2d_sptr bounding_box();

  //: Update bounding box of the xgraph with current graph property
  void update_bounding_box();

  //: Group the vertices by their depths
  // Example: vertex_bins[1] is a vector of all vertices with depth 1
  bool group_vertices_by_depth(vcl_vector<vcl_vector<unsigned > >& vertex_bins);

protected:
  //: Compute bounding box of the shock graph
  vgl_box_2d<double > compute_approx_bounding_box();

public:
 
  // HIGH-LEVEL GRAPH FUNCTIONS ------------------------------------------------
  
  //: Insert a node in the middle of an edge without modifying the boundary 
  // geometric position of the new node is indicated by t, t \in [0,1]
  // with t = 0: source node, t = 1: target node
  dbsksp_xshock_node_sptr insert_xshock_node(const dbsksp_xshock_edge_sptr& xe,
    double t);


  //: Insert a node in the middle of an edge, given the descriptor of the new node
  // assuming the start node is the source node
  dbsksp_xshock_node_sptr insert_xshock_node(const dbsksp_xshock_edge_sptr& xe,
    const dbsksp_xshock_node_descriptor& xdesc);

  //: Insert a node in the middle of an edge, given the descriptor of the new node
  dbsksp_xshock_node_sptr insert_xshock_node(const dbsksp_xshock_edge_sptr& xe,
    const dbsksp_xshock_node_sptr& start_xv,
    const dbsksp_xshock_node_descriptor& xdesc,
    dbsksp_xshock_edge_sptr& xe_from_start_to_new_xv, // connecting start_xv and new node
    dbsksp_xshock_edge_sptr& xe_from_new_xv_to_end); // connectig new node and end_xv



  //: Delete an A12 shock node
  dbsksp_xshock_edge_sptr remove_A12_node(const dbsksp_xshock_node_sptr& xv);

  //: Insert a node in the middle of a terminal edge (to extend the xshock branch)
  // `shock_length' is the length of the new A12 edge
  dbsksp_xshock_node_sptr insert_xshock_node_at_terminal_edge(const 
    dbsksp_xshock_edge_sptr& xe, double shock_length);

  //: Add an A_infty branch to an existing A_1^2 node, right before a reference edge "xe"
  // The A_1^2 node becomes an A_1^3 node
  dbsksp_xshock_edge_sptr insert_A_infty_branch(const dbsksp_xshock_node_sptr& xv,
                   const dbsksp_xshock_edge_sptr& xe);

  //: Remove the A_infty edge at an A_1^2-A_infty node and convert the A_1^2-A_infty node
  // (A_1^3 node) to an A_1^2 node. If the phi angle of the A_infty branch is not zero,
  // it will distributed to the two adjacent branches. phi_ratio_succ (\in [0, 1]) is the
  // proportion given to the successing branch, in CCW order, of the A_infty branch.
  // The predessessing branch get (1-phi_ratio_succ) * phi
  // Return the pointer to the edge which previously succeeded the A_infty branch
  dbsksp_xshock_edge_sptr remove_A_infty_branch(const dbsksp_xshock_node_sptr& xv,
    const dbsksp_xshock_edge_sptr& terminal_xe, double phi_ratio_succ = 0.5);


  //: Remove a terminal edge (a leaf edge) at an A_1^2 node
  // Return pointer to the edge which preceded the terminal edge before the removal
  dbsksp_xshock_edge_sptr remove_leaf_edge_at_A12_node(const dbsksp_xshock_edge_sptr& terminal_xe);



    
    
  // UTILITY -------------------------------------------------------------------

  //: Update descriptors of degree-1 nodes to be compatiable with its adjacent 
  // node (degree > 1)
  // Require: degree of xv is 1.
  bool update_degree_1_node(const dbsksp_xshock_node_sptr& xv);

  //: Update descriptors of degree-1 nodes around a particular node ('xv') so
  // that they are compatible
  bool update_degree_1_nodes_around(const dbsksp_xshock_node_sptr& xv);

  //: Update descriptors of all degree-1 nodes
  bool update_all_degree_1_nodes();
  
  //: Compute vertex depth
  bool compute_vertex_depths(unsigned root_vertex_id);
  
  //: print info of the shock graph to an output stream
  vcl_ostream& print(vcl_ostream & os) const;

protected:
  //: unique id given to its element
  unsigned int next_available_id_;
  unsigned int root_vertex_id_;
  vsol_box_2d_sptr bounding_box_;
};




#endif // shp/dbsksp/dbsksp_xshock_graph.h


