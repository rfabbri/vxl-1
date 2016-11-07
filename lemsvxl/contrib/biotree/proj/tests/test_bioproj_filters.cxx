//: 
// \file   test_bioproj_filters.cxx
// \brief  test suite for bioproj_filters class
// \author H. Can Aras
// \date   2006-07-13
// 
#include <testlib/testlib_test.h>
#include <proj/bioproj_nu_g_filter.h>
#include <proj/bioproj_nu_gx_filter.h>
#include <proj/bioproj_nu_gy_filter.h>
#include <proj/bioproj_nu_gxx_filter.h>
#include <proj/bioproj_nu_gyy_filter.h>
#include <proj/bioproj_nu_gxy_filter.h>
#include <proj/bioproj_g_filter.h>
#include <proj/bioproj_gz_filter.h>
#include <proj/bioproj_gzz_filter.h>
#include <vcl_fstream.h>
#include <vcl_cmath.h>
#include <vnl/vnl_math.h>

void test_bioproj_filters()
{
  double sigma = 2.0;
  double theta = 0;
  int extent = 5;

  bioproj_nu_g_filter filter1(sigma, extent);

  TEST_NEAR("test filter value", filter1.filter_[6], -0.02227979404102, 1e-06);
  TEST_NEAR("test filter value", filter1.filter_[10], 0.07957747154595, 1e-06);
  TEST_NEAR("test filter value", filter1.filter_[14], -0.02227979404102, 1e-06);

  bioproj_nu_gx_filter filter2(sigma, theta, extent);

  TEST_NEAR("test filter value", filter2.filter_[8], 0.03978873577297, 1e-06);
  TEST_NEAR("test filter value", filter2.filter_[10], 0.0, 1e-06);
  TEST_NEAR("test filter value", filter2.filter_[12], -0.03978873577297, 1e-06);

  bioproj_g_filter filter3(sigma, extent);

  TEST_NEAR("test filter value", filter3.filter_[6], 0.02699548325659, 1e-06);
  TEST_NEAR("test filter value", filter3.filter_[10], 0.19947114020072, 1e-06);
  TEST_NEAR("test filter value", filter3.filter_[14], 0.02699548325659, 1e-06);

  bioproj_gz_filter filter4(sigma, extent);

  TEST_NEAR("test filter value", filter4.filter_[8], 0.06049268112979, 1e-06);
  TEST_NEAR("test filter value", filter4.filter_[10], 0.0, 1e-06);
  TEST_NEAR("test filter value", filter4.filter_[12], -0.06049268112979, 1e-06);

  bioproj_gzz_filter filter5(sigma, extent);

  TEST_NEAR("test filter value", filter5.filter_[6], 0.02024661244245, 1e-06);
  TEST_NEAR("test filter value", filter5.filter_[10], -0.04986778505018, 1e-06);
  TEST_NEAR("test filter value", filter5.filter_[14], 0.02024661244245, 1e-06);

  bioproj_nu_gxx_filter filter6(sigma, theta, extent);

  TEST_NEAR("test filter value", filter6.filter_[3], -0.00196196864964, 1e-06);
  TEST_NEAR("test filter value", filter6.filter_[7], 0.02180727615641, 1e-06);
  TEST_NEAR("test filter value", filter6.filter_[10], -0.03978873577297, 1e-06);
  TEST_NEAR("test filter value", filter6.filter_[13], 0.02180727615641, 1e-06);
  TEST_NEAR("test filter value", filter6.filter_[17], -0.00196196864964, 1e-06);

  bioproj_nu_gyy_filter filter7(sigma, vnl_math::pi/3, extent);

  TEST_NEAR("test filter value", filter7.filter_[3], -0.00147147648723, 1e-06);
  TEST_NEAR("test filter value", filter7.filter_[7], 0.01635545711731, 1e-06);
  TEST_NEAR("test filter value", filter7.filter_[10], -0.02984155182973, 1e-06);
  TEST_NEAR("test filter value", filter7.filter_[13], 0.01635545711731, 1e-06);
  TEST_NEAR("test filter value", filter7.filter_[17], -0.00147147648723, 1e-06);

  bioproj_nu_gxy_filter filter8(sigma, vnl_math::pi/6, extent);

  TEST_NEAR("test filter value", filter8.filter_[7], -0.00944282756940, 1e-06);
  TEST_NEAR("test filter value", filter8.filter_[10],  0.01722902798193, 1e-06);
  TEST_NEAR("test filter value", filter8.filter_[13], -0.00944282756940, 1e-06);

#if 0
  int half_filter_size = int(vcl_ceil(sigma*extent));
  int full_kernel_size = 2*half_filter_size + 1;
  for(int i = 0; i < full_kernel_size; i++)
    vcl_cout << filter8.filter_[i] << vcl_endl;
#endif

}

TESTMAIN(test_bioproj_filters);
