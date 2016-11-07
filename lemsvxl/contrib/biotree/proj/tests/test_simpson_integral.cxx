//: 
// \file   test_simpson_integral.cxx
// \brief  test suite for the computation of the integrant by adaptive simpson method
// \author H. Can Aras
// \date   2006-09-05
// 
#include <testlib/testlib_test.h>
#include <proj/bioproj_nu_g_filter.h>
#include <vcl_fstream.h>
#include <vcl_cmath.h>
#include <vnl/vnl_math.h>
#include <proj/error_functions.h>

#include <vnl/vnl_analytic_integrant.h>
#include <vnl/algo/vnl_simpson_integral.h>
#include <vnl/algo/vnl_adaptsimpson_integral.h>
#include <vnl/vnl_double_3.h>

class my_integrant : public vnl_analytic_integrant
{
public:
  my_integrant(double sigma, double xi) : sigma_(sigma), xi_(xi)
  { 
  }

  double f_(double zeta)
  {
    return vcl_exp(-(vcl_pow(xi_,2.0) - vcl_pow(zeta,2.0)) / (2*vcl_pow(sigma_,2.0)));
  }
protected:
  double sigma_;
  double xi_;
};

void test_simpson_integral()
{
  vnl_adaptsimpson_integral simpson_integral;
  my_integrant f1(1.0, 1.0);
  TEST_NEAR("test integral value for xi = 1 and sigma = 1", 
    simpson_integral.integral(&f1, 0.0, 1.0, 1e-6), 0.72477845915381, 1e-6);

  my_integrant f2(1.0, 2.0);
  TEST_NEAR("test integral value for xi = 2 and sigma = 1", 
    simpson_integral.integral(&f2, 0.0, 2.0, 1e-6), 0.63998864308930, 1e-6);

  my_integrant f3(1.0, 3.0);
  TEST_NEAR("test integral value for xi = 3 and sigma = 1", 
    simpson_integral.integral(&f3, 0.0, 3.0, 1e-6), 0.39316687918276, 1e-6);

  my_integrant f10(1.0, 10.0);
  TEST_NEAR("test integral value for xi = 10 and sigma = 1", 
    simpson_integral.integral(&f10, 0.0, 10.0, 1e-6), 0.10103161565077, 1e-6);

#if 0 // DEBUGGING CODE
  double sigma = 0.5;
  int extent = 500;
  int size = int(sigma*extent);

  for(int xi=-size; xi <= size; xi++)
  {
    double integral_val = 0;
    if(xi)
    {
      my_integrant f(sigma, double(xi));
      vnl_adaptsimpson_integral simpson_integral;
      integral_val = simpson_integral.integral(&f, 0.0, double(xi), 1e-6);
    }
    double term1 = 1 / (vnl_math::pi * (vcl_pow(sigma,2.0)));
    double term2 = xi / (vcl_pow(sigma,2.0));
    double val = term1 * (1 - term2 * integral_val);
    vcl_cout << val << vcl_endl;
  }
#endif
}

TESTMAIN(test_simpson_integral);
