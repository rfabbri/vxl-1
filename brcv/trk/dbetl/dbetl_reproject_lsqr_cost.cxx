// This is algo/dbetl/dbetl_reproject_lsqr_cost.cxx
//:
// \file

#include "dbetl_reproject_lsqr_cost.h"
#include <vcl_cassert.h>
#include <vnl/vnl_vector_fixed.h>


//: Constructor
dbetl_reproject_lsqr_cost::dbetl_reproject_lsqr_cost( const vnl_matrix<double>& E, 
                                                    const vnl_matrix<double>& P3 )
 : vnl_least_squares_function(3,E.rows(),use_gradient),
   E_(E),
   P3_(P3)
{
  assert(E_.columns() == 4);
  assert(P3_.columns() == 4);
  assert(2*P3_.rows() == E_.rows());
}


//: Constructor
dbetl_reproject_lsqr_cost::dbetl_reproject_lsqr_cost( const vcl_vector<vnl_double_3x4>& cameras, 
                                                    const vcl_vector<vgl_point_2d<double> >& pts)
 : vnl_least_squares_function(3,2*cameras.size(),use_gradient),
   E_(2*cameras.size(),4),
   P3_(cameras.size(),4)
{
  assert(cameras.size() == pts.size());
  // Construct the matrices used in minimization
  for(unsigned int i=0; i<cameras.size(); ++i){
    P3_.set_row(i, cameras[i][2]);
    E_.set_row(2*i,   cameras[i].get_row(0) - pts[i].x()*cameras[i].get_row(2));
    E_.set_row(2*i+1, cameras[i].get_row(1) - pts[i].y()*cameras[i].get_row(2));
  }
}


//: The main function.
//  Given the parameter vector x, compute the vector of residuals fx.
//  Fx has been sized appropriately before the call.
void 
dbetl_reproject_lsqr_cost::f(vnl_vector<double> const& x, vnl_vector<double>& fx)
{
  // Create a homogeneous representation of the 3D point
  vnl_vector<double> hp(4,3,x.data_block()); hp[3] = 1.0;

  // Compute the unweighted residulal vector
  fx = E_*hp;

  // Compute the vector of depths (inverse weights)
  vnl_vector<double> w = P3_*hp;

  // Apply the weights to the residuals
  for(unsigned int i=0; i<fx.size(); ++i)
    fx[i] /= w[i/2];
}


//: Calculate the Jacobian, given the parameter vector x.
void 
dbetl_reproject_lsqr_cost::gradf(vnl_vector<double> const& x, vnl_matrix<double>& jacobian)
{
   // Create a homogeneous representation of the 3D point
  vnl_vector<double> hp(4,3,x.data_block()); hp[3] = 1.0;

  // Compute the unweighted residulal vector
  vnl_vector<double> residuals = E_*hp;

  // Compute the vector of depths (inverse weights)
  vnl_vector<double> depths = P3_*hp;

  // Apply the weights to the residuals
  for(unsigned int i=0; i<residuals.size(); ++i){
    double w = depths[i/2];
    double r = residuals[i];
    for(unsigned int j=0; j<3; ++j){
      jacobian(i,j) = (w*E_(i,j) - r*P3_(i/2,j))/(w*w); 
    }
  }
}
