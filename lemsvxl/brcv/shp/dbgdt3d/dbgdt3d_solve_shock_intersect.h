//: Aug 19, 2005 MingChing Chang
//  Defines the Interval Section for Goedesic DT Wavefront Propagation Algorithm

#ifndef gdt_solve_shock_intersect_h_
#define gdt_solve_shock_intersect_h_

#include <vcl_cmath.h>
#include <vcl_utility.h>
#include <vcl_map.h>

#include <dbgdt3d/dbgdt3d_shock.h>

bool solve_intrinsic_shock_int (const double& aL, const double& bL2, const double& cL, 
                                const double& aR, const double& bR2, const double& cR, 
                                const double& theta, 
                                const double& tauL_asym_min, const double& tauL_s_max, 
                                const double& tauR_s_min, const double& tauR_asym_max,
                                double& tauL, double& tauR);

bool _solve_trig (const double& A, const double& B, const double& C, const double& theta, 
                  const double& tauL_asym_min, const double& L_tau_ts, 
                  const double& tauR_s_min, const double& tauR_asym_max,
                  double& tauL, double& tauR);

#endif



