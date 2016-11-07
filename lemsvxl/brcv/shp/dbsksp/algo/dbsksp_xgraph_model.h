// This is shp/dbsksp/algo/dbsksp_xgraph_model.h
#ifndef dbsksp_xgraph_model_h_
#define dbsksp_xgraph_model_h_

//:
// \file
// \brief A model to generate an xgraph from a vector of parameters
//        
// \author Nhon Trinh ( ntrinh@lems.brown.edu )
// \date Dec 17, 2009
//
// \verbatim
//  Modifications
// \endverbatim


#include <dbsksp/dbsksp_xshock_graph_sptr.h>
#include <dbsksp/dbsksp_xshock_edge_sptr.h>
#include <dbsksp/dbsksp_xshock_node_sptr.h>


#include <vnl/vnl_vector.h>
#include <vcl_map.h>
#include <vcl_vector.h>


//==============================================================================
// dbsksp_xgraph_model
//==============================================================================


//: A class that can update a shock graph with a `state' vector
class dbsksp_xgraph_model
{
public:
  //: Constructor
  dbsksp_xgraph_model(const dbsksp_xshock_graph_sptr& xgraph): xgraph_(xgraph)
  {
  
  }

  //: Destructor
  virtual ~dbsksp_xgraph_model(){};

  //: Dimension of the state vecotr
  virtual unsigned dim() const = 0;

  //: Current `state' of the shock graph
  virtual void get_xgraph_state(vnl_vector<double >& x) = 0;

  //: Update the shock graph with a `state' vector
  virtual void set_xgraph_state(const vnl_vector<double >& x) = 0;

  //: The current shock graph
  dbsksp_xshock_graph_sptr xgraph() const
  { return this->xgraph_; }

protected:
  // current xgraph
  dbsksp_xshock_graph_sptr xgraph_;
};





//==============================================================================
// dbsksp_xgraph_model_using_L_alpha_phi_radius
//==============================================================================
//: An xgraph model using L and alpha as intrinsic parameters for edges and phi
// and radius for nodes
class dbsksp_xgraph_model_using_L_alpha_phi_radius: public dbsksp_xgraph_model
{
public:
  //: Constructor
  dbsksp_xgraph_model_using_L_alpha_phi_radius(const dbsksp_xshock_graph_sptr& xgraph,
    unsigned root_vid, unsigned pseudo_parent_eid);

  //: Destructor
  ~dbsksp_xgraph_model_using_L_alpha_phi_radius(){};

  //: Dimension of the state vecotr
  virtual unsigned dim() const 
  { return this->dim_; }

  //: Current `state' of the shock graph
  virtual void get_xgraph_state(vnl_vector<double >& x);

  //: Update the shock graph with a `state' vector
  virtual void set_xgraph_state(const vnl_vector<double >& x);

  ////: Update the state of the shock graph with another shock graph with exact topology (required)
  //void set_xgraph_state(const dbsksp_xshock_graph_sptr& other, 
  //  vcl_map<dbsksp_xshock_node_sptr, dbsksp_xshock_node_sptr > map_node_model_to_other,
  //  vcl_map<dbsksp_xshock_edge_sptr, dbsksp_xshock_edge_sptr > map_edge_model_to_other);

  //: Get intrinsic parameters for a given set of nodes and edges
  // The parameters will be packed in 'x' using the order in 'active_nodes' followed
  // by 'active_edges'
  void get_intrinsic_params(const vcl_vector<dbsksp_xshock_node_sptr >& active_nodes,
    const vcl_vector<dbsksp_xshock_edge_sptr >& active_edges,
    vnl_vector<double >& x);


  //: Get pointers for direct access to intrinsic parameters of a set of nodes and edges
  // The parameters will be packed using the order of 'active_nodes' followed by 'active_edges'
  void get_intrinsic_params_ptr(const vcl_vector<dbsksp_xshock_node_sptr >& active_nodes,
    const vcl_vector<dbsksp_xshock_edge_sptr >& active_edges,
    vcl_vector<double* >& x_ptr);

  //: Set intrinsic parameters for a given set of nodes and edges
  // The parameters will be retrieved from 'x' using the order in 'active_nodes' followed
  // by 'active_edges'
  void set_intrinsic_params(const vcl_vector<dbsksp_xshock_node_sptr >& active_nodes,
    const vcl_vector<dbsksp_xshock_edge_sptr >& active_edges,
    vnl_vector<double >& x);

  //: Count number of free parameters corresponding to a list of nodes and edges
  unsigned get_num_intrinsic_params(const vcl_vector<dbsksp_xshock_node_sptr >& active_nodes,
    const vcl_vector<dbsksp_xshock_edge_sptr >& active_edges);

  //: Update the parameter map from the shock graph
  void update_param_maps_from_xgraph();

  //: Update the xgraph using the parameter stored at param maps
  void update_xgraph_from_param_maps();

  // parameters at node and edges
  struct edge_params
  {
    double chord_len;
    double alpha_parent; // angle between the chord and shock tangent
    double alpha_child;
  };

  struct node_params
  {
    unsigned degree;
    double radius;
    double phi_parent;
    double delta_phi; // not used for degree-2 node
  };

protected:
  
  

  //: Parent edge of a node
  dbsksp_xshock_edge_sptr parent_edge(const dbsksp_xshock_node_sptr xv);

  dbsksp_xshock_node_sptr root_node_;
  dbsksp_xshock_edge_sptr pseudo_parent_edge_;

  unsigned dim_; // dimension of the state vector
  vcl_map<dbsksp_xshock_edge_sptr, edge_params > map_xe_to_params_;
  vcl_map<dbsksp_xshock_node_sptr, node_params > map_xv_to_params_;
};




//==============================================================================
// dbsksp_xgraph_model_using_x_y_r_psi_phi
//==============================================================================
//: An xgraph model using all extrinsic info at the nodes
class dbsksp_xgraph_model_using_x_y_r_psi_phi: public dbsksp_xgraph_model
{
public:
  //: Constructor
  dbsksp_xgraph_model_using_x_y_r_psi_phi(const dbsksp_xshock_graph_sptr& xgraph,
    unsigned root_vid, unsigned pseudo_parent_eid);

  //: Destructor
  ~dbsksp_xgraph_model_using_x_y_r_psi_phi(){};

  //: Dimension of the state vecotr
  virtual unsigned dim() const { return this->dim_; }

  //: Current `state' of the shock graph
  virtual void get_xgraph_state(vnl_vector<double >& x);

  //: Update the shock graph with a `state' vector
  virtual void set_xgraph_state(const vnl_vector<double >& x);

  ////: Get intrinsic parameters for a given set of nodes and edges
  //// The parameters will be packed in 'x' using the order in 'active_nodes' followed
  //// by 'active_edges'
  //void get_params(const vcl_vector<dbsksp_xshock_node_sptr >& active_nodes,
  //  const vcl_vector<dbsksp_xshock_edge_sptr >& active_edges,
  //  vnl_vector<double >& x);


  ////: Get pointers for direct access to intrinsic parameters of a set of nodes and edges
  //// The parameters will be packed using the order of 'active_nodes' followed by 'active_edges'
  //void get_params_ptr(const vcl_vector<dbsksp_xshock_node_sptr >& active_nodes,
  //  const vcl_vector<dbsksp_xshock_edge_sptr >& active_edges,
  //  vcl_vector<double* >& x_ptr);

  ////: Set intrinsic parameters for a given set of nodes and edges
  //// The parameters will be retrieved from 'x' using the order in 'active_nodes' followed
  //// by 'active_edges'
  //void set_intrinsic_params(const vcl_vector<dbsksp_xshock_node_sptr >& active_nodes,
  //  const vcl_vector<dbsksp_xshock_edge_sptr >& active_edges,
  //  vnl_vector<double >& x);

  ////: Count number of free parameters corresponding to a list of nodes and edges
  //unsigned get_num_intrinsic_params(const vcl_vector<dbsksp_xshock_node_sptr >& active_nodes,
  //  const vcl_vector<dbsksp_xshock_edge_sptr >& active_edges);



  // \TODO: implement these two functions

  //: Update the parameter map from the shock graph
  void update_param_maps_from_xgraph(){};

  //: Update the xgraph using the parameter stored at param maps
  void update_xgraph_from_param_maps(){};

  struct node_params
  {
    unsigned degree;
    double x;
    double y;
    double radius;
    double phi_parent;
    double psi_parent;
    double delta_phi; // not used for degree-2 node
  };

protected:
  
  

  ////: Parent edge of a node
  //dbsksp_xshock_edge_sptr parent_edge(const dbsksp_xshock_node_sptr xv);

  dbsksp_xshock_node_sptr root_node_;
  dbsksp_xshock_edge_sptr pseudo_parent_edge_;

  unsigned dim_; // dimension of the state vector
  vcl_map<dbsksp_xshock_node_sptr, node_params > map_xv_to_params_;
  //vcl_map<dbsksp_xshock_edge_sptr, edge_params > map_xe_to_params_;
};


#endif // shp/dbsksp/algo/dbsksp_xgraph_model.h









