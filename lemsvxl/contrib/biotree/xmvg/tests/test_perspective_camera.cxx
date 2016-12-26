#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_double_3.h> 
#include <xmvg/xmvg_source.h>
#include <xmvg/xmvg_perspective_camera.h>
static void test_perspective_camera()
{
  //test dummy_scan
  vcl_cout << "Testing constructor\n";
  xmvg_source source;
  //Construct the camera for theta =0 using sky scan data
  vnl_double_3x3 m(0.0);
  m[0][0]=2022.5;m[0][1]=0;m[0][2]=1000;
  m[1][1]=2022.5;m[1][2]=524;
  m[2][2]=1;

  vpgl_calibration_matrix<double> K(m);
  vnl_double_4x4 rot(0.0);
  rot[0][1]=1;
  rot[1][2]=-1;
  rot[2][0]=-1;
  rot[3][3]=1;
  vgl_h_matrix_3d<double> R(rot), Rinv = R.get_inverse();

  vgl_homg_point_3d<double> pre_center(0,0,-18.0), center = Rinv(pre_center);
  
  xmvg_perspective_camera<double> P(K, center, R, source);

  vcl_cout << P << '\n';

  vgl_homg_point_3d<double> X(0,17.8/2, 9.327/2);
    
  vgl_homg_point_2d<double> hu = P.project(X);
  vcl_cout << X << '\n' << hu << '\n';
  TEST_NEAR("test x projection", hu.x()/hu.w(), 2000, 0.1);
  TEST_NEAR("test y projection", hu.y()/hu.w(), 0.0, 0.1);

 //rotation angle in radians
  double theta = vnl_math::pi/4;//45 degrees
  //z axis is the rotation axis
  vnl_double_3 axis(0.0, 0.0, 1.0);
  vgl_h_matrix_3d<double> tr;
  tr.set_identity();
  tr.set_rotation_about_axis(axis, theta);
  vcl_cout <<"Rotation Matrix \n" << tr << '\n';
  
  xmvg_perspective_camera<double> P_rot = 
    xmvg_perspective_camera<double>::postmultiply(P, tr); 
   
  vcl_cout << "P_rot\n" << P_rot << '\n';
  vgl_point_2d<double> ur = P_rot.project(X);
  vcl_cout << X << '\n' << ur << '\n';
  TEST_NEAR("test rotation projection x", ur.x(), 1523.94, 0.01);
  TEST_NEAR("test rotation projection y", ur.y(), 135.747, 0.01);

  // Test constructor with the image dimensions
  vnl_int_2 dim(2000, 1048);
  xmvg_perspective_camera<double> P2(K, center, R, source, dim);
  TEST("Contructor with image dimensions test-1", P2.get_img_dim().get(0), 2000);
  TEST("Contructor with image dimensions test-2", P2.get_img_dim().get(1), 1048);
  TEST("Contructor with image dimensions test-3", P.get_img_dim().get(0), 1024);
  TEST("Contructor with image dimensions test-4", P.get_img_dim().get(1), 1024);
}

TESTMAIN(test_perspective_camera);
