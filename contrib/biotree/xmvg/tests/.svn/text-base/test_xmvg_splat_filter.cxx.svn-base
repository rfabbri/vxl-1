//: 
// \file     test_splat_filter.cxx
// \brief    testing for splat filters
// \author   H. Can Aras
// \date     2005-12-02
// 

#include <testlib/testlib_test.h>
#include <xmvg/xmvg_splat_filter.h>
#include <vcl_iostream.h>
#include <vnl/vnl_math.h>

static void test_xmvg_splat_filter()
{
  vcl_cout << "TESTING THE SPLAT FILTER" << vcl_endl;
  xmvg_splat_filter sf(10, 2.0, RAMLAK);
  TEST("kernel size test", sf.kernel_size(), 10);
  TEST("full kernel size test", sf.full_kernel_size(), 21);
  TEST("cutoff frequency test", sf.cutoff_freq(), 2.0);
  vcl_vector<double> kernel = sf.kernel();
  TEST_NEAR("k=0 kernel value test", kernel[10], 4.0, 1e-12);
  TEST_NEAR("k=even kernel value test", kernel[12], 0.0, 1e-12);
  TEST_NEAR("k=even kernel value test", kernel[8], 0.0, 1e-12);
  TEST_NEAR("k=odd kernel value test", kernel[11], -16.0/(pow(vnl_math::pi,2.0)), 1e-12);
  TEST_NEAR("k=odd kernel value test", kernel[9], -16.0/(pow(vnl_math::pi,2.0)), 1e-12);
}

TESTMAIN(test_xmvg_splat_filter);
