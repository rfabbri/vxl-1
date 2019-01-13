#include <testlib/testlib_test.h>

#include <bsold/algo/bsold_curve_algs.h>
#include <bsold/bsold_interp_curve_2d.h>
#include <vsol/vsol_point_2d.h>
#include <vgl/vgl_vector_2d.h>
#include <vnl/vnl_math.h>

#include <iostream> 
#include <cmath>
#include <limits>


MAIN( test_interp_eno )
{
   START ("2D ENO Interpolation");
  
   std::vector<vsol_point_2d_sptr> pts;

   pts.push_back(new vsol_point_2d(0, std::pow(0, 2.0)));
   pts.push_back(new vsol_point_2d(0.2, std::pow(0.2, 2.0)));
   pts.push_back(new vsol_point_2d(1.5, std::pow(1.5, 2.0)));
   pts.push_back(new vsol_point_2d(2.2, std::pow(2.2, 2.0)));
   pts.push_back(new vsol_point_2d(2.3, std::pow(2.3, 2.0)));
   pts.push_back(new vsol_point_2d(3.6, std::pow(3.6, 2.0)));
   pts.push_back(new vsol_point_2d(4.0, std::pow(4.0, 2.0)));

   bsold_interp_curve_2d c;
   vnl_vector<double> samples;
   bsold_curve_algs::interpolate_eno(&c,pts,samples);
   std::cout << c;

   std::vector<vgl_point_2d<double> > pts2;
   double s1 = 1; 
   double s2 = 2; 
   double s3 = 4;
   pts2.push_back(vgl_point_2d<double>(s1, 2*std::pow(s1,2.0) - 3*s1 + 1));
   pts2.push_back(vgl_point_2d<double>(s2, 2*std::pow(s2,2.0) - 3*s2 + 1));
   pts2.push_back(vgl_point_2d<double>(s3, 2*std::pow(s3,2.0) - 3*s3 + 1));

   bsold_interp_curve_2d c2;
   vnl_vector<double> samples2;
   samples2.set_size(3);
   samples2(0) = 1; samples2(1) = 2; samples2(2) = 4;
   bsold_curve_algs::interpolate_eno(&c2,pts2,samples2);
   std::cout << c2;


   std::vector<vgl_point_2d<double> > pts3;
   s1 = 1; 
   s2 = 3; 
   pts3.push_back(vgl_point_2d<double>(s1, -3*s1 + 1));
   pts3.push_back(vgl_point_2d<double>(s2, -3*s2 + 1));

   bsold_interp_curve_2d c3;
   vnl_vector<double> samples3;
   samples3.set_size(2);
   samples3(0) = 1; samples3(1) = 3;
   bsold_curve_algs::interpolate_eno(&c3,pts3,samples3);
   std::cout << c3;

   SUMMARY();
}
