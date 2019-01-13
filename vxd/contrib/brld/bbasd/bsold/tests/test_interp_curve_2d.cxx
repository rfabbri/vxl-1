#include <testlib/testlib_test.h>

#include <bgld/bgld_poly_curve_line.h>
#include <bsold/bsold_interp_curve_2d.h>

#include <iostream>
#include <cmath>

#define NearZeroValue    (1.0e-8)

bool almost_equal3(vgl_point_2d<double> a, vgl_point_2d<double> b) {
  return (std::fabs(a.x()-b.x()) < NearZeroValue &&
          std::fabs(a.y()-b.y()) < NearZeroValue);
}

MAIN( test_interp_curve_2d )
{
  START ("Interpolated Curve 2d ");

  std::cout << "construct a line along x axis:" << std::endl;
  vgl_point_2d<double> p0(-1.0, -1.0);  // 0
  vgl_point_2d<double> p1(0.0, 0.0);   
  vgl_point_2d<double> p2(1.0, 0.0);
  double s = 0.74563;
  vgl_point_2d<double> test_p(s, 0.0);
  
  std::vector<bgld_param_curve *> ints(2);
/*
  ints[0] = new bgld_poly_curve_line(p0, p1);
  ints[1] = new bgld_poly_curve_line(p1, p2);

  bsold_interp_curve_2d c(ints);
  
  TEST("length() ", std::fabs(c.length()-(sqrt((double)2)+1))<NearZeroValue, true);
  TEST("point_at() start ", almost_equal3((c.point_at(0))->get_p(), p0), true);
  TEST("point_at() middle ", almost_equal3((c.point_at(sqrt((double)2)))->get_p(), p1), true);
  TEST("point_at() end ", almost_equal3((c.point_at(c.length()))->get_p(), p2), true);

  TEST("point_at() test_p ", almost_equal3((c.point_at(sqrt((double)2)+s))->get_p(), test_p), true);

  TEST("tangent_angle_at() start ", std::fabs(c.tangent_angle_at(0.0)-PI/4)<NearZeroValue, true);
  TEST("tangent_angle_at() 0.5 ", std::fabs(c.tangent_angle_at(0.5)-PI/4)<NearZeroValue, true);
  TEST("tangent_angle_at() 0.99999 ", std::fabs(c.tangent_angle_at(0.9999999999)-PI/4)<NearZeroValue, true);
  TEST("tangent_angle_at() middle ", std::fabs(c.tangent_angle_at(1)-PI/4)<NearZeroValue, true);
  TEST("tangent_angle_at() 1.5 ", std::fabs(c.tangent_angle_at(1.5)-0.0)<NearZeroValue, true);
  TEST("tangent_angle_at() end ", std::fabs(c.tangent_angle_at(2)-0.0)<NearZeroValue, true);

//  bgld_poly_curve_line b(start, end);
//  TEST("Operator == ", a == b, true);

//  bgld_poly_curve_line c(a);
//  TEST("Copy Constructor ", a == c, true);

  //: construct a line in reverse orientation
//  std::cout << "Testing line in reverse direction\n";
//  bgld_poly_curve_line d(end, start);
//  TEST("line vs reverse line equality ", c == d, false);
//  TEST("reverse line tangent_angle_at() ", std::fabs(d.tangent_angle_at(0.0)-PI)<NearZeroValue, true);
  
//  midtan = d.tangent_at(1.0/4.0);
//  TEST("reverse line tangent_at() ", std::fabs(atan2(midtan.y(),midtan.x())-PI)<NearZeroValue*100, true);

  //: construct line at center (1,1) 
//  std::cout << "Testing third line direction\n";
//  start.set(-2.0,0.0);
//  end.set(0.0,2.0);
//  center.set(-1.0,1.0);
//  bgld_poly_curve_line f(start, end);
  
//  TEST("Operator == ", a == f, false);

//  TEST("third line point_at() middle ", f.point_at(0.5) == center, true);
//  TEST("third line length() ", std::fabs(f.length()-2*std::sqrt(2.0))<NearZeroValue, true); 

//  midtan = f.tangent_at(1.0/4.0);
 // TEST("third line tangent_at() ", std::fabs(atan2(midtan.y(),midtan.x())-PI/4)<NearZeroValue, true);
  */
  SUMMARY();
}
