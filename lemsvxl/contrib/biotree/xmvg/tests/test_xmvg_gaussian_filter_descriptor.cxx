//: 
// \file     test_gaussian_filter.cxx
// \brief    testing for gaussian filter
// 

#include <testlib/testlib_test.h>
#include <xmvg/xmvg_gaussian_filter_descriptor.h>
#include <xmvg/xmvg_gaussian_filter_3d.h>
#include <vcl_iostream.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_double_3.h>
#include <vbl/vbl_array_3d.h>
#include <vcl_ctime.h>

static void test_xmvg_gaussian_filter_descriptor()
{
  vcl_cout << "---test descriptor---" << vcl_endl;
  // Filter at the centre and along the z-axis
  vcl_cout << "Along z-axis" << vcl_endl;
  double sigma_r = 5;
  double length = 10;
  vgl_point_3d<double> centre(0.0, 0.0, 0.0);
  vgl_vector_3d<double> orientation(0.7071, 0.5, 0.7071);

orientation = normalize(orientation);

  xmvg_gaussian_filter_descriptor gfd(sigma_r, length, centre, orientation);
 
  TEST_NEAR("box dimension test w", gfd.box().width() ,43.1623, 1e-04);
  TEST_NEAR("box dimension test h", gfd.box().height(),42.2361, 1e-04);
  TEST_NEAR("box dimension test d", gfd.box().depth() , 30 , 1e-04);
  TEST("box test centroid test", gfd.box().centroid(), vgl_point_3d<double>(0.0,0.0,0.0));
 

  
}

TESTMAIN(test_xmvg_gaussian_filter_descriptor);
        
