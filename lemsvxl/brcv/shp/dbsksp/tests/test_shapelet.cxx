// This is shp/dbsksp/tests/test_shock_graph.cxx

// \author Nhon Trinh
// \date Sep 27, 2006

#include <testlib/testlib_test.h>
#include <dbsksp/dbsksp_shapelet.h>
#include <vnl/vnl_math.h>

// ----------------------------------------------------------------------------
void test_shapelet()
{
  double x0 = 0;
  double y0 = 0;
  double theta0 = 0;
  double r0 = 10;
  double phi0 = vnl_math::pi/3;
  double phi1 = vnl_math::pi/3 * 2;
  double m0 = 0;
  double len = 30;
  dbsksp_shapelet s(x0, y0, theta0, r0, phi0, phi1, m0, len);
  double R = 30;
  double true_area_left = 
    (1.0/2) * R*R*vcl_sin(vnl_math::pi/3) - (1.0/2) * (R-r0)*(R-r0)*vnl_math::pi/3;
  double true_area_right = true_area_left;

  double true_area_front_arc = 2*vnl_math::pi/3 * r0*r0 + r0*r0*vcl_sin(2*vnl_math::pi/3) /2;

  TEST_NEAR("Compute shapelet area - left", s.area_left(), true_area_left, 1e-8);
  TEST_NEAR("Compute shapelet area - right", s.area_right(), true_area_right, 1e-8);
  TEST_NEAR("Compute shapelet area - overall", s.area(), 
    true_area_left + true_area_right, 1e-8);
  TEST_NEAR("Compute area of front arc", s.area_front_arc(), true_area_front_arc, 1e-8);

  // area of a degenerate shapelet (zero length)
  dbsksp_shapelet_sptr s_front = s.terminal_shapelet_front();
  double true_area_s_front = r0 * r0* 2*vnl_math::pi/3;
  TEST_NEAR("Compute area of front terminal shapelet", s_front->area(), 
    true_area_s_front, 1e-8);
}




//: Test closest point functions
MAIN( test_shapelet )
{
  START (" Test shapelet ");
  test_shapelet();
  SUMMARY();
}
