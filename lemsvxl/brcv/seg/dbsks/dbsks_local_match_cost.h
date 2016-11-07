// This is seg/dbsks/dbsks_local_match_cost.h
#ifndef dbsks_local_match_cost_h_
#define dbsks_local_match_cost_h_

//:
// \file
// \brief Cost function in local match
//        
// \author Nhon Trinh ( ntrinh@lems.brown.edu)
// \date Feb 8, 2008
//
// \verbatim
//  Modifications
// \endverbatim


#include <vnl/vnl_cost_function.h>
#include <dbsksp/dbsksp_shock_graph_sptr.h>
#include <dbsks/dbsks_ocm_image_cost_sptr.h>
#include <dbsksp/dbsksp_shock_edge_sptr.h>
#include <dbsksp/dbsksp_shock_node_sptr.h>
#include <dbsksp/dbsksp_shapelet_sptr.h>
#include <dbsksp/dbsksp_xshock_node_descriptor.h>
#include <vcl_map.h>
#include <dbgl/algo/dbgl_circ_arc.h>
#include <dbgl/algo/dbgl_biarc.h>



// ============================================================================
// dbsks_local_match_cost
// ============================================================================

class dbsks_local_match_cost : public vnl_cost_function 
{
public:
  //: Default constructors ------------------------------------------------
  dbsks_local_match_cost() : vnl_cost_function(1), fmin_(1e100) {}

  //: Constructor from a graph
  dbsks_local_match_cost(const dbsksp_shock_graph_sptr& graph);

  //: Access member variables ---------------------------------------------------
  dbsksp_shock_graph_sptr graph() {return this->graph_; }
  void set_graph(const dbsksp_shock_graph_sptr& graph);
  

  //: Return the oriented chamfer matcher for the image
  dbsks_ocm_image_cost_sptr oriented_chamfer() const
  { return this->oriented_chamfer_; }

  //: ratio between image cost and intrinsic cost
  float lambda() const { return this->lambda_; };

  //: Convert a set of fragments for each edge to a configuration vector for the graph
  vnl_vector<double > convert_to_graph_params(
    const vcl_map<dbsksp_shock_edge_sptr, dbsksp_shapelet_sptr >& shapelet_map);

  //: Convert a configuration vector to a list of twoshapelets associated with the edges
  void convert_to_twoshapelet_map(const vnl_vector<double >& x, 
    vcl_map<dbsksp_shock_edge_sptr, dbsksp_twoshapelet_sptr >& twoshapelet_map);


  //: Convert a configuration vector to a list of xnodes associated with the vertices
  void convert_to_xnode_map(const vnl_vector<double >& x,
    vcl_map<dbsksp_shock_node_sptr, dbsksp_xshock_node_descriptor >& xnode_map);


  //: Convert a configuration vector to a list of xnodes associated with the vertices
  void convert_to_circ_arc_list(const vnl_vector<double >& x,
    vcl_vector<dbgl_circ_arc >& arc_list);


  //: Cost value for each configuration of the graph
  double f(const vnl_vector<double>& x);

public:
  dbsksp_shock_graph_sptr graph_;

  // Oriented chamfer matching cost
  dbsks_ocm_image_cost_sptr oriented_chamfer_;
  
  //: Sampling rate when computing arc costs
  float ds_shapelet_;

  //: ratio between image cost and intrinsic cost
  float lambda_;

  vnl_vector<double > xmin_;
  double fmin_;
};




//: convert a vector to an xnode
dbsksp_xshock_node_descriptor dbsks_convert_to_xnode(const vnl_vector<double >& x);

//: convert an xnode to a vector
vnl_vector<double > dbsks_convert_to_vector(const dbsksp_xshock_node_descriptor& xnode);








// ============================================================================
// dbsks_a12_xnode_cost
// ============================================================================

class dbsks_a12_xnode_cost : public vnl_cost_function 
{
public:
  //: Default constructors ------------------------------------------------
  dbsks_a12_xnode_cost() : vnl_cost_function(5) {}

  //: Access member variables ---------------------------------------------------

  //: Set necessary internal variables for the cost function
  void set(const dbsks_ocm_image_cost_sptr ocm,
    float lambda,
    float ds,
    const dbsksp_xshock_node_descriptor& xnode_parent,
    const dbsksp_xshock_node_descriptor& xnode_child,
    const dbsksp_shapelet_sptr& ref_shapelet_parent = 0,
    const dbsksp_shapelet_sptr& ref_shapelet_child = 0);


  //: Return the oriented chamfer matcher for the image
  dbsks_ocm_image_cost_sptr ocm() const
  { return this->ocm_; }

  //: ratio between image cost and intrinsic cost
  float lambda() const { return this->lambda_; };

  //: Sampling rate
  float ds() const {return this->ds_; }

  //: Cost value for each configuration of the graph
  virtual double f(const vnl_vector<double>& x);

  double f_regularized_using_len(const vnl_vector<double>& x);
  double f_regularized_using_shock_edit(const vnl_vector<double>& x);


  // Member variables
public:
  dbsksp_xshock_node_descriptor xnode_parent_;
  dbsksp_xshock_node_descriptor xnode_child_;

  //: cache values for debugging purposes
  double accum_shape_cost_;
  double accum_len_;
  double accum_image_cost_;

protected:
  // Oriented chamfer matching cost
  dbsks_ocm_image_cost_sptr ocm_;
  
  //: Sampling rate when computing arc costs
  float ds_;

  //: ratio between image cost and intrinsic cost
  float lambda_;

  ////////////////////////////////////////
  //: parent ref shapelet
  dbsksp_shapelet_sptr ref_shapelet_parent_;
  dbsksp_shapelet_sptr ref_shapelet_child_;
};



// ============================================================================
// dbsks_ainfty_xnode_cost
// ============================================================================

class dbsks_ainfty_xnode_cost : public vnl_cost_function 
{
public:
  //: Default constructors ------------------------------------------------
  dbsks_ainfty_xnode_cost() : vnl_cost_function(5) {}

  //: Access member variables ---------------------------------------------------

  //: Set necessary internal variables for the cost function
  void set(const dbsks_ocm_image_cost_sptr ocm,
    float lambda,
    float ds,
    const dbsksp_xshock_node_descriptor& xnode_parent,
    const dbsksp_shapelet_sptr& ref_shapelet_parent = 0);


  //: Return the oriented chamfer matcher for the image
  dbsks_ocm_image_cost_sptr ocm() const
  { return this->ocm_; }

  //: ratio between image cost and intrinsic cost
  float lambda() const { return this->lambda_; };

  //: Sampling rate
  float ds() const {return this->ds_; }

  //: Cost value for each configuration of the graph
  double f(const vnl_vector<double>& x);

  // two different types of costs
  // regularized the energy by length of the boundary
  // TODO: use xfrag cost
  double f_regularized_using_len(const vnl_vector<double>& x);

  // regularized the energy by shock-edit distance to the reference shapelet
  double f_regularized_using_shock_edit(const vnl_vector<double>& x);


public:
  dbsksp_xshock_node_descriptor xnode_parent_;
  dbsksp_shapelet_sptr ref_shapelet_parent_;


  //: cache values for debugging purposes
  double accum_shape_cost_;
  double accum_len_;
  double accum_image_cost_;
  
protected:
  // Oriented chamfer matching cost
  dbsks_ocm_image_cost_sptr ocm_;

  //: Sampling rate when computing arc costs
  float ds_;

  //: ratio between image cost and intrinsic cost
  float lambda_;

  
};








// ============================================================================
// dbsks_xfrag_cost
// ============================================================================


//: Cost of one extrinsic fragment: 
class dbsks_xfrag_cost
{
public:
  //: Default constructors ------------------------------------------------
  dbsks_xfrag_cost() {}

  //: Access member variables ---------------------------------------------------

  //: Set necessary internal variables for the cost function
  void set(const dbsks_ocm_image_cost_sptr ocm,
    float lambda,
    float ds);


  //: Return the oriented chamfer matcher for the image
  dbsks_ocm_image_cost_sptr ocm() const
  { return this->ocm_; }

  //: ratio between image cost and intrinsic cost
  float lambda() const { return this->lambda_; };

  //: Sampling rate
  float ds() const {return this->ds_; }

  //: Cost value for a extrinsic fragment, between two xnodes
  double f_regularized_using_len(const dbsksp_xshock_node_descriptor& parent, 
    const dbsksp_xshock_node_descriptor& child);

  //: Cost value for a terminal fragment, characterized by an xnode
  double f_regularized_using_len(const dbsksp_xshock_node_descriptor& terminal_xnode);

  //: Image cost for an extrinsic fragment (integrate ocm cost
  // along the boundary)
  double f_image(const dbsksp_xshock_node_descriptor& xnode_parent, 
    const dbsksp_xshock_node_descriptor& xnode_child);

  //: Image cost for an A_infty fragment
  double f_image(const dbsksp_xshock_node_descriptor& xnode_terminal);

  //: Boundary length of an extrinsic fragment, characterized by two xnodes
  double f_bnd_length(const dbsksp_xshock_node_descriptor& xnode_parent, 
    const dbsksp_xshock_node_descriptor& xnode_child);

  //: Shock-edit distance of an extrinsic fragment compared to a shapelet
  double f_shock_edit(const dbsksp_xshock_node_descriptor& xnode_parent, 
    const dbsksp_xshock_node_descriptor& xnode_child,
    const dbsksp_shapelet_sptr& sref);
  
protected:
  // Oriented chamfer matching cost
  dbsks_ocm_image_cost_sptr ocm_;

  //: Sampling rate when computing arc costs
  float ds_;

  //: ratio between image cost and intrinsic cost
  float lambda_;  
};







#endif // seg/dbsks/dbsks_local_match_cost.h



