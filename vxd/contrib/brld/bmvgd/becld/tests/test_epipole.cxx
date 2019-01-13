#include <testlib/testlib_test.h>
#include <becld/becld_epipole.h>
#include <becld/becld_epipole_sptr.h>
#include <vnl/vnl_math.h>
#include <iostream>


static bool approx_equal(double a, double b)
{
  return std::abs(b-a) < 1e-14;
}

MAIN( test_epipole )
{
  START ("epipole");

  // make an epipole
  becld_epipole_sptr epipole = new becld_epipole(-10.0,20.0);
  std::cout << "Epipole at (-10.0, 20.0)" << std::endl;
  

  // test the distance function
  double s = epipole->distance(6.0,32.0);
  std::cout << "distance to (6.0, 32.0) = " << s << std::endl;
  TEST("Distance", s, 20.0);


  // test the angle function
  double a = epipole->angle(10.0,20.0);
  std::cout << "angle at (10.0,20.0) = " << a << std::endl;
  TEST("Angle (0 deg)",a, 0.0);

  a = epipole->angle(-5.0,25.0);
  std::cout << "angle at (-5.0,25.0) = " << a << std::endl;
  TEST("Angle (+45 deg)",approx_equal(a, vnl_math::pi_over_4), true);

  a = epipole->angle(0.0,10.0);
  std::cout << "angle at (0.0,10.0) = " << a << std::endl;
  TEST("Angle (-45 deg)",approx_equal(a, -vnl_math::pi_over_4), true);

  a = epipole->angle(-10.0 - 1.0/std::sqrt(3.0), 21.0);
  std::cout << "angle at (-10.0 - 1.0/sqrt(3.0), 21.0) = " << a << std::endl;
  TEST("Angle (150 deg)",approx_equal(a, (vnl_math::pi)*2.0/3.0), true);

  SUMMARY();
}
