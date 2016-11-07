//: Aug 19, 2005 MingChing Chang
//  

#include <assert.h>
#include <vcl_algorithm.h>

#include <gdt/gdt_shock.h>
#include <gdt/gdt_solve_shock_intersect.h>
#include <gdt/gdt_manager.h>

//: Solve two shock intersection.
//  Three kinds of shocks:
//    - contact shock: b=0
//    - line shock : a=0
//    - hyperbola shock: a!=0, b!=0
//  If both shock are non-contact, use the intrinsic shock-shock formula.
//  If one of them is a contact (b=0), use the contact-shock formula.
//  If both shock are contact, no intersection.
bool solve_intrinsic_shock_int (const double& aLi, const double& bL2, const double& cL, 
                                const double& aRi, const double& bR2, const double& cR, 
                                const double& theta, 
                                const double& tauL_asym_min, const double& tauL_s_max, 
                                const double& tauR_s_min, const double& tauR_asym_max,
                                double& tauL, double& tauR)
{
  //1) If both shocks are contact, no intersection.
  assert (bL2 != 0 || bR2 !=0);
  assert (tauL_asym_min < tauL_s_max);
  assert (tauR_s_min < tauR_asym_max);

  //2) If leftS is contact, rightS can be a line or hyperbola.
  if (bL2 == 0) {
    if (aLi > 0) { // L-contact * shock
      tauL = vnl_math::pi;
      tauR = vector_minus_pi_to_pi (vnl_math::pi - theta);

      if (tauR_s_min < tauR && tauR < tauR_asym_max)
        return true;
      else
        return false;
    }
    else { // R-contact * shock
      tauL = 0;
      tauR = vector_minus_pi_to_pi (vnl_math::pi*2 - theta);

      if (tauR_s_min < tauR && tauR < tauR_asym_max)
        return true;
      else
        return false;
    }
  }

  //3) If rightS is contact, leftS can be a line or hyperbola.
  if (bR2 == 0) {
    if (aRi > 0) { // shock * R-contact
      tauR = 0;
      tauL = theta;

      if (tauL_asym_min < tauL && tauL < tauL_s_max)
        return true;
      else
        return false;
    }
    else { // shock * L-contact
      tauR = vnl_math::pi;
      tauL = vector_minus_pi_to_pi (theta - vnl_math::pi);

      if (tauL_asym_min < tauL && tauL < tauL_s_max)
        return true;
      else
        return false;
    }
  }

  //4) Both shocks are non-contact, use the shock-shock formula.

  const double cos_theta = vcl_cos (theta);
  const double sin_theta = vcl_sin (theta);

  const double A = -cL * cos_theta - bL2/bR2 * cR;
  const double B = cL * sin_theta;
  const double C = aLi - bL2/bR2 * aRi;

  return _solve_trig (A, B, C, theta, tauL_asym_min, tauL_s_max, tauR_s_min, tauR_asym_max, tauL, tauR);
}


// This function solves for tauR in the Acos(tauR)+Bsin(tauR)=C equation
// and returns true only if there is a valid solution!
// The solution is returned in tauR and tauL.
//
// The min, max value are from both asymptote and shock starting tau.
//
// Note that the shock is directional, so there will be only one solution!
//
// The relationship between tauR and tauL is that of a rotation of the
// axis of reference so tauL = tauR + theta;
//
// Since tauL/tauR min/max is computed from time, which is accuracty up to GDT_DIST_EPSILON (1E-5)
// Here should also use it to compare the angle.
//
bool _solve_trig (const double& A, const double& B, const double& C, const double& theta, 
                  const double& tauL_asym_min, const double& tauL_s_max, 
                  const double& tauR_s_min, const double& tauR_asym_max,
                  double& tauL, double& tauR)
{
  double tauL1, tauR1, tauL2, tauR2;
  assert (tauL_asym_min < tauL_s_max);
  assert (tauR_s_min < tauR_asym_max);

  if (A==0) {
    assert (B != 0);
    tauR1 = vcl_asin(C/B); // -pi/2 < asin < pi/2
    tauR2 = vector_minus_pi_to_pi (vnl_math::pi - tauR1);
  }
  else {
    // If acos(cos_value) not correct, no solution.
    double cos_value = C / vcl_sqrt(A*A + B*B);
    if (cos_value>1 || cos_value<-1)
      return false;

    double alpha = vcl_atan2 (B, A);

    // Two possible solutions: 0 < acos < pi, -pi < atan2 < pi
    // So -pi < acos+atan2 < 2pi.
    // But we know that valid solution is from 0~pi. How to convert???
    double acos_value = acos (cos_value);
    tauR1 = vector_minus_pi_to_pi (acos_value + alpha);
    tauR2 = vector_minus_pi_to_pi (-acos_value + alpha);
  }

  tauL1 = vector_minus_pi_to_pi (tauR1 + theta);
  tauL2 = vector_minus_pi_to_pi (tauR2 + theta);

  //R1) Check the validity of the first solution on right side.
  bool tauR1_valid = false;
  ///if (tauR_s_min < tauR1 && tauR1 < tauR_asym_max)
  if (_leqD (tauR_s_min, tauR1) && _leqD (tauR1, tauR_asym_max))
    tauR1_valid = true;

  //R2) Check the validity of the second solution on right side.
  bool tauR2_valid = false;
  ///if (tauR_s_min < tauR2 && tauR2 < tauR_asym_max)
  if (_leqD (tauR_s_min, tauR2) && _leqD (tauR2, tauR_asym_max))
    tauR2_valid = true;

  //L1) Check the validity of the first solution on left side.
  bool tauL1_valid = false;
  ///if (tauL_asym_min < tauL1 && tauL1 < tauL_s_max)
  if (_leqD (tauL_asym_min, tauL1) && _leqD (tauL1, tauL_s_max))
    tauL1_valid = true;

  //L2) Check the validity of the first solution on left side.
  bool tauL2_valid = false;
  ///if (tauL_asym_min < tauL2 && tauL2 < tauL_s_max)
  if (_leqD (tauL_asym_min, tauL2) && _leqD (tauL2, tauL_s_max))
    tauL2_valid = true;

  // Make final decision. At most one valid solution is possible
  if (tauL1_valid && tauR1_valid) {
    tauL = tauL1;
    tauR = tauR1;
    return true;
  }
  if (tauL2_valid && tauR2_valid) {
    tauL = tauL2;
    tauR = tauR2;
    return true;
  }

  return false;
}
