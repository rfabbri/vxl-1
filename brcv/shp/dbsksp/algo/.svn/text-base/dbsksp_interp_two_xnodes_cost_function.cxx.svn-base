// This is file shp/dbsksp/dbsksp_interp_two_xnodes_cost_function.cxx

//:
// \file

#include <vcl_fstream.h>
#include "dbsksp_interp_two_xnodes_cost_function.h"
#include <vgl/vgl_distance.h>
#include <vnl/vnl_math.h>
#include <vnl/algo/vnl_levenberg_marquardt.h>
#include <dbnl/algo/dbnl_brent_root.h>
#include <dbsksp/algo/dbsksp_twoshapelet_estimator.h>




//: Return 1 if x is positive
// Otherwise, return sinh(x) ( sinh(0) = 1 and sinh'(x) = 0)
double dbsksp_interp_two_xnodes_f0(double x)
{
  return (x >=0) ? 1 : 2 / (vcl_exp(x) + vcl_exp(-x));
}





// ============================================================================
// dbsksp_property_of_twoshapelet_estimate
// ============================================================================

// -------------------------------------------------------------------------
double dbsksp_property_of_twoshapelet_estimate::
f(const vnl_vector<double >& x)
{
  double phi1 = x[0];
  //dbsksp_twoshapelet_sptr ss = 
    //dbsksp_estimate_twoshapelet(this->start_xnode_, this->end_xnode_, this->alpha0_, phi1);

  dbsksp_twoshapelet_estimator g;
  switch (this->est_type_)
  {
  case IGNORE_VERTICAL_DISCREPANCY:
    {
      g.compute(this->start_xnode_, this->end_xnode_, this->alpha0_, phi1, 
        dbsksp_twoshapelet_estimator::IGNORE_VERTICAL_DISCREPANCY);
      break;
    }
  case IGNORE_RADIUS_DISCREPANCY:
    {
      g.compute(this->start_xnode_, this->end_xnode_, this->alpha0_, phi1, 
        dbsksp_twoshapelet_estimator::IGNORE_RADIUS_DISCREPANCY);
      break;
    }
  default:
    {
      vcl_cerr << "Unknown estimation type\n";
      assert(false);
    }
  }


  dbsksp_twoshapelet_sptr ss = g.twoshapelet();
  switch (this->prop_type_)
  {
  case CHORD_LENGTH_0:
    return ss->len0();
  case CHORD_LENGTH_1:
    return ss->len1();
  case DET_A:
    {
      return g.det_A();
    }
  case FIT_COST:
    {
      return g.discrepancy_of_ignored_param();
    };
  default:
    vcl_cerr << "Wrong property type.\n";
    return vnl_numeric_traits<double >::maxval;
  }
};








// ============================================================================
// dbsksp_interp_two_xnodes_compute_phi1_given_alpha0
// ============================================================================

// ----------------------------------------------------------------------------
//: Given alpha, optimize phi1 to fit to end_xnode by finding root of equation
// vertical_discrepancy(phi1) = 0;
dbsksp_twoshapelet_sptr dbsksp_interp_two_xnodes_compute_phi1_given_alpha0::
compute_twoshapelet_via_vertical_discrepancy(double alpha0)
{
  
  // First determine the upper and low bounds for phi1, which are the limit when
  // one of the chord length reaches 0;
  double phi1_bound_tol = 1e-3;

  // Compute the lower of the range of phi1
  // determine bound on phi1 using len0
  double phi1_range_min = 10;
  dbsksp_property_of_twoshapelet_estimate detA_fnt(
    this->start_xnode(), this->end_xnode(), 
    alpha0, dbsksp_property_of_twoshapelet_estimate::DET_A,
    dbsksp_property_of_twoshapelet_estimate::IGNORE_VERTICAL_DISCREPANCY);
  dbnl_brent_root phi1_range_min_solver(detA_fnt, phi1_bound_tol);

  phi1_range_min_solver.solve(0.01*vnl_math::pi, 0.99*vnl_math::pi,
    phi1_range_min);

  // incremrent a little bit to stay out the unstable zone
  phi1_range_min += 2*phi1_bound_tol;
  
  //double phi1_range_min = 0.1 * vnl_math::pi;
  double phi1_range_max = vnl_math::pi - 5*phi1_bound_tol; //0.95 * vnl_math::pi;
  
  // determine bound on phi1 using len0
  dbsksp_property_of_twoshapelet_estimate len0_fnt(
    this->start_xnode(), this->end_xnode(), 
    alpha0, dbsksp_property_of_twoshapelet_estimate::CHORD_LENGTH_0,
    dbsksp_property_of_twoshapelet_estimate::IGNORE_VERTICAL_DISCREPANCY);
  dbnl_brent_root phi1_bound0_solver(len0_fnt, phi1_bound_tol);

  // len0 should varying monotonically as phi1 changes from 0 to pi
  double phi1_bound0 = -1;
  if (phi1_bound0_solver.f(phi1_range_min) * phi1_bound0_solver.f(phi1_range_max) < 0)
  {
    phi1_bound0_solver.solve(phi1_range_min, phi1_range_max, phi1_bound0);
  }
  else
  {
    if (phi1_bound0_solver.f(phi1_range_min) < 0)
      return 0;
    else
      // the whole phi1_range is valid, set to min
      phi1_bound0 = phi1_range_min;
  }
  
  dbsksp_property_of_twoshapelet_estimate len1_fnt(
    this->start_xnode(), this->end_xnode(), 
    alpha0, dbsksp_property_of_twoshapelet_estimate::CHORD_LENGTH_1,
    dbsksp_property_of_twoshapelet_estimate::IGNORE_VERTICAL_DISCREPANCY);
  dbnl_brent_root phi1_bound1_solver(len1_fnt, phi1_bound_tol);

  // len1 should varying monotonically, in an opposite direction from
  // len0, as phi1 changes from 0 to pi
  double phi1_bound1 = -1;
  if (phi1_bound1_solver.f(phi1_range_min) * phi1_bound1_solver.f(phi1_range_max) < 0)
  {
    phi1_bound1_solver.solve(phi1_range_min, phi1_range_max, phi1_bound1);
  }
  else
  {
    if (phi1_bound1_solver.f(phi1_range_min) < 0)
      return 0;
    else
      // the whole phi1_range is valid, 
      // set the bound to max, combined with phi1_bound0 possibly
      // set to min, we can a full range
      phi1_bound1 = phi1_range_max; 
  }

  // Check if there exist a valid region
  double phi1_average = (phi1_bound0 + phi1_bound1) / 2;
  if ( (phi1_bound0_solver.f(phi1_average) <= 0) || 
    (phi1_bound1_solver.f(phi1_average) <= 0) )
    return 0;

 
  // Now we've got the upper and lower bounds of phi1, experimentally, the fitting cost
  // to the two extrinsic nodes (sine of angle between two chord vectors) varies 
  // monotonically
  // If the fitting_error at the two bounds have the same sign then we can never get it down
  // to zero. Fitting is not possible.

  // fitting cost function
  dbsksp_property_of_twoshapelet_estimate err_fnt(
    this->start_xnode(), this->end_xnode(), 
    alpha0, dbsksp_property_of_twoshapelet_estimate::FIT_COST,
    dbsksp_property_of_twoshapelet_estimate::IGNORE_VERTICAL_DISCREPANCY);
  dbnl_brent_root xnode_fit_solver(err_fnt, 1e-4);

  // shrink the upper and lower bounds a little bit to make sure they're in bound
  double ax = phi1_bound0 + vnl_math_sgn(phi1_bound1 - phi1_bound0) * 2*phi1_bound_tol;
  double bx = phi1_bound1 - vnl_math_sgn(phi1_bound1 - phi1_bound0) * 2*phi1_bound_tol;

    // solve for the root using brent's method
  double phi1_root;
  dbsksp_twoshapelet_sptr ss;
  if (xnode_fit_solver.solve(ax, bx, phi1_root))
  {
    //ss = dbsksp_estimate_twoshapelet(this->start_xnode(), this->end_xnode(), 
    //alpha0, phi1_root);
    dbsksp_twoshapelet_estimator g;
    ss = g.compute(this->start_xnode(), this->end_xnode(), 
      alpha0, phi1_root, 
      dbsksp_twoshapelet_estimator::IGNORE_VERTICAL_DISCREPANCY);
  }
  else
  {
    return 0;
  }

  // DEBUG /////////////////////////////////////////////////////////
  bool print_debug_table = false;

  if (print_debug_table)
  {
    vcl_string filename = "D:/vision/temp/fitcost.txt";
    vnl_matrix<double > fitcost(200, 5, 0); // x fitcost len0 len1
    for (unsigned j=0; j<200; ++j)
    {
      double x = vnl_math::pi/200 * j;
      fitcost(j, 0) = x;
      fitcost(j, 1) = xnode_fit_solver.f(x);

      dbsksp_twoshapelet_estimator estimator;
      dbsksp_twoshapelet_sptr ss0 = estimator.compute(
        this->start_xnode(), this->end_xnode(), 
        alpha0, x,
        dbsksp_twoshapelet_estimator::IGNORE_VERTICAL_DISCREPANCY);
      //ss0 = dbsksp_estimate_twoshapelet(this->start_xnode(), this->end_xnode(), 
        //alpha0, x);
      fitcost(j, 2) = ss0->len0();
      fitcost(j, 3) = ss0->len1();
      fitcost(j, 4) = estimator.det_A();
    }
    vcl_ofstream outstr(filename.c_str(), vcl_ios_out);
    outstr << "x fitcost len0 len1 det_A\n";
    outstr << fitcost;
    outstr.close(); 
  }
  ////////////////////////////////////////////////////////////////

  return ss;
}



// ----------------------------------------------------------------------------
//: Given alpha, optimize phi1 to fit to end_xnode by finding root of equation
// radius_discrepancy(phi1) = 0
dbsksp_twoshapelet_sptr dbsksp_interp_two_xnodes_compute_phi1_given_alpha0::
compute_twoshapelet_via_radius_discrepancy(double alpha0, double& fit_error)
{
  double phi1_bound_tol = 1e-4;
  double phi1_bound0 = 0.01 * vnl_math::pi;
  double phi1_bound1 = 0.99 * vnl_math::pi;
 
  // With the upper and lower bounds of phi1, experimentally, the fitting cost
  // to the two extrinsic nodes (sine of angle between two chord vectors) varies 
  // monotonically
  // If the fitting_error at the two bounds have the same sign then we can never get it down
  // to zero. Fitting is not possible.

  // fitting cost function
  dbsksp_property_of_twoshapelet_estimate err_fnt(
    this->start_xnode(), this->end_xnode(), 
    alpha0, dbsksp_property_of_twoshapelet_estimate::FIT_COST,
    dbsksp_property_of_twoshapelet_estimate::IGNORE_RADIUS_DISCREPANCY);
  dbnl_brent_root xnode_fit_solver(err_fnt, 1e-4);

  // shrink the upper and lower bounds a little bit to make sure they're in bound
  double ax = phi1_bound0 + vnl_math_sgn(phi1_bound1 - phi1_bound0) * 2*phi1_bound_tol;
  double bx = phi1_bound1 - vnl_math_sgn(phi1_bound1 - phi1_bound0) * 2*phi1_bound_tol;

    // solve for the root using brent's method
  double phi1_root;
  dbsksp_twoshapelet_sptr ss;
  if (xnode_fit_solver.solve(ax, bx, phi1_root))
  {
    //ss = dbsksp_estimate_twoshapelet(this->start_xnode(), this->end_xnode(), 
    //alpha0, phi1_root);
    dbsksp_twoshapelet_estimator g;
    ss = g.compute(this->start_xnode(), this->end_xnode(), 
      alpha0, phi1_root,
      dbsksp_twoshapelet_estimator::IGNORE_RADIUS_DISCREPANCY);
    fit_error = 0;
    return ss;
  }
  else
  {
    // the error should be monotonic. We take the one closer to zero
    double f_ax = xnode_fit_solver.f(ax);
    double f_bx = xnode_fit_solver.f(bx);
    fit_error = (vnl_math_abs(f_ax) < vnl_math_abs(f_bx)) ?  f_ax : f_bx;
    phi1_root = (vnl_math_abs(f_ax) < vnl_math_abs(f_bx)) ?  ax : bx;
    
    dbsksp_twoshapelet_estimator g;
    ss = g.compute(this->start_xnode(), this->end_xnode(), 
      alpha0, phi1_root,
      dbsksp_twoshapelet_estimator::IGNORE_RADIUS_DISCREPANCY);

    return ss;
  }

  // DEBUG /////////////////////////////////////////////////////////
  bool print_debug_table = false;

  if (print_debug_table)
  {
    vcl_string filename = "D:/vision/temp/fitcost.txt";
    vnl_matrix<double > fitcost(200, 5, 0); // x fitcost len0 len1
    for (unsigned j=1; j<200; ++j)
    {
      double x = vnl_math::pi/200 * j;
      fitcost(j, 0) = x;
      fitcost(j, 1) = xnode_fit_solver.f(x);

      dbsksp_twoshapelet_estimator estimator;
      dbsksp_twoshapelet_sptr ss0 = estimator.compute(
        this->start_xnode(), this->end_xnode(), 
        alpha0, x, 
        dbsksp_twoshapelet_estimator::IGNORE_RADIUS_DISCREPANCY);
      //ss0 = dbsksp_estimate_twoshapelet(this->start_xnode(), this->end_xnode(), 
        //alpha0, x);
      fitcost(j, 2) = ss0->len0();
      fitcost(j, 3) = ss0->len1();
      fitcost(j, 4) = estimator.det_A();
    }
    vcl_ofstream outstr(filename.c_str(), vcl_ios_out);
    outstr << "x fitcost len0 len1 det_A\n";
    outstr << fitcost;
    outstr.close(); 
  }
  ////////////////////////////////////////////////////////////////

  return ss;
}



// ============================================================================
// dbsksp_interp_two_xnodes_curvature_variation_type1_cost_fnt
// ============================================================================

// ----------------------------------------------------------------------------
//: constructor
// dimension 3 of the cost is the fitting cost (should be zero when fitting works
dbsksp_interp_two_xnodes_curvature_variation_type1_cost_fnt::
dbsksp_interp_two_xnodes_curvature_variation_type1_cost_fnt(
  const dbsksp_xshock_node_descriptor& start_xnode,
  const dbsksp_xshock_node_descriptor& end_xnode,
  const dbgl_biarc& shock_estimate) :
vnl_least_squares_function(1, 3, no_gradient),
start_xnode_(start_xnode), end_xnode_(end_xnode),
shock_estimate_(shock_estimate)
{
  
}


// ----------------------------------------------------------------------------
//: Return alpha0 given a parameter t
double dbsksp_interp_two_xnodes_curvature_variation_type1_cost_fnt::
alpha0_estimate(double t)
{
  assert(t > 0 && t < 1);

  vgl_point_2d<double > pt = 
    this->shock_estimate_.point_at(t * this->shock_estimate_.len());
  
  // compute alpha0 given t
  vgl_vector_2d<double > tA(vcl_cos(this->start_xnode().psi_), 
    vcl_sin(this->start_xnode().psi_));
  vgl_vector_2d<double > vAC = pt - this->start_xnode().pt_;
  return signed_angle(vAC, tA);
}


//: Return an estimate of phi1 given t
double dbsksp_interp_two_xnodes_curvature_variation_type1_cost_fnt::
phi1_estimate(double t)
{
  // linearly interpolate phi between phi0 and phi2
  //double phi1 = (1-t) * this->start_xnode().phi_ + t * this->end_xnode().phi_;

  //t = 0.5;
  double phi1 = (1-t) * this->start_xnode().phi_ + t * this->end_xnode().phi_;
  return phi1;
}



// ----------------------------------------------------------------------------
//: The main function.
//  Given the parameter vector x, compute the vector of residuals fx.
//  Fx has been sized appropriately before the call.
// Structure of unknown params:
// x[0] : alpha0
void dbsksp_interp_two_xnodes_curvature_variation_type1_cost_fnt::
f(vnl_vector<double> const& x, vnl_vector<double>& fx)
{
  double fit_cost;
  // limit search two ``balanced'' twoshapelet
  if (x[0] < 0.1 || x[0] > 0.9) 
  {
    fx[0] = 1e12;
    fx[1] = 1e12;
    fx[2] = 1e12;
    return;
  }

  dbsksp_twoshapelet_sptr ss = this->get_twoshapelet(x, fit_cost);
  if (!ss)
  {
    fx[0] = 1e12;
    fx[1] = 1e12;
    fx[2] = 1e12;
    return;
  }

  // balanced length
  double length_ratio = ss->len0() / (ss->len0() + ss->len1());
  if (length_ratio < 0.1 || length_ratio > 0.9)
  {
    fx[0] = 1e12;
    fx[1] = 1e12;
    fx[2] = 1e12;
    return;
  }

  dbsksp_shapelet_sptr s0 = ss->shapelet_start();
  dbsksp_shapelet_sptr s1 = ss->shapelet_end();
  fx[0] = s0->bnd_arc_left().k() - s1->bnd_arc_left().k();
  fx[1] = s0->bnd_arc_right().k() - s1->bnd_arc_right().k();

  // fitting cost of the twoshapelet (should be zero when fitting works)
  dbsksp_shapelet_sptr se = ss->shapelet_end();
  vgl_point_2d<double > shock_pt = se->end();
  vgl_point_2d<double > left_pt = se->bnd_arc_left().point2();
  vgl_point_2d<double > right_pt = se->bnd_arc_right().point2();
  // distance between shock points
  double a = vgl_distance(shock_pt, this->end_xnode_.pt_);

  // distance between left boundary points
  double b = vgl_distance(left_pt, this->end_xnode_.bnd_pt_left());

  // distance between right boundary points
  double c = vgl_distance(right_pt, this->end_xnode_.bnd_pt_right());

  // penalty for having "negative shock chord length"
  double d = vnl_math_abs(ss->len0())-ss->len0() + vnl_math_abs(ss->len1()) - ss->len1();
  fx[2] = vnl_math_max( vnl_math_max(a,b), vnl_math_max(c,d));
  

  // penalty for legality of the twoshapelet
  if (!ss->is_legal())
  {
    fx[2] += vnl_math_abs(ss->len0()) + vnl_math_abs(ss->len1());
  }

  return;
  
  
}



// ----------------------------------------------------------------------------
//: construct a new twoshapelet given a set unknown parameters
dbsksp_twoshapelet_sptr dbsksp_interp_two_xnodes_curvature_variation_type1_cost_fnt::
get_twoshapelet(const vnl_vector<double >& x, double& fit_error)
{
  
  fit_error = 0;

  double t = x[0];
  if (t <=0.0 || t>=1.0)
  {
    return 0;
  }
  
  double alpha0 = this->alpha0_estimate(t);

  dbsksp_interp_two_xnodes_compute_phi1_given_alpha0 g(this->start_xnode(), 
    this->end_xnode(), this->shock_estimate());

  //return this->compute_twoshapelet_via_vertical_discrepancy(alpha0);
  dbsksp_twoshapelet_sptr ss = 
    g.compute_twoshapelet_via_radius_discrepancy(alpha0, fit_error);
  return ss;
}



// ============================================================================
// dbsksp_interp_two_xnodes_min_k_variation_type2_cost_fnt
// ============================================================================

// ----------------------------------------------------------------------------
//: constructor
dbsksp_interp_two_xnodes_min_k_variation_type2_cost_fnt::
dbsksp_interp_two_xnodes_min_k_variation_type2_cost_fnt(
  const dbsksp_xshock_node_descriptor& start_xnode,
  const dbsksp_xshock_node_descriptor& end_xnode,
  const dbgl_biarc& shock_estimate) : vnl_cost_function(1),
start_xnode_(start_xnode),
end_xnode_(end_xnode),
shock_estimate_(shock_estimate)
{
}


//: Return alpha0 given a parameter t
double dbsksp_interp_two_xnodes_min_k_variation_type2_cost_fnt::
alpha0_estimate(double t)
{
  assert(t > 0 && t < 1);

  vgl_point_2d<double > pt = 
    this->shock_estimate_.point_at(t * this->shock_estimate_.len());
  
  // compute alpha0 given t
  vgl_vector_2d<double > tA(vcl_cos(this->start_xnode().psi_), 
    vcl_sin(this->start_xnode().psi_));
  vgl_vector_2d<double > vAC = pt - this->start_xnode().pt_;
  return signed_angle(vAC, tA);
}

  

// ----------------------------------------------------------------------------
//: The main function.
//  Given the parameter vector x, compute cost
double dbsksp_interp_two_xnodes_min_k_variation_type2_cost_fnt::
f(vnl_vector<double> const& x)
{
  double t = x[0];
  double t_in_range_cost = 1;
  double fitting_cost = 1;
  double legality_cost = 10;
  double kdiff_cost = 1;
  double t_low = 0.05;
  double t_high = 0.95;
  
  if (t >= t_low && t <= t_high)
  {
    t_in_range_cost = 0;
    double fit_error = 1e100;
    dbsksp_twoshapelet_sptr ss = this->get_twoshapelet(x, fit_error);
    if (ss)
    {
      double len = (this->end_xnode().pt_ - this->start_xnode().pt_).length();

      // fitting cost: do end point radius match?
      //double fitting_cost = 1 - vcl_exp(- vnl_math_sqr(fit_error/10));
      fitting_cost = 1 - dbsksp_interp_two_xnodes_f0(fit_error/10);

      // legality: how illegal is the twoshapelet?
      ////
      //double legality_cost = 1;
      //if (fit_error == 0)
      //{
      //  double a = ss->len0() * ss->len1() / (len*len);
      //  a = (a>0) ? 0 : a;
      //  legality_cost = 1 - vcl_exp(a/10);
      //}

      
      if (fit_error == 0)
      {
        vnl_vector<double > a0 = ss->shapelet_start()->legality_measure();
        vnl_vector<double > a1 = ss->shapelet_end()->legality_measure();

        // recompute legality cost
        legality_cost = 0;
        for (unsigned i=0; i<a0.size(); ++i)
        {
          legality_cost += 1 - dbsksp_interp_two_xnodes_f0(a0(i));
          legality_cost += 1 - dbsksp_interp_two_xnodes_f0(a1(i));
        }

        // how large is the curvature variation?
        if (ss->is_legal())
        {
          dbsksp_shapelet_sptr s0 = ss->shapelet_start();
          dbsksp_shapelet_sptr s1 = ss->shapelet_end();
          double kdiff0 = s0->bnd_arc_left().k() - s1->bnd_arc_left().k();
          double kdiff1 = s0->bnd_arc_right().k() - s1->bnd_arc_right().k();
          kdiff_cost = 1 - vcl_exp(- (kdiff0 * kdiff0 + kdiff1 * kdiff1) * len);
        }
      }
    }
  }
  else
  {
    if (t < t_low)
      t_in_range_cost = 1 - dbsksp_interp_two_xnodes_f0(t - t_low);
    else
      t_in_range_cost = 1 - dbsksp_interp_two_xnodes_f0(t_high-t);

  
  }
  
  double total_cost = t_in_range_cost + fitting_cost + legality_cost + kdiff_cost;
  return total_cost;
}


// ----------------------------------------------------------------------------
//: construct a new twoshapelet given a set unknown parameters
// save fitting cost to fit_cost
dbsksp_twoshapelet_sptr dbsksp_interp_two_xnodes_min_k_variation_type2_cost_fnt::
get_twoshapelet(const vnl_vector<double >& x, double& fit_error)
{
  double alpha0 = this->alpha0_estimate(x[0]);
  dbsksp_interp_two_xnodes_compute_phi1_given_alpha0 g(this->start_xnode(),
    this->end_xnode(), this->shock_estimate());
  return g.compute_twoshapelet_via_radius_discrepancy(alpha0, fit_error);
}












