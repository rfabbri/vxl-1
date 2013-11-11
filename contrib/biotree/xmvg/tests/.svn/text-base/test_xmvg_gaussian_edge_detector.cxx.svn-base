//: 
// \file     test_gaussian_edge_detector.cxx
// \brief    testing for gaussian edge detector
// \author   H. C. Aras
// \date     2005-03-21
// 

#include <testlib/testlib_test.h>
#include <xmvg/xmvg_gaussian_edge_detector_descriptor.h>
#include <xmvg/xmvg_gaussian_edge_detector_x_3d.h>
#include <vcl_iostream.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_double_3.h>
#include <vbl/vbl_array_3d.h>
#include <vcl_ctime.h>

static void test_xmvg_gaussian_edge_detector()
{
  vcl_cout << "---test gaussian edge detector descriptor---" << vcl_endl;
  double sigma = 0.1;
  vgl_point_3d<double> centre(0.0, 0.0, 0.0);
  xmvg_gaussian_edge_detector_descriptor gedd(sigma, centre);
  TEST("name test", gedd.name(), "gaussian_edge_detector");
  TEST_NEAR("inner radius test", gedd.sigma(), 0.1, 1e-06);
  TEST_NEAR("box dimension test w", gedd.box().width() , 8 * 0.1, 1e-06);
  TEST_NEAR("box dimension test h", gedd.box().height(), 8 * 0.1, 1e-06);
  TEST_NEAR("box dimension test d", gedd.box().depth() , 8 * 0.1 , 1e-06);
  TEST("box test centroid test", gedd.box().centroid(), vgl_point_3d<double>(0.0,0.0,0.0));

  
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
  

  //rotate the camera
  double theta = (vnl_math::pi/2);//90 degrees
//  theta = 0;
  //z axis is the rotation axis
  vnl_double_3 axis(0.0, 0.0, 1.0);
  axis.normalize();
  vgl_h_matrix_3d<double> tr;
  tr.set_identity();
  //use minus theta since the postmultiply operation is equivalent to a rotation of 
  //the coordinate frame not the camera
  tr.set_rotation_about_axis(axis, -theta);
  
  xmvg_perspective_camera<double> cam_rot = xmvg_perspective_camera<double>::postmultiply(cam, tr);
  vcl_cout << cam_rot.camera_center() << vcl_endl;
  //construct the edge detector
  xmvg_gaussian_edge_detector_x_3d ged(gedd);

  clock_t t1,t2;
  t1 = vcl_clock();
  xmvg_atomic_filter_2d<double> res(ged.splat(vgl_point_3d<double> (0.0, 0.0, 0.0), cam_rot));
  t2 = vcl_clock();
  double tt = (double)(t2-t1) / CLOCKS_PER_SEC;
  vcl_cout << "Elapsed time is: " << tt << vcl_endl;
  vcl_cout << "Splat location is: " << res.location() << vcl_endl;
  vcl_cout << "Splat size is: " << res.size() << vcl_endl;
  //unused variables
  /*int sx = res.size().get(0);
  int sy = res.size().get(1);*/
  

#if 0
  //write as ppm to view
  FILE *fp;
  fp = vcl_fopen("D:\\MyDocs\\Temp\\file.ppm", "w");
  vcl_fprintf(fp,"P3\n%d %d\n5000\n", sx, sy);
  for(int j=0; j<sy; j++)
  {
    vcl_cout << j << vcl_endl;
    for(int i=0; i<sx; i++)
    {
      if(res.weights().get(i,j) < 0)
        vcl_fprintf(fp, "%d %d %d ", 0, 0, int(-10000*res.weights().get(i,j)));
      else
        vcl_fprintf(fp, "%d %d %d ", int(10000*res.weights().get(i,j)), 0, 0);
    }
    vcl_fprintf(fp, "\n");
  } 
  vcl_fclose(fp);
#endif
}

TESTMAIN(test_xmvg_gaussian_edge_detector);
