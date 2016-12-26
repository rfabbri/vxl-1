// This is /algo/dbetl/dbetl_epiprofile.h
#ifndef dbetl_epiprofile_h_
#define dbetl_epiprofile_h_

//:
// \file
// \brief  Creates a profile of points along an epipolar line
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date   11/12/2004

#include <vcl_vector.h>
#include <dbecl/dbecl_episeg_sptr.h>
#include <dbetl/dbetl_point_2d_sptr.h>
#include <vil/vil_image_resource_sptr.h>


//: Computes the intersections of an epipolar line with a vector of episegs
// \param episegs is the collection of episegements in an image
// \param angle is the epipolar angle and defines and epipolar line
// \returns a vector of intersection points
// \note it is assumed that all episegs share a common epipole
vcl_vector<dbetl_point_2d_sptr> 
dbetl_epiprofile( const vcl_vector<dbecl_episeg_sptr>& episegs, double angle );


//: Computes the intersections of an epipolar line with a vector of episegs
//  and also compiles statistics from the image along the profile
// \param episegs is the collection of episegements in an image
// \param image is the image that the episegs came from
// \param angle is the epipolar angle and defines and epipolar line
// \returns a vector of intersection points
// \note it is assumed that all episegs share a common epipole
vcl_vector<dbetl_point_2d_sptr> 
dbetl_epiprofile( const vcl_vector<dbecl_episeg_sptr>& episegs, 
                 const vil_image_resource_sptr& image,
                 double angle );


//: Computes the intersections of several epipolar lines with a vector of episegs.
//  Also compiles statistics from the image along the profile.
//  Also links intersection point across neighboring epipolar lines
// \param episegs is the collection of episegements in an image
// \param image is the image that the episegs came from
// \param start_angle is the initial epipolar angle
// \param step is the increment in the epipolar angle
// \param num is the number of steps to take
// \returns a vector of vectors of intersection points of size \p num
// \note it is assumed that all episegs share a common epipole
vcl_vector<vcl_vector<dbetl_point_2d_sptr> >
dbetl_epiprofile( const vcl_vector<dbecl_episeg_sptr>& episegs, 
                 const vil_image_resource_sptr& image,
                 double start_angle, double step, int num );


#endif // dbetl_epiprofile_h_
