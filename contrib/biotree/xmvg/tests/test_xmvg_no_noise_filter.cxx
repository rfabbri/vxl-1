//: 
// \file     test_no_noise_filter.cxx
// \brief    testing for no noise filter
// \author   H. C. Aras
// \date     2005-28-08
// 
//  This class inherits from the vpgl_perspective_camera 

#include <testlib/testlib_test.h>
#include <xmvg/xmvg_no_noise_filter_descriptor.h>
#include <xmvg/xmvg_no_noise_filter_3d.h>
#include <vcl_iostream.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_double_3.h>
#include <vbl/vbl_array_3d.h>
#include <vcl_ctime.h>

static void test_xmvg_no_noise_filter()
{
  vcl_cout << "---test descriptor---" << vcl_endl;
  // Filter at the centre and along the z-axis
  vcl_cout << "Along z-axis" << vcl_endl;
  double radius = 1.0;
  double length = 4.0;
  vgl_point_3d<double> centre(0.0, 0.0, 0.0);
  vgl_vector_3d<double> orientation(0.0, 0.0, 5.0);
  xmvg_no_noise_filter_descriptor nnfd(radius, length, centre, orientation);
  TEST("name test", nnfd.name(), "no_noise_filter");
  TEST_NEAR("inner radius test", nnfd.inner_radius(), 1.0, 1e-06);
  TEST_NEAR("length test", nnfd.length(), 4.0, 1e-06);
  TEST_NEAR("orientation vector unit test", nnfd.orientation().length(), 1.0, 1e-06);
  TEST_NEAR("box dimension test w", nnfd.box().width(), vcl_sqrt(2.0)*2.0, 1e-06);
  TEST_NEAR("box dimension test h", nnfd.box().height(), vcl_sqrt(2.0)*2.0, 1e-06);
  TEST_NEAR("box dimension test d", nnfd.box().depth(), 4.0, 1e-06);
  TEST("box test centroid test", nnfd.box().centroid(), vgl_point_3d<double>(0.0,0.0,0.0));
  // Filter at the centre and along the y-axis
  vcl_cout << "Along y-axis" << vcl_endl;
  vgl_vector_3d<double> orientation2(0.0, 5.0, 0.0);
  xmvg_no_noise_filter_descriptor nnfd2(radius, length, centre, orientation2);
  vcl_cout << nnfd2.box() << vcl_endl;
  TEST_NEAR("rotation angle test", nnfd2.rotation_angle(), double(vnl_math::pi_over_2), 1e-06);
  TEST_NEAR("rotation axis test-x", nnfd2.rotation_axis().get(0), -1.0, 1e-06);
  TEST_NEAR("rotation axis test-y", nnfd2.rotation_axis().get(1), 0.0, 1e-06);
  TEST_NEAR("rotation axis test-z", nnfd2.rotation_axis().get(2), 0.0, 1e-06);
  // Filter at the centre and along the x-axis
  vcl_cout << "Along x-axis" << vcl_endl;
  vgl_vector_3d<double> orientation3(4.0, 0.0, 0.0);
  xmvg_no_noise_filter_descriptor nnfd3(radius, length, centre, orientation3);
  TEST_NEAR("rotation angle test", nnfd3.rotation_angle(), double(vnl_math::pi_over_2), 1e-06);
  TEST_NEAR("rotation axis test-x", nnfd3.rotation_axis().get(0), 0.0, 1e-06);
  TEST_NEAR("rotation axis test-y", nnfd3.rotation_axis().get(1), 1.0, 1e-06);
  TEST_NEAR("rotation axis test-z", nnfd3.rotation_axis().get(2), 0.0, 1e-06);
  
  vcl_cout << "---test filter 3d line integral---" << vcl_endl;
  xmvg_no_noise_filter_3d nnf(nnfd);
  vcl_cout <<  "All below tests are along rays in the xz-plane" << vcl_endl;
  TEST_NEAR("line integral test-1",
    nnf.line_integral(vgl_point_3d<double>(-2.0, 0.0, 0.0), vgl_vector_3d<double>(1.0, 0.0, 0.0), nnfd.inner_radius()), 2.0, 1e-06);
  TEST_NEAR("line integral test-2",
    nnf.line_integral(vgl_point_3d<double>(-2.0, 0.0, 0.0), vgl_vector_3d<double>(4.0, 0.0, 2.0), nnfd.inner_radius()), vcl_sqrt(5.0), 1e-06);
  TEST_NEAR("line integral test-3",
    nnf.line_integral(vgl_point_3d<double>(-2.0, 0.0, 0.0), vgl_vector_3d<double>(100.0, 0.0, -100.0), nnfd.inner_radius()), vcl_sqrt(2.0), 1e-06);
  TEST_NEAR("line integral test-4",
    nnf.line_integral(vgl_point_3d<double>(-2.0, 0.0, 0.0), vgl_vector_3d<double>(2.0, 0.0, 4.0), nnfd.inner_radius()), 0, 1e-06);
  TEST_NEAR("line integral test-5",
    nnf.line_integral(vgl_point_3d<double>(-2.0, 0.0, 0.0), vgl_vector_3d<double>(0.0, 0.0, -1.0), nnfd.inner_radius()), 0, 1e-06);
  TEST_NEAR("line integral test-6",
    nnf.line_integral(vgl_point_3d<double>(-1.0, 0.0, -3.0), vgl_vector_3d<double>(0.0, 0.0, 1.0), nnfd.inner_radius()), 4, 1e-06);
  TEST_NEAR("line integral test-7",
    nnf.line_integral(vgl_point_3d<double>(-1.0, 0.0, -3.0), vgl_vector_3d<double>(1.0, 0.0, 4.0), nnfd.inner_radius()), vcl_sqrt(17.0), 1e-06);
  TEST_NEAR("line integral test-8",
    nnf.line_integral(vgl_point_3d<double>(-1.0, 0.0, -3.0), vgl_vector_3d<double>(1.0, 0.0, 2.0), nnfd.inner_radius()), vcl_sqrt(11.25), 1e-06);
  TEST_NEAR("line integral test-9",
    nnf.line_integral(vgl_point_3d<double>(-1.0, 0.0, -3.0), vgl_vector_3d<double>(2.0, 0.0, 2.0), nnfd.inner_radius()), vcl_sqrt(2.0), 1e-06);
  TEST_NEAR("line integral test-10",
    nnf.line_integral(vgl_point_3d<double>(-1.0, 0.0, -3.0), vgl_vector_3d<double>(4.0, 0.0, 2.0), nnfd.inner_radius()), 0, 1e-06);
  TEST_NEAR("line integral test-11",
    nnf.line_integral(vgl_point_3d<double>(-1.0, 0.0, -3.0), vgl_vector_3d<double>(1.0, 0.0, 0.0), nnfd.inner_radius()), 0, 1e-06);

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
  double f_radius = 0.02;
  double f_length = 0.08;
  vgl_point_3d<double> f_centre(0.0, 0.0, 0.0);
  vgl_vector_3d<double> f_orientation(0.0, 0.0, 1.0);
  xmvg_no_noise_filter_descriptor fd(f_radius, f_length, f_centre, f_orientation);
  // construct the filter
  xmvg_no_noise_filter_3d f(fd);

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

  TEST("splat result x-size test", res.size().get(0), 21);
  TEST("splat result y-size test", res.size().get(1), 29);
  TEST("splat result x-location test", res.location().get(0), 100 - 10);
  TEST("splat result y-location test", res.location().get(1), 100 - 14);
  TEST_NEAR("splat result center value test", res.weights().get(10,14), (4 - 2 * vcl_sqrt(2.0)) * f_radius, 1e-09);

  double sum = 0;
  res.eliminate_residue_effect();
  int sx = res.size().get(0);
  int sy = res.size().get(1);
  for(int i=0; i<sx; i++)
    for(int j=0; j<sy; j++)
      sum += res.weights().get(i,j);
  vcl_cout << "Weights sum before eliminating residue: " << res.weights_sum() << vcl_endl;
  vcl_cout << "Weights sum after eliminating residue: " << sum << vcl_endl;
  TEST_NEAR("eliminating splat residue test", sum, 0, 1e-09);

  //write as ppm to view
//  FILE *fp;
//  fp = vcl_fopen("F:\\MyDocs\\Temp\\file.ppm", "w");
//  vcl_fprintf(fp,"P3\n%d %d\n255\n", sx, sy);
//  for(int j=0; j<sy; j++)
//  {
//    for(int i=0; i<sx; i++)
//    {
//      if(res.weights().get(i,j) < 0)
//        vcl_fprintf(fp, "%d %d %d ", 0, 0, int(-5000*res.weights().get(i,j)));
//      else
//        vcl_fprintf(fp, "%d %d %d ", int(5000*res.weights().get(i,j)), 0, 0);
//    }
//    vcl_fprintf(fp, "\n");
//  } 
//  vcl_fclose(fp);
}

TESTMAIN(test_xmvg_no_noise_filter);
