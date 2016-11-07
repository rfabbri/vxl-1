// This is shp/dbsksp/dbsksp_twoshapelet_function.h
#ifndef dbsksp_twoshapelet_function_h_
#define dbsksp_twoshapelet_function_h_

//:
// \file
// \brief Cost function of a shock snake
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
#include <vil/vil_image_view.h>



// ============================================================================
// dbsksp_twoshapelet_type2_cost_fnt
// ============================================================================

//: A cost function for a regular twoshapelet
class dbsksp_twoshapelet_type2_cost_fnt : public vnl_cost_function
{
public:
  // CONSTRUCTORS / DESTRUCTORS -----------------------------------------------

  //: Constructor - default
  dbsksp_twoshapelet_type2_cost_fnt() : vnl_cost_function(5){}

  //: Constructor
  dbsksp_twoshapelet_type2_cost_fnt(const dbsksp_twoshapelet_sptr& s,
    dbsksp_potential_integrator* fnt,
    double lambda);

  //: Destructor
  virtual ~dbsksp_twoshapelet_type2_cost_fnt(){};

  // DATA ACCESS --------------------------------------------------------------

  //: Get the fixed parameters of the twoshapelet
  // x_fixed = [x0, y0, psi0, r0, phi0, len]
  void extract_fixed_params(const dbsksp_twoshapelet_sptr& ss,
    double& x0, double& y0, double& psi0, 
    double& r0, double& phi0, double& len) const;

  //: Get the free parameters of the twoshapelet
  // x_free = [m0, phi1, m1, phi2, len_diff]
  vnl_vector<double > get_free_params(const dbsksp_twoshapelet_sptr& s);

  //: Get fixed parameters
  void get_fixed_params(double& x0, double& y0, double& psi0, 
    double& r0, double& phi0, double& len);

  //: Compute two shapelet from a vector of free parameters
  dbsksp_twoshapelet_sptr get_twoshapelet(vnl_vector<double > free_params);

  

  // THE MAIN FUNCTIONS -------------------------------------------------------

  //:  Compute value of f(x) given a (free) parameter vector x
  virtual double f(vnl_vector<double> const& x);

protected:

  // MEMBER VARIABLES ---------------------------------------------------------

  //: The external energy function
  dbsksp_potential_integrator* potential_integrator_;

  //: ratio between internal and external energy
  double lambda_;

  //: fixed parameters
  double x0_;
  double y0_;

  // shock direction
  double psi0_;
  double r0_;
  double phi0_;
  double len_; // sum of chord lengths
  
};







// ============================================================================
// dbsksp_twoshapelet_cost_function
// ============================================================================
//: A cost function for a regular twoshapelet
class dbsksp_twoshapelet_cost_function : public vnl_cost_function
{
public:

  // --------------------------------------------------------------------------
  // CONSTRUCTORS / DESTRUCTORS
  // --------------------------------------------------------------------------

  //: Constructor
  dbsksp_twoshapelet_cost_function(): vnl_cost_function(4){};

  //: Constructor
  dbsksp_twoshapelet_cost_function(const dbsksp_twoshapelet_sptr& s,
    dbsksp_external_energy_function* fnt,
    double internal_external_energy_ratio);

  //: Destructor
  virtual ~dbsksp_twoshapelet_cost_function(){};


  // --------------------------------------------------------------------------
  // DATA ACCESS
  // --------------------------------------------------------------------------

  //: Get and set and external energy function
  dbsksp_external_energy_function* external_energy_function()
  {return this->external_energy_function_; }

  void set_external_energy_function(dbsksp_external_energy_function* fnt)
  {this->external_energy_function_ = fnt; }

  //: Get a twoshapelet given a vector of free variables (4x1)
  // x_free = [m0, phi1, m1, phi2]
  dbsksp_twoshapelet_sptr get_twoshapelet(const vnl_vector<double >& x);

  //: Get the free parameters of the twoshapelet
  // x_free = [m0, phi1, m1, phi2]
  vnl_vector<double > get_free_params(const dbsksp_twoshapelet_sptr& s);

  //: Get vector of fixed params
  // x_fixed = [x0, y0, psi0, r0, phi0, len0, len1]
  vnl_vector<double > fixed_params() const;

  //: Set the free parameters of the twoshapelet
  // x_fixed = [x0, y0, psi0, r0, phi0, len0, len1]
  void set_fixed_params(const vnl_vector<double >& fixed_params);
  
  // --------------------------------------------------------------------------
  // THE MAIN FUNCTIONS
  // --------------------------------------------------------------------------
  
  //:  Compute value of f(x) given a (free) parameter vector x
  // x_free = [m0, phi1, m1, phi2]
  virtual double f(vnl_vector<double> const& x);

protected:

  // --------------------------------------------------------------------------
  // INTERNAL SUPPORT FUNCTIONS
  // --------------------------------------------------------------------------
  
  //: Get the fixed parameters of the twoshapelet
  // x_fixed = [x0, y0, psi0, r0, phi0, len0, len1]
  vnl_vector<double > extract_fixed_params(const dbsksp_twoshapelet_sptr& s);


  // --------------------- MEMBER VARIABLES ---------------------

  //: The external energy function
  dbsksp_external_energy_function* external_energy_function_;

  //: ratio between internal and external energy
  double lambda_;

  //: fixed parameters
  double x0_;
  double y0_;
  // shock direction
  double psi0_;
  double r0_;
  double phi0_;
  
  double len0_;
  double len1_;

  //// free variables
  //double m0_;
  //double phi1_;
  //double m1_;
  //double phi2_;
};



// ============================================================================
// dbsksp_terminal_twoshapelet_cost_fnt
// ============================================================================

//: A cost function for a terminal twoshapelet - a degenerate twoshapelet where
// one end is a A_infty fragment. All the free variables are in the (starting)
// A_1^2 fragment. There are 3 free variables (for the starting fragment):
// - chord length L
// - param_m
// - phi1
class dbsksp_terminal_twoshapelet_cost_fnt : public vnl_cost_function
{

  // CONSTRUCTORS / DESTRUCTORS -----------------------------------------------
  
  //: Constructor - default
  dbsksp_terminal_twoshapelet_cost_fnt() : vnl_cost_function(3){}

  //: Constructor
  dbsksp_terminal_twoshapelet_cost_fnt(const dbsksp_shapelet_sptr& s,
    vil_image_view<float > potential_image,
    double internal_external_energy_ratio);

  //: Destructor
  virtual ~dbsksp_terminal_twoshapelet_cost_fnt(){};


  // DATA ACCESS --------------------------------------------------------------

  //: Get the fixed parameters of the twoshapelet
  // x_fixed = [x0, y0, psi0, r0, phi0, init_len0]
  void extract_fixed_params(const dbsksp_shapelet_sptr& s,
    double& x0, double& y0, double& psi0, 
    double& r0, double& phi0, double& init_len0 ) const;

  //: Get fixed parameters
  void get_fixed_params(double& x0, double& y0, double& psi0, 
    double& r0, double& phi0, double& init_len0);

  
  //: Get the free parameters of the terminal_twoshapelet
  // x_free = [len0_multiplier, m0, phi1]
  vnl_vector<double > get_free_params(const dbsksp_shapelet_sptr& s);

  //: Get a shapelet (next to terminal_edge) given a free variable vector (3x1)
  // x_free = [len0_multiplier, m0, phi1]
  dbsksp_shapelet_sptr get_shapelet(const vnl_vector<double >& x);


  
  // THE MAIN FUNCTIONS -------------------------------------------------------
  
  //:  The main function. Given the parameter vector x, compute the value of f(x).
  // x = [len0, m0, phi1]
  virtual double f(vnl_vector<double> const& x);


protected:
  
  // --------------------------------------------------------------------------
  // MEMBER VARIABLES
  // --------------------------------------------------------------------------

  //: ratio between internal and external energy
  double lambda_;

  //: Potential field
  vil_image_view<float > potential_image_;

  //: fixed parameters
  double x0_;
  double y0_;
  // shock direction
  double psi0_;
  double r0_;
  double phi0_;

  // length of the first shapelet
  double init_len0_;

  // free variables
  // double len0_multiplier;
  //double m0_;
  //double phi1_;
};




#endif // shp/dbsksp/dbsksp_twoshapelet_function.h









