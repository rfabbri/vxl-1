//: 
// \file   test_error_function.cxx
// \brief  test suite for error_functions class
// \author H. Can Aras
// \date   2006-08-30
// 
#include <testlib/testlib_test.h>
#include <proj/bioproj_nu_g_filter.h>
#include <vcl_fstream.h>
#include <vcl_cmath.h>
#include <vnl/vnl_math.h>
#include <proj/error_functions.h>

void test_error_function()
{
  error_functions err;
  TEST_NEAR("test erfi(1)", err.erfi(1, 150), 1.650425759, 1e-03);
  TEST_NEAR("test erfi(2)", err.erfi(2, 150), 18.56480241, 1e-03);
  TEST_NEAR("test erfi(3)", err.erfi(3, 150), 1629.994623, 1e-03);
}

TESTMAIN(test_error_function);
