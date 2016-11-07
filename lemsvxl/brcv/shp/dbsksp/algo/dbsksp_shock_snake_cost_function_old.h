// This is shp/dbsksp/dbsksp_shock_snake_cost_function_old.h
#ifndef dbsksp_shock_snake_cost_function_old_h_
#define dbsksp_shock_snake_cost_function_old_h_

//:
// \file
// \brief Old version of dbsksp_shock_snake_cost_function, to be removed
//        
// \author Nhon Trinh ( ntrinh@lems.brown.edu)
// \date Oct 8, 2006
//
// \verbatim
//  Modifications
// \endverbatim


#include <vnl/vnl_cost_function.h>
#include <dbsksp/dbsksp_shock_graph_sptr.h>
#include <dbsksp/dbsksp_shock_edge_sptr.h>
#include <dbsksp/dbsksp_shock_node_sptr.h>
#include <dbsksp/dbsksp_shock_model.h>
#include <dbsksp/algo/dbsksp_external_energy_function.h>
#include <dbsksp/dbsksp_shapelet_sptr.h>
#include <vnl/vnl_vector.h>

//: A base class for cost functions of a shock snake
class dbsksp_shock_snake_cost_function : public vnl_cost_function
{
public:

  // ========================== CONSTRUCTORS / DESTRUCTORS ====================
  //: Constructor
  dbsksp_shock_snake_cost_function(): vnl_cost_function(){};
  
  //: Constructor
  dbsksp_shock_snake_cost_function(int number_of_unknowns):
      vnl_cost_function(number_of_unknowns){};
  
  //: Destructor
  virtual ~dbsksp_shock_snake_cost_function(){};

  // ============== DATA ACCESS ==================================== 

  //: Get and Set the free_params model
  dbsksp_shock_model* shock_model() 
  {return this->shock_model_; }
  void set_shock_model(dbsksp_shock_model* model) 
  {
    this->shock_model_ = model; 
    this->set_number_of_unknowns(model->num_params());
  }

  //: Get and set and external energy function
  dbsksp_external_energy_function* external_energy_function()
  {return this->external_energy_function_; }
  void set_external_energy_function(dbsksp_external_energy_function* fnt)
  {this->external_energy_function_ = fnt; }
  
  
  // ============= THE MAIN FUNCTION =========================================
  
  //:  The main function.  Given the parameter vector x, compute the value of f(x).
  virtual double f(vnl_vector<double> const& x);


  // ============== MEMBER VARIABLES ===================================
protected:
  dbsksp_shock_model* shock_model_;
  dbsksp_external_energy_function* external_energy_function_;
  
};






//: A base class for cost functions of a shock snake
class dbsksp_shapelet_cost_function : public vnl_cost_function
{
public:

  // --------------------- CONSTRUCTORS / DESTRUCTORS ---------------------
  //: Constructor
  dbsksp_shapelet_cost_function(): vnl_cost_function(){};
  
  //: Constructor
  dbsksp_shapelet_cost_function(vnl_vector_fixed<double, 8 > param_template, 
    vnl_vector_fixed<int, 8 > param_filter,
    dbsksp_external_energy_function* fnt):
  vnl_cost_function(param_filter.sum()),
  param_template_(param_template),
  param_filter_(param_filter),
  external_energy_function_(fnt)
  {};
  
  //: Destructor
  virtual ~dbsksp_shapelet_cost_function(){};

  // --------------------- DATA ACCESS --------------------- 

  //: Get and set and external energy function
  dbsksp_external_energy_function* external_energy_function()
  {return this->external_energy_function_; }

  void set_external_energy_function(dbsksp_external_energy_function* fnt)
  {this->external_energy_function_ = fnt; }

  //: Get and set the parameter filter
  // 0: the parameter is not subject to change
  // 1: the parameter is to be optimized (together with others)
  vnl_vector<int > param_filter() const {return this->param_filter_.as_vector(); }

  void set_param_filter(const vnl_vector<int >& filter)
  {this->param_filter_ = filter; }

  //: Get and set the parameter template
  // Part of these parameters will be changed in the optimization process
  vnl_vector<double > param_template() const {return this->param_template_.as_vector(); }

  void set_param_template(const vnl_vector<double >& params)
  {this->param_template_ = params; }

  vnl_vector<double > free_params() const;


  
  
  // --------------------- THE MAIN FUNCTION ---------------------
  
  //:  The main function.  Given the parameter vector x, compute the value of f(x).
  virtual double f(vnl_vector<double> const& x);


  // --------------------- MEMBER VARIABLES ---------------------
protected:
  vnl_vector_fixed<double, 8 > param_template_;
  vnl_vector_fixed<int, 8 > param_filter_;
  dbsksp_external_energy_function* external_energy_function_;
  
};











//: A base class for cost functions of a shock snake
class dbsksp_half_shapelet_cost_function : public vnl_cost_function
{
public:

  // --------------------- CONSTRUCTORS / DESTRUCTORS ---------------------
  //: Constructor
  dbsksp_half_shapelet_cost_function(): vnl_cost_function(){};
  
  //: Constructor
  dbsksp_half_shapelet_cost_function(const dbsksp_shock_edge_sptr& e,
    const dbsksp_shock_node_sptr& node,
    dbsksp_external_energy_function* fnt,
    double internal_external_energy_ratio);

  //: Constructor
  dbsksp_half_shapelet_cost_function(vgl_point_2d<double > start,
  double theta0,
  double phi_start,
  double radius_start,
  double chord_length,
    dbsksp_external_energy_function* fnt,
    double internal_external_energy_ratio);


  dbsksp_half_shapelet_cost_function(const vnl_vector<double >& fixed_params,
  dbsksp_external_energy_function* fnt,
  double internal_external_energy_ratio);
  
  //: Destructor
  virtual ~dbsksp_half_shapelet_cost_function(){};

  // --------------------- DATA ACCESS --------------------- 

  //: Get and set and external energy function
  dbsksp_external_energy_function* external_energy_function()
  {return this->external_energy_function_; }

  void set_external_energy_function(dbsksp_external_energy_function* fnt)
  {this->external_energy_function_ = fnt; }

  //: Get a shapelet given a vector of free variables (4x1)
  dbsksp_shapelet_sptr get_shapelet(const vnl_vector<double >& x);

  vnl_vector<double > get_fixed_params(const dbsksp_shock_edge_sptr& e,
    const dbsksp_shock_node_sptr& node);

  vnl_vector<double > get_free_params(const dbsksp_shock_edge_sptr& e,
    const dbsksp_shock_node_sptr& node);

  double chord_length() const {return this->chord_length_; }
  void set_chord_length(double len) {this->chord_length_ = len; }

  


  
  
  // --------------------- THE MAIN FUNCTION ---------------------
  
  //:  The main function.  Given the parameter vector x, compute the value of f(x).
  virtual double f(vnl_vector<double> const& x);


  // --------------------- MEMBER VARIABLES ---------------------
protected:
  dbsksp_external_energy_function* external_energy_function_;

  // ration between internal and external energy
  double lambda_;

  //: parameters extracted from the edge and node
  vgl_point_2d<double > start_;
  vgl_vector_2d<double > shock_tangent_start_;
  double phi_start_;
  double radius_start_;
  double chord_length_;
};




//: A base class for cost functions of a shock snake
class dbsksp_3_param_shapelet_cost_function : public vnl_cost_function
{
public:

  // --------------------- CONSTRUCTORS / DESTRUCTORS ---------------------
  //: Constructor
  dbsksp_3_param_shapelet_cost_function(): vnl_cost_function(){};
  
  //: Constructor
  dbsksp_3_param_shapelet_cost_function(const dbsksp_shock_edge_sptr& e,
    const dbsksp_shock_node_sptr& node,
    dbsksp_external_energy_function* fnt,
    double internal_external_energy_ratio);
  
  //: Destructor
  virtual ~dbsksp_3_param_shapelet_cost_function(){};

  // --------------------- DATA ACCESS --------------------- 

  //: Get and set and external energy function
  dbsksp_external_energy_function* external_energy_function()
  {return this->external_energy_function_; }

  void set_external_energy_function(dbsksp_external_energy_function* fnt)
  {this->external_energy_function_ = fnt; }

  //: Get a shapelet given a vector of free variables (4x1)
  dbsksp_shapelet get_shapelet(const vnl_vector<double >& x);

  vnl_vector<double > get_free_params(const dbsksp_shock_edge_sptr& e,
    const dbsksp_shock_node_sptr& node);

  // --------------------- THE MAIN FUNCTION ---------------------
  
  //:  The main function.  Given the parameter vector x, compute the value of f(x).
  virtual double f(vnl_vector<double> const& x);


  // --------------------- MEMBER VARIABLES ---------------------
protected:
  dbsksp_external_energy_function* external_energy_function_;

  // ration between internal and external energy
  double lambda_;

  //: parameters extracted from the edge and node
  vgl_point_2d<double > start_;
  vgl_vector_2d<double > shock_tangent_start_;
  double phi_start_;
  double radius_start_;
};

#endif // shp/dbsksp/dbsksp_shock_snake_cost_function_old_h









