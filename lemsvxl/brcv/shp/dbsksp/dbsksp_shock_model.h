// This is shp/dbsksp/dbsksp_shock_model.h
#ifndef dbsksp_shock_model_h_
#define dbsksp_shock_model_h_

//:
// \file
// \brief A class to represent the free parameters in a shock graph (of circular arc boundary)
//        
// \author Nhon Trinh ( ntrinh@lems.brown.edu)
// \date Oct 7, 2006
//
// \verbatim
//  Modifications
// \endverbatim

#include <vcl_map.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_int_4.h>
#include <dbsksp/dbsksp_shock_graph_sptr.h>
#include <dbsksp/dbsksp_shock_edge_sptr.h>
#include <dbsksp/dbsksp_shock_node_sptr.h>


//: A vector representing the free parameters of a shock graph
class dbsksp_shock_model
{
public:

  // ========================== CONSTRUCTORS / DESTRUCTORS ====================
  //: Constructor
  dbsksp_shock_model(const dbsksp_shock_graph_sptr& shock_graph);
  
  //: Destructor
  virtual ~dbsksp_shock_model(){};


  // ============== DATA ACCESS ====================================  
  //: Return and set the shock graph associated with this set of free variables
  dbsksp_shock_graph_sptr shock_graph() const {return this->shock_graph_; }
  void set_shock_graph(const dbsksp_shock_graph_sptr& shock_graph);
  

  // ============= UTILITY =============================================

  //: return the number of parameters of this shock graph model
  int num_params() const {return this->num_params_; }

  //; Update (compute) the number of free parameters of the model
  // This number is typically dependent on the shock graph, among others
  // By default, the function compute the total degree of freedom of the shape
  virtual void update_num_params();
  
  //: construct `this' vector from its shock graph
  virtual void compute_params_from_shock_graph(vnl_vector<double >& params) = 0;

  //: Update shock graph with `this' set of free params
  virtual void update_shock_graph(const vnl_vector<double >& params ) = 0;




  // ============== MEMBER VARIABLES ===================================
protected:
  //: the shock graph this set of variables is associated with
  dbsksp_shock_graph_sptr shock_graph_;


  //: Set number of free parameters
  void set_num_params(int num_params){ this->num_params_ = num_params; }

  //: number of free variables in the model
  int num_params_;
};



//: A model of free parameters of a shock graph, keeping phi, m, edge length 
// as the free parameters
class dbsksp_phi_m_L_shock_model: public dbsksp_shock_model
{
public:
  // ========================== CONSTRUCTORS / DESTRUCTORS ====================
  //: constructor
  dbsksp_phi_m_L_shock_model(
    const dbsksp_shock_graph_sptr& shock_graph);

  //: destructor
  virtual ~dbsksp_phi_m_L_shock_model(){};


  // ============= UTILITY =============================================

  //; Update (compute) the number of free parameters of the model
  // This number is typically dependent on the shock graph, among others
  // By default, the function compute the total degree of freedom of the shape
  virtual void update_num_params();
  
  //: construct the free parameter vector from the current shock graph
  virtual void compute_params_from_shock_graph(vnl_vector<double >& params);

  //: Update shock graph with a set of free params
  virtual void update_shock_graph(const vnl_vector<double >& params );

  //: Extract the free parameters for the internal shock graph given its vertex and
  // edge correspondences with another shock graph of the same topology
  void get_free_params(const dbsksp_shock_graph_sptr& that_graph, 
    vnl_vector<double >& params,
    const vcl_map<dbsksp_shock_edge_sptr, dbsksp_shock_edge_sptr>& edge_map,
    const vcl_map<dbsksp_shock_node_sptr, dbsksp_shock_node_sptr>& node_map);

  //: index of reference direction variable in the parameter vector
  int param_index_of_ref_dir() const;
    

protected:

};


//: A modified version of phi-m-L model, with parameters grouped together to
// have the same unit
class dbsksp_phi_m_L_model_unified_dim: public dbsksp_phi_m_L_shock_model
{
public:
  // ========================== CONSTRUCTORS / DESTRUCTORS ====================
  //: constructor
  dbsksp_phi_m_L_model_unified_dim(
    const dbsksp_shock_graph_sptr& shock_graph);

  //: destructor
  virtual ~dbsksp_phi_m_L_model_unified_dim(){};


  // ============= UTILITY =============================================
  
  //: extract parameters from shock graph
  virtual void compute_params_from_shock_graph(vnl_vector<double >& params);

  //: Update shock graph with `this' set of free params
  virtual void update_shock_graph(const vnl_vector<double >& params );

protected:
  vnl_vector<double > node_phi_list_;
  vnl_vector<double > edge_m_len_list_;
};



// ============================================================================
// dbsksp_extrinsic_info_model
// ============================================================================

//: A model of free parameters of a shock graph only modifying the extrinsic
// params (ref_point, ref_dir, and ref_radius)
class dbsksp_extrinsic_info_model: public dbsksp_shock_model
{
public:
  // ========================== CONSTRUCTORS / DESTRUCTORS ====================
  //: constructor
  dbsksp_extrinsic_info_model(const dbsksp_shock_graph_sptr& shock_graph) :
      dbsksp_shock_model(shock_graph){}

  //: destructor
  virtual ~dbsksp_extrinsic_info_model(){};


  // ============= UTILITY =============================================
  

  //; Update (compute) the number of free parameters of the model
  virtual void update_num_params()
  {this->set_num_params(4); }

  //: construct `this' vector from its shock graph
  virtual void compute_params_from_shock_graph(vnl_vector<double >& params);

  //: Update shock graph with `this' set of free params
  virtual void update_shock_graph(const vnl_vector<double >& params );

protected:

};


// ========================================================================
// dbsksp_intrinsic_phi_m_L_shock_model
// ========================================================================

//: A model of free parameters of a shock graph, keeping phi, m, edge length 
// as the free parameters
class dbsksp_intrinsic_phi_m_L_shock_model: public dbsksp_phi_m_L_shock_model
{
public:
  // ========================== CONSTRUCTORS / DESTRUCTORS ====================
  //: constructor
  dbsksp_intrinsic_phi_m_L_shock_model(const dbsksp_shock_graph_sptr& shock_graph)
    : dbsksp_phi_m_L_shock_model(shock_graph){}

  //: destructor
  virtual ~dbsksp_intrinsic_phi_m_L_shock_model(){};


  // ============= UTILITY =============================================

  //; Update (compute) the number of free parameters of the model
  virtual void update_num_params();


  //: construct `this' vector from its shock graph
  virtual void compute_params_from_shock_graph(vnl_vector<double >& params);

  //: Update shock graph with `this' set of free params
  virtual void update_shock_graph(const vnl_vector<double >& params );

protected:

};



// ============================================================================
// dbsksp_selective_phi_m_L_shock_model
// ============================================================================


//: A model of free parameters of a shock graph, keeping phi, m, edge length 
// as the free parameters
class dbsksp_selective_phi_m_L_shock_model: public dbsksp_phi_m_L_shock_model
{
public:
  // ========================== CONSTRUCTORS / DESTRUCTORS ====================
  //: constructor
  dbsksp_selective_phi_m_L_shock_model(
    const dbsksp_shock_graph_sptr& shock_graph): 
  dbsksp_phi_m_L_shock_model(shock_graph){}

  //: destructor
  virtual ~dbsksp_selective_phi_m_L_shock_model(){};


  // ============= UTILITY =============================================
  
  //: construct `this' vector from its shock graph
  virtual void compute_params_from_shock_graph(vnl_vector<double >& params);

  //: Update shock graph with `this' set of free params
  virtual void update_shock_graph(const vnl_vector<double >& params );

  //; Update (compute) the number of free parameters of the model
  // This number is typically dependent on the shock graph, among others
  // By default, the function compute the total degree of freedom of the shape
  virtual void update_num_params();

  //: get the selection vector. 1 when selected, 0 when not
  vcl_map<dbsksp_shock_edge_sptr, vnl_int_4 > params_selection() const {return this->selection_; }
  void set_params_selection(const vcl_map<dbsksp_shock_edge_sptr, vnl_int_4 >& selection )
  {this->selection_ = selection; }
  //

protected:

  // order: m, L, source_phi, target_phi
  vcl_map<dbsksp_shock_edge_sptr, vnl_int_4 > selection_;

};





#endif // shp/dbsksp/dbsksp_shock_model.h









