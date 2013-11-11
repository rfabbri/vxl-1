//: Aug 19, 2005 MingChing Chang
//  

#include <assert.h>
#include <vcl_algorithm.h>

#include <gdt/gdt_numerical.h>
#include <gdt/gdt_solve_intersect.h>
#include <gdt/gdt_edge.h>

bool _proj_tau_to_L (const double& L, const double& H, 
                     const double& input_tau, const double& alpha_l, 
                     const dbmsh3d_gdt_edge* cur_edge,
                     const dbmsh3d_gdt_edge* left_edge,
                     const double& nL_nofix, const double& nH, 
                     double& proj_tau)
{
  const double input_theta = _theta_from_tau (L, H, input_tau);
  const double new_theta = input_theta + alpha_l;
  
  if (new_theta <= 0 || new_theta >= vnl_math::pi)
    return false;

  proj_tau = nL_nofix - nH/vcl_tan(new_theta);

  //: numerical error
  if (proj_tau < 0)
    proj_tau = 0;

  //: fix proj_tau orientation issue
  if (cur_edge->sV() != left_edge->sV())
    proj_tau = left_edge->len() - proj_tau;

  return true;
}

bool _proj_tau_to_R (const double& L, const double& H, 
                     const double& input_tau, const double& alpha_r,
                     const dbmsh3d_gdt_edge* cur_edge,
                     const dbmsh3d_gdt_edge* right_edge,
                     const double& nL_nofix, const double& nH, 
                     double& proj_tau)
{
  const double input_theta = _theta_from_tau (L, H, input_tau);
  const double new_theta = input_theta - alpha_r;

  if (new_theta <= 0 || new_theta >= vnl_math::pi)
    return false;

  const double lenR = right_edge->len();
  proj_tau = nL_nofix - nH/vcl_tan(new_theta);

  //: numerical error
  if (proj_tau > lenR)
    proj_tau = lenR;

  //: fix proj_tau orientation issue
  if (cur_edge->eV() != right_edge->eV())
    proj_tau = lenR - proj_tau;

  return true;
}

// ###############################################################

// This function solves quadratic equations
// tau1 <= tau2
bool _solve_quad (const double& A, const double& B, const double& C,
                  double& tau1, double& tau2)
{
  if (_eqM (A, 0))
    return false;

  double delta = B*B-4*A*C;

  //: be careful about the numerical issues
  if (_eqM (delta,0)) {
    //: delta = 0;
    tau1 = (-B)/A*0.5;
    return true;
  }

  if (delta < 0)
    return false;
  
  tau1 = (-B - sqrt(delta))/A*0.5;
  tau2 = (-B + sqrt(delta))/A*0.5;

  return true;
}

void _solve_eqdist_tau12 (const double& La, const double& Ha, const double& muA, 
                          const double& Lb, const double& Hb, const double& muB,
                          double& tau1, double& tau2)
{
  //: solve for the equal-distance ptau
  //  At most two solutions, assuming the two solutions: tau1 <= tau2
  tau1 = INVALID_TAU;
  tau2 = INVALID_TAU;
 
  //: if Ha==Hb and La==Lb, two identical hyperbola,
  //  possible for a contact shock from a planar vertex.
  //  Should detect and handle it separately!
  assert (!_eqM(La,Lb) || !_eqM(Ha, Hb));
  
  //: try to solve for up to two solutions.   
  double alpha = Lb - La;
  double beta = muB - muA;
  double gamma = La*La + Ha*Ha - Lb*Lb - Hb*Hb - beta*beta;

  //: if beta==0, B*B-4AC is 0.
  double A = alpha*alpha - beta*beta;
  double B = gamma*alpha + Lb*beta*beta*2;
  double C = gamma*gamma*0.25 - (Lb*Lb+Hb*Hb)*beta*beta;

  if (_solve_quad (A, B, C, tau1, tau2)) {
    //: test the two sqrt-root conditions, if beta !=0 and the solution is still valid
    //  In beta==0 case, the constaints do not apply.
    if (beta!=0) {
      if ((gamma+2*alpha*tau1)/beta < 0)
        tau1 = INVALID_TAU;
      if ((gamma+2*beta*beta+2*alpha*tau1)/beta <0)
        tau1 = INVALID_TAU;

      if (tau2 != INVALID_TAU) {
        if ((gamma+2*alpha*tau2)/beta < 0)
          tau2 = INVALID_TAU;
        if ((gamma+2*beta*beta+2*alpha*tau2)/beta <0)
          tau2 = INVALID_TAU;
      }
    }
  }
}

double solve_eqdist_tau (const double& La, const double& Ha, const double& muA, 
                         const double& Lb, const double& Hb, const double& muB,
                         const double& tau_min, const double& tau_max,
                         const double& existingTau)
{
  double tau1, tau2;
  _solve_eqdist_tau12 (La, Ha, muA, Lb, Hb, muB, tau1, tau2);

  //If tau1 == tau2 (one solution), make one of them invalid.
  if (_eqT (tau1, tau2) && tau1 != INVALID_TAU) {
    if (tau_min<=tau1 && tau1<=tau_max) {
      if (tau_min<=tau2 && tau2<=tau_max) {
        tau1 = (tau1 + tau2)/2;
        tau2 = INVALID_TAU;
      }
    }
    else {
      tau1 = INVALID_TAU;
      if (tau_min>tau2 || tau2>tau_max)
        tau2 = INVALID_TAU;
    }
  }

  //Numerical boundary of tau min max.
  if (_eqT(tau1, tau_min))
    tau1 = tau_min;
  if (_eqT(tau1, tau_max))
    tau1 = tau_max;
  if (_eqT(tau2, tau_min))
    tau2 = tau_min;
  if (_eqT(tau2, tau_max))
    tau2 = tau_max;

  //Check the min, max.
  if (tau1 < tau_min || tau1 > tau_max)
    tau1 = INVALID_TAU;
  if (tau2 < tau_min || tau2 > tau_max)
    tau2 = INVALID_TAU;

  //strangely, this is possible if at boundary.
  if (tau1 == tau2)
    tau2 = INVALID_TAU;

  //If tau1/tau2 == existingTau, make it invalid.
  if (tau1 == existingTau)
    tau1 = INVALID_TAU;
  if (tau2 == existingTau)
    tau2 = INVALID_TAU;

  //Only one solution is possible!
  if (tau1 != INVALID_TAU) {
    assert (tau2 == INVALID_TAU);
    return tau1;
  }
  else if (tau2 != INVALID_TAU) {
    assert (tau1 == INVALID_TAU);
    return tau2;
  }
  else {
    return INVALID_TAU;
  }
}

