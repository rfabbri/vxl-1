//: 
// \file     test_xmvg_point_filter.cxx
// \brief    testing for point filter
// \author   Kongbin Kang 
// \date     August'2005
//

#include <testlib/testlib_test.h>
#include <xmvg/xmvg_point_filter_descriptor.h>
#include <xmvg/xmvg_point_filter_3d.h>
#include <vcl_iostream.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_double_3.h>
#include <vbl/vbl_array_3d.h>
#include <vcl_ctime.h>

static void test_xmvg_point_filter()
{
  vcl_cout << "---test descriptor---" << vcl_endl;

  xmvg_point_filter_descriptor fd;

  vcl_string name = fd.name();

  vcl_string id = "point_filter";

  bool same_name = ((name == id)? true : false);

  TEST("test description name", same_name, true );
 
  xmvg_point_filter_3d filter(fd);
  
  vcl_cout << "---test filter 3d splat---" << vcl_endl;
  // construct the initial camera
  xmvg_source source;
  vnl_double_3x3 m(0.0);
  m[0][0]=13563.6;  m[0][1]=0;   m[0][2]=100;
  m[1][1]=13563.6;  m[1][2]=100;
  m[2][2]=1;
  vpgl_calibration_matrix<double> K(m);
  vnl_double_4x4 rot(0.0);
  rot[0][1]=1;
  rot[1][2]=-1;
  rot[2][0]=-1;
  rot[3][3]=1;
  vgl_h_matrix_3d<double> R(rot), Rinv = R.get_inverse();
  vgl_homg_point_3d<double> center(40.0,0.0,0.0);
  xmvg_perspective_camera<double> cam(K, center, R, source);

  // construct the filter descriptor
 // construct the filter
  xmvg_point_filter_3d f(fd);

  //rotate the camera
  double theta = (vnl_math::pi/2);//90 degrees
  //z axis is the rotation axis
  vnl_double_3 axis(0.0, 0.0, 1.0);
  axis.normalize();
  vgl_h_matrix_3d<double> tr;
  tr.set_identity();
  //use minus theta since the postmultiply operation is equivalent to a rotation of 
  //the coordinate frame not the camera
  tr.set_rotation_about_axis(axis, -theta);
  xmvg_perspective_camera<double> cam_rot = xmvg_perspective_camera<double>::postmultiply(cam, tr);
  clock_t t1,t2;
  t1 = vcl_clock();
  xmvg_atomic_filter_2d<double> res(f.splat(vgl_point_3d<double> (0.0, 0.0, 0.0), cam_rot));
  t2 = vcl_clock();
  double tt = (double)(t2-t1) / CLOCKS_PER_SEC;
  vcl_cout << "Elapsed time is: " << tt << vcl_endl;
  vcl_cout << "Splat size is: " << res.size() << vcl_endl;

  TEST("splat result x-size test", res.size().get(0), 1);
  TEST("splat result y-size test", res.size().get(1), 1);
  TEST("splat result x-location test", res.location().get(0), 100);
  TEST("splat result y-location test", res.location().get(1), 100);
  TEST_NEAR("splat result center value test", res.weights().get(0,0), 1, 1e-09);

}

TESTMAIN(test_xmvg_point_filter);
