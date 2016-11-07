//: Aug 19, 2005 MingChing Chang

#ifndef gdt_solve_intersect_h_
#define gdt_solve_intersect_h_

#include <vcl_cmath.h>
#include <vcl_utility.h>
#include <gdt/gdt_numerical.h>

class dbmsh3d_gdt_edge;

// ###############################################################
//   Interval Propagation Subroutines

//: given a theta, return the arc-length parameter, tau
inline double _tau_from_theta (const double& L, const double& H, 
                              const double input_theta) 
{
  assert (H != 0);
  //: be careful that tan(theta)=0
  double tan_theta = vcl_tan (input_theta);
  if (tan_theta==0)
    return GDT_NEG_HUGE;

  return L - H / tan_theta;
}

//: given a tau, return the angle parameter, theta
inline double _theta_from_tau (const double& L, const double& H,
                               const double input_tau) 
{
  assert (H != 0);
  return vcl_atan2 (H, L-input_tau);
}

inline void _get_next_left_L_H (const double& L, const double& H,
                                const double& alpha_l, 
                                double& left_nL_nofix, double& left_nH)
{
  if (_eqM (alpha_l, vnl_math::pi/2)) {
    left_nH       = L;
    left_nL_nofix = -H;
  }
  else { // 0 < alpha_l < pi/2 or pi/2 < alpha_l < pi
    const double cos_alpha_l = vcl_cos(alpha_l);
    const double tan_alpha_l = vcl_tan(alpha_l);
    left_nH       = (H + L*tan_alpha_l) * cos_alpha_l;
    left_nL_nofix = L/cos_alpha_l - left_nH*tan_alpha_l;
  }
}

// lenC: cur_edge length
// lenR: right_edge len
inline void _get_next_right_L_H (const double& L, const double& H, 
                                 const double& lenC, const double& lenR, 
                                 const double& alpha_r, 
                                 double& right_nL_nofix, double& right_nH)
{
  if (_eqM (alpha_r, vnl_math::pi/2)) {
    right_nH        = lenC - L;
    right_nL_nofix  = lenR + H;
  }
  else { // 0 < alpha_r < pi/2 or pi/2 < alpha_r < pi
    double cos_alpha_r = vcl_cos(alpha_r);
    double tan_alpha_r = vcl_tan(alpha_r);
    right_nH        = (H + (lenC-L)*tan_alpha_r) * cos_alpha_r;
    right_nL_nofix  = lenR - (lenC-L)/cos_alpha_r + right_nH*tan_alpha_r;
  }
}

bool _proj_tau_to_L (const double& L, const double& H,
                     const double& input_tau, const double& alpha_l, 
                     const dbmsh3d_gdt_edge* cur_edge,
                     const dbmsh3d_gdt_edge* left_edge,
                     const double& left_nL_nofix, const double& left_nH, 
                     double& proj_tau);

bool _proj_tau_to_R (const double& L,const double& H, 
                     const double& input_tau, const double& alpha_r,
                     const dbmsh3d_gdt_edge* cur_edge,
                     const dbmsh3d_gdt_edge* right_edge,
                     const double& right_nL_nofix, const double& right_nH, 
                     double& proj_tau);

// ###############################################################

bool _solve_quad (const double& A, const double& B, const double& C,
                  double& sol1, double& sol2);

//: solve for both possible solutions.
void _solve_eqdist_tau12 (const double& l1, const double& h1, const double& mu1, 
                          const double& l2, const double& h2, const double& mu2,
                          double& tau1, double& tau2);

//: use the above function to solve for one solution.
double solve_eqdist_tau (const double& La, const double& Ha, const double& muA, 
                         const double& Lb, const double& Hb, const double& muB,
                         const double& tau_min, const double& tau_max,
                         const double& existingTau = GDT_HUGE);

#endif



