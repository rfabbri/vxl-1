// This is file shp/dbsksp/dbsksp_interp_two_xnodes.cxx

//:
// \file

#include "dbsksp_interp_two_xnodes.h"

#include <vcl_fstream.h>
#include <vgl/vgl_distance.h>
#include <dbsksp/algo/dbsksp_interp_two_xnodes_cost_function.h>
#include <vnl/algo/vnl_levenberg_marquardt.h>
#include <vnl/vnl_random.h>
#include <vnl/vnl_math.h>
#include <vgl/vgl_line_2d.h>
#include <vnl/algo/vnl_brent_minimizer.h>
#include <dbsksp/algo/dbsksp_twoshapelet_estimator.h>






// ============================================================================
// dbsksp_optimal_interp_2_extrinsic_nodes
// ============================================================================

//: Constructor
dbsksp_optimal_interp_two_xnodes::
dbsksp_optimal_interp_two_xnodes(
  const dbsksp_xshock_node_descriptor& start_xnode,
  const dbsksp_xshock_node_descriptor& end_xnode)
{
  this->set(start_xnode, end_xnode);
  this->shock_estimate_ = 
    dbgl_biarc(start_xnode.pt_, start_xnode.psi_,  
    end_xnode.pt_, end_xnode.psi_);

  vgl_vector_2d<double > x_axis(1, 0);
  
  // estimate the left and right boundary using biarcs
  this->left_bnd_estimate_ = 
    dbgl_biarc(start_xnode.bnd_pt_left(), 
    signed_angle(x_axis, start_xnode.bnd_tangent_left()),
    end_xnode.bnd_pt_left(), 
    signed_angle(x_axis, end_xnode.bnd_tangent_left()) );

  this->right_bnd_estimate_ = 
    dbgl_biarc(start_xnode.bnd_pt_right(), 
    signed_angle(x_axis, start_xnode.bnd_tangent_right()),
    end_xnode.bnd_pt_right(), 
    signed_angle(x_axis, end_xnode.bnd_tangent_right()) );
}




// ----------------------------------------------------------------------------
//: set the two extrinsic nodes
void dbsksp_optimal_interp_two_xnodes::
set(const dbsksp_xshock_node_descriptor& start_xnode,
    const dbsksp_xshock_node_descriptor& end_xnode)
{
  this->start_xnode_ = start_xnode;
  this->end_xnode_ = end_xnode;
}


// --------------------------------------------------------------------------
//: optimize to fit the extrinsic nodes
dbsksp_twoshapelet_sptr dbsksp_optimal_interp_two_xnodes::
optimize()
{
  // There are two cases
  // case 1: the estimated shock curve is not a straight line
  // the fitted twoshaplet will not be symmetric
  // we can optimze over alph0. For each alpha0, there is only one phi1 that can fit
  // Case 2: there estimated shock curve is a straightline,
  // the fitted twoshapelet will be symmetric
  // In this case, for all values of t along the estimated shock curve, there is only one
  /// alpha0, alpha0 = 0. And there are infinitely many phi1 that can fit to the start 
  // and ending xnode
  // We need to optimize over phi1

  // case : optimize over alpha0
  dbsksp_twoshapelet_sptr ss = 0;
  if (vnl_math::abs(this->shock_estimate().k1()) < 1e-12 &&
    vnl_math::abs(this->shock_estimate().k2()) < 1e-12 )
  {
    ss = this->optimize_over_phi1();
    if (ss && ss->is_legal()) return ss;
  }

  // if not applicable or no success with optimization over alpha0
  // use the regular case method
  //return this->optimize_over_alpha0();
  return this->optimize_over_alpha0_method_2();
}



// ----------------------------------------------------------------------------
//: General case optimize over alpha0
dbsksp_twoshapelet_sptr dbsksp_optimal_interp_two_xnodes::
optimize_over_alpha0_method_1()
{
  dbsksp_interp_two_xnodes_curvature_variation_type1_cost_fnt kp_cost(
    this->start_xnode_, this->end_xnode_,
    this->shock_estimate_);
  vnl_vector<double > x_init(1, 0.5);

  // get a good initialization
  dbsksp_twoshapelet_sptr ss_init = 0;


  // TODO:
  // optimization to search for a good initialization
  // by using two cost function and LM with two cost function
  // one penalize non-existence and one penalizes illegal shapes

  vnl_random random(9667566);
  for (unsigned i=0; i<100; ++i)
  {
    double t = random.drand32(0.05, 0.95);
    x_init[0] = t;
    double fit_error;
    ss_init = kp_cost.get_twoshapelet(x_init, fit_error);
    if (ss_init && ss_init->is_legal())
      break;
  }

  // DEBUG /////////////////////////////////////////////////////////
  if (!ss_init) 
  {
    bool print_debug_table = false;
    if (print_debug_table)
    {
      vcl_string filename = "D:/vision/temp/t_fitcost.txt";
      vnl_matrix<double > fitcost(200, 5, 0); // x fitcost len0 len1
      for (unsigned j=1; j<200; ++j)
      {
   
        double t = j/200.0;
        x_init[0] = t;
        fitcost(j, 0) = t;
        fitcost(j, 4) = kp_cost.alpha0_estimate(t);

        double fit_error;
        ss_init = kp_cost.get_twoshapelet(x_init, fit_error);
        if (ss_init)
        {
          fitcost(j, 1) = 
            ss_init->shapelet_end()->radius_end() - this->end_xnode_.radius_;
          fitcost(j, 2) = ss_init->len0();
          fitcost(j, 3) = ss_init->len1();
          
        }
      }
      vcl_ofstream outstr(filename.c_str(), vcl_ios_out);
      outstr << "x fitcost len0 len1 det_A\n";
      outstr << fitcost;
      outstr.close(); 
    }
    //return 0;
  }
  ////////////////////////////////////////////////////////////////


  vnl_vector<double> x = x_init;
  vnl_levenberg_marquardt lm(kp_cost);
  lm.minimize(x);

  //lm.diagnose_outcome();
  this->start_error_ = lm.get_start_error();
  this->end_error_ = lm.get_end_error();

  double fit_error;
  return kp_cost.get_twoshapelet(x, fit_error);
}






// ----------------------------------------------------------------------------
//: General case optimize over alpha0 - method 2
dbsksp_twoshapelet_sptr dbsksp_optimal_interp_two_xnodes::
optimize_over_alpha0_method_2()
{

  dbsksp_interp_two_xnodes_min_k_variation_type2_cost_fnt kp_cost(
    this->start_xnode_, this->end_xnode_,
    this->shock_estimate_);
  
  double x_init = 0.5;
  vnl_brent_minimizer brent_minimizer(kp_cost);

  double x = brent_minimizer.minimize(x_init);
  double fit_error;
  vnl_vector<double > xv(1, x);
  dbsksp_twoshapelet_sptr ss = kp_cost.get_twoshapelet(xv, fit_error);

  if (!ss || !ss->is_legal())
  {
    // start from somewhere else randomly
    vnl_random random(9667566);
    for (unsigned i=0; i<10; ++i)
    {
      double t = random.drand32(0.05, 0.95);
      double err_temp;
      t = brent_minimizer.minimize(t);
      vnl_vector<double > tv(1, t);
      dbsksp_twoshapelet_sptr ss_temp = kp_cost.get_twoshapelet(tv, err_temp);
      if (ss_temp && ss_temp->is_legal())
      {
        ss = ss_temp;
        break;
      }
    }
  }

  return ss;

  
  // DEBUG /////////////////////////////////////////////////////////
  bool print_debug_table = false;

  if (print_debug_table)
  {
    vcl_string filename = "D:/vision/temp/t_fitcost.txt";
    vnl_matrix<double > fitcost(200, 5, 0); // x fitcost len0 len1
    for (unsigned j=1; j<200; ++j)
    {
      double t = j/200.0;
      vnl_vector<double > y (1, t);
      fitcost(j, 0) = t;
      fitcost(j, 4) = kp_cost.alpha0_estimate(t);

      double fit_error;
      dbsksp_twoshapelet_sptr ss_init = kp_cost.get_twoshapelet(y, fit_error);
      if (ss_init)
      {
        fitcost(j, 1) = 
          ss_init->shapelet_end()->radius_end() - this->end_xnode_.radius_;
        fitcost(j, 2) = ss_init->len0();
        fitcost(j, 3) = ss_init->len1();
      }
    }
    vcl_ofstream outstr(filename.c_str(), vcl_ios_out);
    outstr << "x fitcost len0 len1 det_A\n";
    outstr << fitcost;
    outstr.close(); 
  }
  
  return ss;
  ////////////////////////////////////////////////////////////////
}



// ----------------------------------------------------------------------------
//: Special case: alpha0 = 0 constant, optimize over phi1
dbsksp_twoshapelet_sptr dbsksp_optimal_interp_two_xnodes::
optimize_over_phi1()
{
  // The estimated biars are optimized for curvature variation.
  double phi0 = this->start_xnode_.phi_;
  double phi2 = this->end_xnode_.phi_;
  double phi1 = phi0 + 
    this->left_bnd_estimate().len1() * this->left_bnd_estimate().k1();

  // Find the intersection of the shock chord and the contact shock at the midpoint
  // of the biarc
  vgl_point_2d<double > left_mid_pt = this->left_bnd_estimate_.mid_pt();

  // contact shock at mid point
  vgl_vector_2d<double > lef_mid_pt_normal = rotated(
    this->left_bnd_estimate_.tangent_at(this->left_bnd_estimate_.len1()), 
    vnl_math::pi_over_2);
  vgl_line_2d<double > left_mid_pt_contact(left_mid_pt, 
    left_mid_pt + 1 * lef_mid_pt_normal);


  // shock chord
  vgl_line_2d<double > shock_line(this->start_xnode_.pt_, this->end_xnode_.pt_);
  
  // shock point
  vgl_point_2d<double > shock_mid_pt(shock_line, left_mid_pt_contact);

  // determine len0 and len1 for the twoshapelet
  vgl_vector_2d<double > v0 = normalized(this->end_xnode_.pt_ - this->start_xnode_.pt_);
  double len0 = dot_product(shock_mid_pt - this->start_xnode_.pt_, v0);
  double len1 = dot_product(this->end_xnode_.pt_ - shock_mid_pt , v0);

  // 
  double x0 = this->start_xnode_.pt_.x();
  double y0 = this->start_xnode_.pt_.y();
  double theta0 = vcl_atan2(v0.y(), v0.x());
  double r0 = this->start_xnode_.radius_;
  double m0 = 0;
  double m1 = 0;
  return new dbsksp_twoshapelet(x0, y0, theta0, r0, phi0, m0, len0, phi1, m1, len1, phi2);
}



// --------------------------------------------------------------------------------
//: Local optimum (fit) to the end conditions given a parameter t \in [0..1]
// Each t corresponds to an initial angle theta (the reverse is not true)
// t represents the location of the initial 
// shock-meet-point along the estimated biarc for the shock
// the biarc naturally limits the range of theta.
// As t varies from 0 to 1, theta varies inside that range
dbsksp_twoshapelet_sptr dbsksp_optimal_interp_two_xnodes::
init(double t)
{
  //
  dbsksp_interp_two_xnodes_curvature_variation_type1_cost_fnt kp_cost(
    this->start_xnode_, this->end_xnode_,
    this->shock_estimate_);
  double alpha0 = kp_cost.alpha0_estimate(t);
  double phi1 = kp_cost.phi1_estimate(t);


  dbsksp_twoshapelet_estimator estimator;
  return estimator.compute(this->start_xnode_, this->end_xnode_, alpha0, phi1, 
    dbsksp_twoshapelet_estimator::IGNORE_RADIUS_DISCREPANCY);
}


// ----------------------------------------------------------------------------
//: fit a shape given t
dbsksp_twoshapelet_sptr dbsksp_optimal_interp_two_xnodes::
optimize(double t)
{
  dbsksp_interp_two_xnodes_curvature_variation_type1_cost_fnt kp_cost(
    this->start_xnode_, this->end_xnode_,
    this->shock_estimate_);
  vnl_vector<double > x(1, t);
  double fit_error;
  return kp_cost.get_twoshapelet(x, fit_error);
}
