#include <testlib/testlib_test.h>

#include <bgld/bgld_poly_curve_line.h>
#include <bsold/bsold_interp_curve_2d.h>
#include <dbcvr/dbcvr_clsd_interp_cvmatch.h>

#include <iostream>
#include <cmath>

#define NearZeroValue    (1.0e-8)

MAIN( test_clsd_interp_cvmatch )
{
  START ("Closed Interpolated Curve 2d Matching Test");

  std::cout << "construct first line:" << std::endl;
  vgl_point_2d<double> p0(-1.0, -1.0);  // 0
  vgl_point_2d<double> p1(0.0, 0.0);   
  vgl_point_2d<double> p2(1.0, 0.0);
  
  std::vector<bgld_param_curve *> ints(3);
  ints[0] = new bgld_poly_curve_line(p0, p1);
  ints[1] = new bgld_poly_curve_line(p1, p2);
  ints[2] = new bgld_poly_curve_line(p2, p0);

  bsold_interp_curve_2d_sptr c1 = new bsold_interp_curve_2d(ints);
  
  std::cout << "construct second line:" << std::endl;
  vgl_point_2d<double> p3(-1.0, 0.0);  // 0
  vgl_point_2d<double> p4(0.0, 1.0);   
  vgl_point_2d<double> p5(1.0, 1.0);
  
  std::vector<bgld_param_curve *> ints2(3);
  ints2[0] = new bgld_poly_curve_line(p3, p4);
  ints2[1] = new bgld_poly_curve_line(p4, p5);
  ints2[2] = new bgld_poly_curve_line(p5, p3);

  bsold_interp_curve_2d_sptr c2 = new bsold_interp_curve_2d(ints2);

  double L = c1->length();
  double L_bar = c2->length();

  TEST("curve 1 closed? ", *(c1->point_at(0)) == *(c1->point_at(L)), true);
  TEST("curve 2 closed? ", *(c2->point_at(0)) == *(c2->point_at(L_bar)), true);

  std::cout << "c1 tangent 0: " << c1->tangent_angle_at(0) << " tangent L: " << c1->tangent_angle_at(L) << std::endl;
  std::cout << "c2 tangent 0: " << c2->tangent_angle_at(0) << " tangent L: " << c2->tangent_angle_at(L_bar) << std::endl;

 // dbcvr_interp_cvmatch cvm(c1, c2, 10.0f, (L+L_bar)/6);
 // cvm->Match();
 // TEST("match() ", std::fabs(cvm->finalCost()-0.0)<NearZeroValue, true);
  
  SUMMARY();
}
