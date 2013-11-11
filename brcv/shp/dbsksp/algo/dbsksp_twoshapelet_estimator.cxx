// This is file shp/dbsksp/dbsksp_twoshapelet_estimator.cxx

//:
// \file

#include "dbsksp_twoshapelet_estimator.h"


#include <vnl/vnl_math.h>
#include <vnl/vnl_det.h>
#include <vnl/vnl_inverse.h>

// ============================================================================
// dbsksp_estimate_twoshapelet
// ============================================================================



const double dbsksp_twoshapelet_estimator::
det_A_epsilon = 1e-12;


//: Main function: estimate a twoshapelet given
  // start_xnode, end_xnode, alpha0, and phi1
dbsksp_twoshapelet_sptr dbsksp_twoshapelet_estimator::
compute(const dbsksp_xshock_node_descriptor& start_xnode,
        const dbsksp_xshock_node_descriptor& end_xnode,
        double alpha0,
        double phi1,
        estimation_type type)
{
  this->set(start_xnode, end_xnode, alpha0, phi1);
  
  return this->compute(type);
}


// ----------------------------------------------------------------------------
//: Do the actual computation for the over the constrained problem
// ``type'' specifies the term that is ignored in the estimation
dbsksp_twoshapelet_sptr dbsksp_twoshapelet_estimator::
compute(estimation_type type)
{
  // unknown parameter: x[0] --> phi1
  // fix parameter on the twoshapelet
  //double x0_;
  //double y0_;
  //double theta0_;
  //double r0_;
  //double phi0_;
  //double m0_;
  //double phi2_;
  

  // To determine:
  //double m1_;
  //double len1_;
  //double len0_;

  //dbsksp_xshock_node_descriptor start_xnode = this->start_xnode();
  //dbsksp_xshock_node_descriptor end_xnode = this->end_xnode();
  //double phi1 = x[0];
  //double alpha0 = this->alpha0();

  double phi0 = start_xnode.phi_;
  double phi2 = end_xnode.phi_;
  vgl_vector_2d<double > tA (vcl_cos(start_xnode.psi_), vcl_sin(start_xnode.psi_));
  vgl_vector_2d<double > tC (vcl_cos(end_xnode.psi_), vcl_sin(end_xnode.psi_));

  // 1. 
  double m0 = vcl_sin(alpha0) / vcl_sin(phi0);

  // 2. alpha10 = angle(AB, tB)
  double alpha10 = vcl_asin(-m0 * vcl_sin(phi1));

  // 3.
  double angle_tA_tB = -alpha0 + alpha10;

  // 4. angle (tB, tC) = angle(tA, tC) - angle(tA, tB)
  double angle_tA_tC = signed_angle(tA, tC);
  double angle_tB_tC = angle_tA_tC - angle_tA_tB;
  
  //// 4. angle (tB, tC) = (tC - AB) - (tB - AB)
  ////                   = (psi2 - theta0) - (alpha10)

  //// shock tangent at B
  //vgl_vector_2d<double > tB(vcl_cos(ss->theta0()+alpha10), vcl_sin(ss->theta0()+alpha10));
  //vgl_vector_2d<double > tC(vcl_cos(this->xnode_.psi_), vcl_sin(this->xnode_.psi_));
  //double angle_tB_tC = signed_angle(tB, tC);


  //                      sin(phi1) sin (angle_tB_tC)    
  // 5. tan(alpha11) = - -----------------------------------------
  //                      sin(phi2) + sin(phi1) * cos(angle_tB_tC)
  // alpha11 = angle (BC, tB)
  
  double alpha11 = vcl_atan2(-vcl_sin(phi1)*vcl_sin(angle_tB_tC),  // y
    vcl_sin(phi2)+ vcl_sin(phi1) * vcl_cos(angle_tB_tC)); // x

  double m1 = vcl_sin(alpha11) / vcl_sin(phi1);


  // compute the two normalized radius increment of the two fragments
  // construct a twoshapelet with wrong chord_lengths to compute normalized
  // radius increments
  double theta0 = start_xnode.psi_ - alpha0;
  dbsksp_twoshapelet_sptr ss = new dbsksp_twoshapelet(start_xnode.pt_.x(), start_xnode.pt_.y(),
    theta0, start_xnode.radius_, phi0, m0, 1, phi1, m1, 1, phi2);
  
  double lambda0 = ss->shapelet_start()->normalized_radius_increment();
  double lambda1 = ss->shapelet_end()->normalized_radius_increment();

  // Let A, C: the two shock points of the xnodes
  // B: intersecting shock point of the two fragments
  // C': true end-shock-point of the constructed twoshapelet
  // two linear equations
  // lambda0 * len0 + lambda1 * len1 = R2 - R0
  // cos(angle(AC, AB)) * len0 + cos(angle(AC, BC') * len1 = L  // AC
  
  vgl_vector_2d<double > vAC = end_xnode.pt_ - start_xnode.pt_;
  vgl_vector_2d<double > vAB(vcl_cos(theta0), vcl_sin(theta0));
  double angle_AC_AB = signed_angle(vAC, vAB);

  //double cos_AC_AB = cos_angle(vAC, vAB);
  

  vgl_vector_2d<double > vBCprime = rotated(vAB, alpha10-alpha11);
  double angle_AC_BCprime = signed_angle(vAC, vBCprime);
  //double cos_BCprime_AC = cos_angle(vAC, vBCprime);

  // set up the linear system of equations to compute len0 and len1
  // the system depends on what term is ignored in the estimation
  // because the system is over constrained, there has to be term that is ignored

  vnl_vector_fixed<double, 2> len;
  switch (type)
  {
  case IGNORE_VERTICAL_DISCREPANCY:
    A(0, 0) = lambda0;
    A(0, 1) = lambda1;
    A(1, 0) = vcl_cos(angle_AC_AB);
    A(1, 1) = vcl_cos(angle_AC_BCprime);

    B(0) = end_xnode.radius_-start_xnode.radius_;
    B(1) = vAC.length();

    if (vnl_math_abs(vnl_det(A)) > dbsksp_twoshapelet_estimator::det_A_epsilon)
    {
      len = vnl_inverse(A) * B;
    }
    else
    {
      // check whether this is a degenerate system
      // C = A(:, 1) | B
      vnl_matrix_fixed<double, 2, 2 > C;
      C(0, 0) = A(0, 1);
      C(1, 0) = A(1, 1);
      C(0, 1) = B(0);
      C(1, 1) = B(1);
      // if det(C) > 0, the system has no solution
      if (vnl_math_abs(vnl_det(C)) > dbsksp_twoshapelet_estimator::det_A_epsilon)
      {
        len(0) = 1e100;
        len(1) = -1e100;
      }
      // otherwise, there are infinity number of solution
      else
      {
        vnl_matrix_fixed<double, 2, 2 > AA = A;
        AA(1, 0) = vcl_sin(angle_AC_AB);
        AA(1, 1) = vcl_sin(angle_AC_BCprime);

        vnl_vector_fixed<double, 2> BB = B;
        BB(0) = 0;

        if (vnl_math_abs(vnl_det(AA)) > dbsksp_twoshapelet_estimator::det_A_epsilon)
        {
          len = vnl_inverse(AA) * BB;
        }
        else
        {
          len(0) = len(1) = B(1) / (A(1,0) + A(1,1)); 
        }
      }
    }
    break;
  case IGNORE_RADIUS_DISCREPANCY:
    A(0, 0) = vcl_cos(angle_AC_AB);
    A(0, 1) = vcl_cos(angle_AC_BCprime);
    A(1, 0) = vcl_sin(angle_AC_AB);
    A(1, 1) = vcl_sin(angle_AC_BCprime);

    B(0) = vAC.length();
    B(1) = 0;

    if (vnl_math_abs(vnl_det(A)) > dbsksp_twoshapelet_estimator::det_A_epsilon)
    {
      len = vnl_inverse(A) * B;
    }
    else
    {
      // check whether this is a degenerate system
      // C = A(:, 1) | B
      vnl_matrix_fixed<double, 2, 2 > C;
      C(0, 0) = A(0, 1);
      C(1, 0) = A(1, 1);
      C(0, 1) = B(0);
      C(1, 1) = B(1);
      // if det(C) > 0, the system has no solution
      if (vnl_math_abs(vnl_det(C)) > dbsksp_twoshapelet_estimator::det_A_epsilon)
      {
        len(0) = 1e100;
        len(1) = -1e100;
      }
      // otherwise, there are infinity number of solution
      else
      {
        vnl_matrix_fixed<double, 2, 2 > AA = A;
        AA(1, 0) = lambda0;
        AA(1, 1) = lambda1;
        vnl_vector_fixed<double, 2> BB = B;
        BB(1) = end_xnode.radius_-start_xnode.radius_;

        if (vnl_math_abs(vnl_det(AA)) > dbsksp_twoshapelet_estimator::det_A_epsilon)
        {
          len = vnl_inverse(AA) * BB;
        }
        else
        {
          len(0) = len(1) = B(0) / (A(0,0) + A(0,1)); 
        }
      }
    }

    break;
  default:
    A.set_identity();
    vcl_cerr << "ERROR: In dbsksp_twoshapelet_estimator:compute(...)\n"
      << "   Unknown estimation type.\n";
    break;  
  }
  
  
  

  ss->set_len0(len(0));
  ss->set_len1(len(1));

  // compute discrepancy of the parameter that was ignored in the estimation
  switch (type)
  {
  case IGNORE_VERTICAL_DISCREPANCY:
    {
      double vert_err = 
        vcl_sin(angle_AC_AB)*len(0) + vcl_sin(angle_AC_BCprime) * len(1);
      this->discrepancy_of_ignored_param_ = vert_err / vAC.length();
      break;
    }
  case IGNORE_RADIUS_DISCREPANCY:
    {
      double radius_err = lambda0 * len(0) + lambda1 * len(1) - 
        (end_xnode.radius_-start_xnode.radius_);
      this->discrepancy_of_ignored_param_ = radius_err / vAC.length();
      break;
    }
  default:
    vcl_cerr << "ERROR: In dbsksp_twoshapelet_estimator:compute(...)\n"
      << "   Unknown estimation type.\n";
    break;  
  }

  this->ss_ = ss;
  return ss;
}



// ----------------------------------------------------------------------------
//: determinant of matrix A used to compute chord lengths
double dbsksp_twoshapelet_estimator::
det_A() const
{
  return vnl_det(this->A);
}





//// ----------------------------------------------------------------------------
////: Compute the value of phi1 at the discontinuity point on len0 and len1 as 
//// functions of phi1 when estimating a twoshapelet fitting to two extrinsic 
//// nodes
//double dbsksp_compute_phi1_where_len0_and_len1_discontinuous_at(const dbsksp_xshock_node_descriptor& start_xnode,
//  const dbsksp_xshock_node_descriptor& end_xnode,
//  double alpha0)
//{
//  // Notation: A : start shock point
//  // B: mid_shock point
//  // C: (desired) end shock point
//  // C' (Cprime) : end shock point as the result of estimation
//  double phi0 = start_xnode.phi_;
//  double phi2 = end_xnode.phi_;
//  vgl_vector_2d<double > tA(vcl_cos(start_xnode.psi_), vcl_sin(start_xnode.psi_));
//  vgl_vector_2d<double > tC(vcl_cos(end_xnode.psi_), vcl_sin(end_xnode.psi_));
//
//  double m0 = vcl_sin(alpha0) / vcl_sin(phi0);
//
//  vgl_vector_2d<double > vAC = normalized(end_xnode.pt_ - start_xnode.pt_);
//  vgl_vector_2d<double > vAB = rotated(tA, -alpha0);
//  
//
//  double angle_vAC_vAB = signed_angle(vAC, vAB);
//  double a = angle_vAC_vAB;
//  vgl_vector_2d<double > vBCprime = rotated(vAB, -2*a);
//  
//  double alpha2 = signed_angle(vBCprime, tC);
//  double m1 = - vcl_sin(alpha2) / vcl_sin(phi2);
//
//  // condition check to avoid denominator = 0;
//  if ( vnl_math_abs(m1) < 1e-12 && vnl_math_abs(m0) < 1e-12 )
//    return 1e-12;
//
//  double t_sqr = vnl_math_sqr(vcl_sin(2*a)) / 
//    ( vnl_math_sqr(m1 * vcl_cos(2*a) + m0) + vnl_math_sqr(m1 * vcl_sin(2*a)) );
//
//  double phi1_discontinuity = vcl_asin(vcl_sqrt(t_sqr));
//  return phi1_discontinuity;
//
//  // TODO: validate this result on gui
//}
//
//
