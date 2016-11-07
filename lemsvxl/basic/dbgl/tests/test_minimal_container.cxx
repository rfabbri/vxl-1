// This is basic/dbgl/tests/test_minimal_enclosing_ball.cxx

// \author Nhon Trinh
// \date July 13, 2006

#include <testlib/testlib_test.h>
#include <dbgl/dbgl_minimal_container.h>
#include <vcl_iostream.h>
#include <vcl_cmath.h>
#include <vgl/vgl_distance.h>



// ----------------------------------------------------------------------------
void test_minimal_enclosing_circle()
{
  vcl_vector<vgl_point_2d<float > > points;

  // test sphere enclosing a rectangular box
  // center (5, 3)
  points.push_back(vgl_point_2d<float >(0, 0) );
  points.push_back(vgl_point_2d<float >(10, 0) );
  points.push_back(vgl_point_2d<float >(10, 6) );
  points.push_back(vgl_point_2d<float >(0, 6) );
  
  points.push_back(vgl_point_2d<float >(2, 3) );
  points.push_back(vgl_point_2d<float >(8, 3) );

  vgl_point_2d<double > center;
  double radius;

  dbgl_minimal_enclosing_circle(points, center, radius);

  vgl_point_2d<double > true_center(5, 3);
  double true_radius = vcl_sqrt(double(5*5+3*3));
  

  double center_err = vgl_distance(center, true_center);
  double radius_err = vcl_abs(radius - true_radius);

  TEST_NEAR("Compute center of enclosing circle", center_err, 0, 1e-10);
  TEST_NEAR("Compute radius of enclosing circle", radius_err, 0, 1e-10);
}


// ----------------------------------------------------------------------------
void test_minimal_enclosing_sphere()
{
  vcl_vector<vgl_point_3d<float > > points;

  // test sphere enclosing a rectangular box
  // center (5, 3, 1)
  points.push_back(vgl_point_3d<float >(0, 0, 0) );
  points.push_back(vgl_point_3d<float >(10, 0, 0) );
  points.push_back(vgl_point_3d<float >(10, 6, 0) );
  points.push_back(vgl_point_3d<float >(0, 6, 0) );
  
  points.push_back(vgl_point_3d<float >(0, 0, 2) );
  points.push_back(vgl_point_3d<float >(10, 0, 2) );
  points.push_back(vgl_point_3d<float >(10, 6, 2) );
  points.push_back(vgl_point_3d<float >(0, 6, 2) );
  
  points.push_back(vgl_point_3d<float >(2, 3, 4) );
  points.push_back(vgl_point_3d<float >(8, 3, -2) );

  vgl_point_3d<double > center;
  double radius;

  dbgl_minimal_enclosing_sphere(points, center, radius);

  vgl_point_3d<double > true_center(5, 3, 1);
  double true_radius = vcl_sqrt(double(5*5+3*3+1*1));
  

  double center_err = vgl_distance(center, true_center);
  double radius_err = vcl_abs(radius - true_radius);

  TEST_NEAR("Compute center of enclosing sphere", center_err, 0, 1e-10);
  TEST_NEAR("Compute radius of enclosing sphere", radius_err, 0, 1e-10);
}


//: Test closest point functions
MAIN( test_minimal_container )
{
  START (" Test Minimal Container ");
  test_minimal_enclosing_circle();
  test_minimal_enclosing_sphere();  
  SUMMARY();
}
