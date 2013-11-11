// This is shp/dbsksp/tests/test_xshock_fragment.cxx

// \author Nhon Trinh
// \date Sep 22, 2008

#include <testlib/testlib_test.h>
#include <dbsksp/dbsksp_xshock_fragment.h>
#include <vnl/vnl_math.h>


double angle_difference(double a, double b)
{
  vgl_vector_2d<double > va(vcl_cos(a), vcl_sin(a));
  vgl_vector_2d<double > vb(vcl_cos(b), vcl_sin(b));
  double diff = vnl_math_abs(signed_angle(va, vb));
  return diff;
}

// ----------------------------------------------------------------------------
void test_xshock_fragment()
{
  dbsksp_xshock_node_descriptor start(100, 0, 0, vnl_math::pi * 2/3, 50);
  dbsksp_xshock_node_descriptor end(200, 0, 0, vnl_math::pi *2/3, 100);
  dbsksp_xshock_fragment xfrag(start, end);

  // compute are of fragment formed by two line segments
  double computed_area = xfrag.area();
  double true_area = 100 * 100 * vcl_sqrt(3.0) * 3/4;
  TEST_NEAR("simple xshock_fragment area", (computed_area-true_area) / true_area, 0, 1e-2);

  // compute area of a terminal edge
  dbsksp_xshock_node_descriptor start2(0, 0, 0, vnl_math::pi/3, 100);
  dbsksp_xshock_node_descriptor end2(0, 0, 0, 0, 100);
  dbsksp_xshock_fragment xfrag2(start2, end2);

  // compute are of fragment formed by two line segments
  double computed_area2 = xfrag2.area();
  double true_area2 = 100 * 100 * vnl_math::pi / 3;

  vcl_cout << "Computed area2 = " << computed_area2 << ".\n"
    << "True area2 = " << true_area2 << ".\n";

  TEST_NEAR("are of terminal xfragment", vnl_math_abs(computed_area2-true_area2) / true_area2, 0, 1e-2);
}




//: Test closest point functions
MAIN( test_xshock_fragment )
{
  START (" Test xshock_fragment ");
  test_xshock_fragment();
  SUMMARY();
}
