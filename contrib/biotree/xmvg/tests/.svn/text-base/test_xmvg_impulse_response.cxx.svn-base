//: 
// \file     test_xmvg_impulse_response.cxx
// \brief    testing for impulse response of a Gaussian filter
// \author   H. Can Aras
// \date     May'2006
//

#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <xscan/xscan_dummy_scan.h>
#include <xscan/xscan_scan.h>
#include <xmvg/xmvg_gaussian_filter_descriptor.h>
#include <xmvg/xmvg_gaussian_filter_3d.h>

static void test_xmvg_impulse_response()
{
  vcl_cout << "---test descriptor---" << vcl_endl;
  vcl_stringstream ss;
  ss << "dummy";
  xscan_dummy_scan scan(10, 120, 160, vnl_int_2(256, 256), vnl_double_2(128, 128),
    vgl_point_3d<double>(0.0, -120.0, 0.0), vnl_double_3(.0, .0, 1.), 
    vnl_math::pi/180, 360,  ss.str().c_str());

  xscan_scan const & scan_ref = dynamic_cast<xscan_scan const&>(scan);

  xmvg_gaussian_filter_descriptor filter_desc(0.05, 0.1, vgl_point_3d<double> (0.0,0.0,0.0), vgl_vector_3d<double> (1.0,0.0,0.0));
  xmvg_gaussian_filter_3d filter(filter_desc);

  for(double disp = 0; disp <= 0.3; disp += 0.01)
  {
    double sum = 0;
    for(orbit_index t=0; t < 360; t++)
    {
      xmvg_perspective_camera<double> cam = scan_ref(t);
      double val = filter.splat_impulse(vgl_point_3d<double> (0.0, 0.0, 0.0), cam, vgl_point_3d<double> (0.0, disp, 0.0));
//      vcl_cout << t << " "  << val << vcl_endl;
      sum += val;
    }
    vcl_cout << sum/360 << vcl_endl;
  }
}

TESTMAIN(test_xmvg_impulse_response);
