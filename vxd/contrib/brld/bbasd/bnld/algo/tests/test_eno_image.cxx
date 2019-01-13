#include <testlib/testlib_test.h>
#include <bnld/algo/bnld_eno_image.h>
#include <bnld/algo/bnld_eno_zerox_image.h>
#include <iostream>
#include <limits>
#include <vil/vil_fill.h>
#include <vil/vil_print.h>



//static double p1(double x);
//static void test_interval(double x, double x_ref, bnld_eno_1d &e,double tol);

MAIN( test_eno_image )
{
  START ("eno_image");

//  double tol = std::numeric_limits<double>::epsilon()*1000;

  vil_image_view <double> image(6,5);
  vil_image_view <unsigned> label(6,5);

  image.fill(1);
  label.fill(69);

  image(2,1) = 0;
  image(1,2) = 0;
  image(2,2) = 0;
  image(3,2) = 0;
           
  image(1,3) = 0;
  image(3,3) = 0;
           
  image(1,4) = 0;
  image(2,4) = 0;
  image(3,4) = 0;
  image(5,0) = 0;

  std::cout << "original img:\n\n";

  vil_print_all(std::cout,image);

  bnld_eno_image eim;

  eim.interpolate(&image);
  eim.print();

  bnld_eno_zerox_image zim(eim);
  zim.print();
  zim.assign_labels(image,label);
  zim.print();

  SUMMARY();
}

/*

void test_interval(double x, double x_ref, bnld_eno_1d &e, double tol)
{
   std::cout << "Abscissa " << x << " in interval #" << e.interval_index(x) << std::endl;
   TEST_NEAR("Sample",e.sample(x),x_ref,tol);
}

double p1(double x)
{
   return (x-9.1)*(x-10.8);
}
*/
