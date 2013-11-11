// This is algo/dbetl/dbetl_reproject_lsqr_cost.h
#ifndef dbetl_reproject_lsqr_cost_h_
#define dbetl_reproject_lsqr_cost_h_
//:
// \file
// \brief A least squares cost function for the reprojection of 3D points into images
// \author Matt Leotta
// \date 12/9/04
//
// \verbatim
//  Modifications
//   None
// \endverbatim

#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_least_squares_function.h>
#include <vnl/vnl_double_3x4.h>
#include <vgl/vgl_point_2d.h>

//: A least squares cost function for the reprojection of 3D points into images
class dbetl_reproject_lsqr_cost : public vnl_least_squares_function
{
public:
  //: Constructor - using the cost matrices
  dbetl_reproject_lsqr_cost(const vnl_matrix<double>& E, const vnl_matrix<double>& P3);

  //: Constructor - using cameras and correspondence points
  dbetl_reproject_lsqr_cost( const vcl_vector<vnl_double_3x4>& cameras, 
                            const vcl_vector<vgl_point_2d<double> >& pts);

  //: Destructor
  virtual ~dbetl_reproject_lsqr_cost() {}

  //: The main function.
  //  Given the parameter vector x, compute the vector of residuals fx.
  //  fx has been sized appropriately before the call.
  virtual void f(vnl_vector<double> const& x, vnl_vector<double>& fx);

  //: Calculate the Jacobian, given the parameter vector x.
  virtual void gradf(vnl_vector<double> const& x, vnl_matrix<double>& jacobian);

protected:
  //: The 2n x 4 matrix use to compute reprojection error
  vnl_matrix<double> E_;
  //: The n x 4 matrix of camera 3rd rows used to compute depths
  vnl_matrix<double> P3_;
};

#endif // dbetl_reproject_lsqr_cost_h_

