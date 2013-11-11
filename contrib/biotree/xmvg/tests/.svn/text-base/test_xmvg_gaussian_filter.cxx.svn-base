//: 
// \file     test_gaussian_filter.cxx
// \brief    testing for gaussian filter
// \author   H. C. Aras
// \date     2005-03-21
// 

#include <testlib/testlib_test.h>
#include <xmvg/xmvg_gaussian_filter_descriptor.h>
#include <xmvg/xmvg_gaussian_filter_3d.h>
#include <vcl_iostream.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_double_3.h>
#include <vbl/vbl_array_3d.h>
#include <vcl_ctime.h>

static void test_xmvg_gaussian_filter()
{
  vcl_cout << "---test descriptor---" << vcl_endl;
  // Filter at the centre and along the z-axis
  vcl_cout << "Along z-axis" << vcl_endl;
  double sigma_r = 0.04;
  double length = 0.1;
  vgl_point_3d<double> centre(0.0, 0.0, 0.0);
  vgl_vector_3d<double> orientation(0.0, 0.0, 5.0);
  xmvg_gaussian_filter_descriptor gfd(sigma_r, length, centre, orientation);
  TEST("name test", gfd.name(), "gaussian_filter");
  TEST_NEAR("inner radius test", gfd.sigma_r(), 0.04, 1e-06);
  TEST_NEAR("length test", gfd.sigma_z(), 0.05, 1e-06);
  TEST_NEAR("orientation vector unit test", gfd.orientation().length(), 1.0, 1e-06);
  TEST_NEAR("box dimension test w", gfd.box().width() , 8 * 0.04, 1e-06);
  TEST_NEAR("box dimension test h", gfd.box().height(), 8 * 0.04, 1e-06);
  TEST_NEAR("box dimension test d", gfd.box().depth() , 3 * 0.1 , 1e-06);
  TEST("box test centroid test", gfd.box().centroid(), vgl_point_3d<double>(0.0,0.0,0.0));
  TEST_NEAR("rotation angle test", gfd.rotation_angle(), 0, 1e-06);
  TEST("rotation axis test", gfd.rotation_axis(), vnl_double_3(0.0, 0.0, 0.0));
//  vcl_cout << "---test filter 3d line integral---" << vcl_endl;

  vcl_cout << "---test filter 3d splat---" << vcl_endl;
  // construct the initial camera
  xmvg_source source;
  vnl_double_3x3 m(0.0);
  m[0][0]=13563.6;  m[0][1]=0;   m[0][2]=200;
  m[1][1]=13563.6;  m[1][2]=200;
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
  double f_sr = 0.04;
  double f_length = 0.1;
  double f_sz = f_length/2;
  vgl_point_3d<double> f_centre(0.0, 0.0, 0.0);
  vgl_vector_3d<double> f_orientation(0.0, 0.0, 1.0);
  xmvg_gaussian_filter_descriptor fd(f_sr, f_length, f_centre, f_orientation);
  // construct the filter
  xmvg_gaussian_filter_3d f(fd);

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

  TEST("splat result x-size test", res.size().get(0), 111);
  TEST("splat result y-size test", res.size().get(1), 105);
  TEST("splat result x-location test", res.location().get(0), 200 - 55);
  TEST("splat result y-location test", res.location().get(1), 200 - 52);
  TEST_NEAR("splat result center value test-1", res.weights().get(55,52), f_sr/(2*f_sz), 1e-6);
  TEST_NEAR("splat result center value test-2", res[55][52], f_sr/(2*f_sz), 1e-6);

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
//  vcl_fprintf(fp,"P3\n%d %d\n5000\n", sx, sy);
//  for(int j=0; j<sy; j++)
//  {
//    for(int i=0; i<sx; i++)
//    {
//      if(res.weights().get(i,j) < 0)
//        vcl_fprintf(fp, "%d %d %d ", 0, 0, int(-10000*res.weights().get(i,j)));
//      else
//        vcl_fprintf(fp, "%d %d %d ", int(10000*res.weights().get(i,j)), 0, 0);
//    }
//    vcl_fprintf(fp, "\n");
//  } 
//  vcl_fclose(fp);
}

TESTMAIN(test_xmvg_gaussian_filter);
