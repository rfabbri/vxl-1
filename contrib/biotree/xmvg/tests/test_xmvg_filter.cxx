//: 
// \file  test_xmvg_filter.cxx
// \brief  testing suite for filter_2d, filter_3d, filter_descriptor
// \author    Kongbin Kang
// \date        2005-02-08
// 
#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <xmvg/xmvg_filter_descriptor.h>

static void test_xmvg_filter()
{

  vcl_cout << "-----------------testing filter_descriptor------------------\n";
  vgl_box_3d<double> box(0.0 ,0.0, 0.0, 1.0, 2.0, 3.0);
  
  vcl_string name("testing descriptor");
  
  xmvg_filter_descriptor fd(box, name);

  vcl_cout << fd.name() << "\n";
  TEST_NEAR("dummy test", 1.0, 1.0, 1e-06);  
}

TESTMAIN(test_xmvg_filter);
