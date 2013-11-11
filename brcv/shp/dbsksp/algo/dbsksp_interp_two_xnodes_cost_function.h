// This is shp/dbsksp/algo/dbsksp_interp_two_xnodes_cost_function_cost_function.h
#ifndef dbsksp_interp_two_xnodes_cost_function_h_
#define dbsksp_interp_two_xnodes_cost_function_h_

//:
// \file
// \brief Cost functions to interpolate between two extrinsic nodes
//        
// \author Nhon Trinh ( ntrinh@lems.brown.edu)
// \date April 30, 2007
//
// \verbatim
//  Modifications
// \endverbatim


#include <dbsksp/dbsksp_shapelet.h>
#include <dbsksp/dbsksp_xshock_node.h>
#include <dbgl/algo/dbgl_biarc.h>
#include <vnl/vnl_least_squares_function.h>
#include <vnl/vnl_cost_function.h>
#include <vnl/vnl_matrix_fixed.h>



// ============================================================================
// dbsksp_property_of_twoshapelet_estimate
// ============================================================================
class dbsksp_property_of_twoshapelet_estimate: public vnl_cost_function
{
public:
  enum property_type {
    CHORD_LENGTH_0 = 0, 
    CHORD_LENGTH_1 = 1, 
    FIT_COST = 2,
    DET_A = 3
  };

  enum estimation_type{
    IGNORE_VERTICAL_DISCREPANCY = 0,
    IGNORE_RADIUS_DISCREPANCY = 1
  };

  dbsksp_property_of_twoshapelet_estimate(
    const dbsksp_xshock_node_descriptor& start_xnode,
    const dbsksp_xshock_node_descriptor& end_xnode,
    double alpha0, 
    property_type prop_type,
    estimation_type est_type) : 
  vnl_cost_function(1), 
    start_xnode_(start_xnode),
    end_xnode_(end_xnode), 
    alpha0_(alpha0),
    prop_type_(prop_type),
    est_type_(est_type)
  {}
  virtual ~dbsksp_property_of_twoshapelet_estimate(){};

  // x has size 1 and x[0] = phi1
  double f(const vnl_vector<double >& x);
protected:
  dbsksp_xshock_node_descriptor start_xnode_;
  dbsksp_xshock_node_descriptor end_xnode_;
  double alpha0_;
  property_type prop_type_;
  estimation_type est_type_;
};



// ============================================================================
// dbsksp_interp_two_xnodes_compute_phi1_given_alpha0
// ============================================================================

// ----------------------------------------------------------------------------
// Curvature variation on the boundary of a twoshapelet
// The 1 variable to optimize: t \in [0,1], which corresponds to one alpha0
class dbsksp_interp_two_xnodes_compute_phi1_given_alpha0
{
public:
  //: constructor
  dbsksp_interp_two_xnodes_compute_phi1_given_alpha0(
    const dbsksp_xshock_node_descriptor& start_xnode,
    const dbsksp_xshock_node_descriptor& end_xnode,
    const dbgl_biarc& shock_estimate
    ) : start_xnode_(start_xnode),
    end_xnode_(end_xnode),
    shock_estimate_(shock_estimate)
  {}

  //: destructor
  virtual ~dbsksp_interp_two_xnodes_compute_phi1_given_alpha0() {};

  dbsksp_xshock_node_descriptor start_xnode() const { return this->start_xnode_; }
  dbsksp_xshock_node_descriptor end_xnode() const { return this->end_xnode_; }
  dbgl_biarc shock_estimate() const {return this->shock_estimate_; }

  //: Given alpha, optimize phi1 to fit to end_xnode by finding root of equation
  // vertical_discrepancy(phi1) = 0;
  // vertical direction is the direction orthogonal to the overall chord
  dbsksp_twoshapelet_sptr compute_twoshapelet_via_vertical_discrepancy(double alpha0);

  //: Given alpha, optimize phi1 to fit to end_xnode by finding root of equation
  // radius_discrepancy(phi1) = 0;
  dbsksp_twoshapelet_sptr compute_twoshapelet_via_radius_discrepancy(double alpha0,
    double& fit_error);

protected:
  //dbsksp_twoshapelet_sptr ss_;
  dbsksp_xshock_node_descriptor start_xnode_;
  dbsksp_xshock_node_descriptor end_xnode_;
  dbgl_biarc shock_estimate_;
};








// ============================================================================
// dbsksp_interp_two_xnodes_curvature_variation_type1_cost_fnt
// ============================================================================

// ----------------------------------------------------------------------------
// Curvature variation on the boundary of a twoshapelet
// The 1 variable to optimize: t \in [0,1], which corresponds to one alpha0
class dbsksp_interp_two_xnodes_curvature_variation_type1_cost_fnt : 
  public vnl_least_squares_function
{
public:
  //: constructor
  dbsksp_interp_two_xnodes_curvature_variation_type1_cost_fnt(
    const dbsksp_xshock_node_descriptor& start_xnode,
    const dbsksp_xshock_node_descriptor& end_xnode,
    const dbgl_biarc& shock_estimate
    );

  //: destructor
  virtual ~dbsksp_interp_two_xnodes_curvature_variation_type1_cost_fnt() {};

  dbsksp_xshock_node_descriptor start_xnode() const { return this->start_xnode_; }
  dbsksp_xshock_node_descriptor end_xnode() const { return this->end_xnode_; }
  dbgl_biarc shock_estimate() const {return this->shock_estimate_; }
  
  //: Return alpha0 given a parameter t
  double alpha0_estimate(double t);

  //: Return an estimate of phi1 given t
  double phi1_estimate(double t);
  
  //: The main function.
  //  Given the parameter vector x, compute the vector of residuals fx.
  //  Fx has been sized appropriately before the call.
  // Structure of unknown params:
  // x[0] : alpha0
  virtual void f(vnl_vector<double> const& x, vnl_vector<double>& fx);

  //: construct a new twoshapelet given a set unknown parameters
  // save fitting cost to fit_cost
  dbsksp_twoshapelet_sptr get_twoshapelet(const vnl_vector<double >& x, double& fit_error);


protected:
  //dbsksp_twoshapelet_sptr ss_;
  dbsksp_xshock_node_descriptor start_xnode_;
  dbsksp_xshock_node_descriptor end_xnode_;
  dbgl_biarc shock_estimate_;
};


// ============================================================================
// dbsksp_interp_two_xnodes_min_k_variation_type2_cost_fnt
// ============================================================================

// ----------------------------------------------------------------------------
// Curvature variation on the boundary of a twoshapelet
// The 1 variable to optimize: t \in [0,1], which corresponds to one alpha0
class dbsksp_interp_two_xnodes_min_k_variation_type2_cost_fnt : 
  public vnl_cost_function
{
public:
  //: constructor
  dbsksp_interp_two_xnodes_min_k_variation_type2_cost_fnt(
    const dbsksp_xshock_node_descriptor& start_xnode,
    const dbsksp_xshock_node_descriptor& end_xnode,
    const dbgl_biarc& shock_estimate
    );

  //: destructor
  virtual ~dbsksp_interp_two_xnodes_min_k_variation_type2_cost_fnt() {};

  dbsksp_xshock_node_descriptor start_xnode() const { return this->start_xnode_; }
  dbsksp_xshock_node_descriptor end_xnode() const { return this->end_xnode_; }
  dbgl_biarc shock_estimate() const {return this->shock_estimate_; }

  //: Return alpha0 given a parameter t
  double alpha0_estimate(double t);

  
  //: The main function.
  //  Given the parameter vector x, compute cost
  virtual double f(vnl_vector<double> const& x);

  //: construct a new twoshapelet given a set unknown parameters
  // save fitting cost to fit_cost
  dbsksp_twoshapelet_sptr get_twoshapelet(const vnl_vector<double >& x, double& fit_error);

  dbsksp_twoshapelet_sptr compute_twoshapelet_via_radius_discrepancy(double alpha0,
    double& fit_error);



protected:
  dbsksp_xshock_node_descriptor start_xnode_;
  dbsksp_xshock_node_descriptor end_xnode_;
  dbgl_biarc shock_estimate_;
};







#endif // shp/dbsksp/dbsksp_interp_two_xnodes_cost_function.h









