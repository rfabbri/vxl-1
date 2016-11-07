#ifndef rrel_homography2d_est_epipole_h_
#define rrel_homography2d_est_epipole_h_
//:
// \file

#include <rrel/rrel_homography2d_est.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vnl/vnl_double_3.h>

//: Subclass of the generalized 8-DOF homography estimator for affine transformations (6 DOF).

class rrel_homography2d_est_epipole : public rrel_homography2d_est
{
 public:

  //: Constructor from vgl_homg_point_2d's
  rrel_homography2d_est_epipole( const vcl_vector< vgl_homg_point_2d<double> > & from_pts,
                             const vcl_vector< vgl_homg_point_2d<double> > & to_pts,
                             vgl_homg_point_2d<double> from_epi, vgl_homg_point_2d<double> to_epi);

  //: Constructor from vnl_vectors
  rrel_homography2d_est_epipole( const vcl_vector< vnl_vector<double> > & from_pts,
                             const vcl_vector< vnl_vector<double> > & to_pts, 
                             vnl_vector<double> from_epi,vnl_vector<double>to_epi);

  //: Destructor.
  virtual ~rrel_homography2d_est_epipole();

  
virtual bool
 fit_from_minimal_set( const vcl_vector<int>& point_indices,
                                               vnl_vector<double>& params ) const;


 private:
     vnl_double_3 from_epi_;
     vnl_double_3 to_epi_;
};

#endif // rrel_homography2d_est_epipole_h_
