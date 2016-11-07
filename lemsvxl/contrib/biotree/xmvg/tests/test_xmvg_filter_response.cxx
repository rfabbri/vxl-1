//: 
// \file     test_filter_response.cxx
// \brief    testing for filter response class
// \author   H. C. Aras
// \date     2005-05-02
// 

#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <xmvg/xmvg_filter_response.h>

static void test_filter_response()
{
  xmvg_filter_response<double> resp(10, 0.0);
  vcl_cout << "testing constructor successful" << vcl_endl;
}

TESTMAIN(test_filter_response);
