#include <testlib/testlib_test.h>
#include <xmvg/xmvg_parallel_beam_filter_3d.h>
#include <xmvg/xmvg_perspective_camera.h>
#include <xmvg/xmvg_no_noise_filter_descriptor.h>
#include <xmvg/xmvg_pipe_filter_3d.h>
#include <xmvg/xmvg_no_noise_filter_3d.h>
#include <vnl/vnl_double_3x3.h>

static void test_xmvg_parallel_beam_filter_3d(){
  double pixel_size = 10;//in microns
  double source_sensor_dist = 3000.;//in mm
  double source_origin_dist = 3000;//in mm
  vnl_double_2 principle_pt(0.,0.);
  vgl_homg_point_3d<double> camera_center(source_origin_dist, 0.,0.);
  // calibration matrix
  vnl_double_3x3 m(0.0);
  m[0][0] = source_sensor_dist*1000 / pixel_size;
  m[0][1] = 0;
  m[0][2] = principle_pt[0];
  m[1][1] = source_sensor_dist*1000 / pixel_size;
  m[1][2] = principle_pt[1];
  m[2][2] = 1;
  vpgl_calibration_matrix<double> K(m);
  vnl_double_3 t(0.0, 0.0, 0.0);

  // the camera sits on positive part of x-axis and look towards
  // the origin
  vnl_double_3x3 R(0.0);
  R[0][1] = 1;
  R[1][2] = -1;
  R[2][0] = -1;
  vnl_double_3 r(0.,0.,0.);
    xmvg_source source;
    xmvg_perspective_camera<double> camera(K, camera_center, vgl_h_matrix_3d<double>(R, r), source);

  //filter
    vgl_vector_3d<double> f_orientation(0.0, 0.0, 1.0);
  double inner_radius = 2.0;//
  //unused variable
  //double outer_radius = sqrt(2.)*inner_radius;
  double f_length = 2.0;//shouldn't matter for this test
  vgl_point_3d<double> f_centre(0.,0.,0.);
  xmvg_no_noise_filter_descriptor fdy(inner_radius, f_length, f_centre, f_orientation);
  xmvg_pipe_filter_3d fpy(fdy);
  typedef xmvg_parallel_beam_filter_3d<xmvg_pipe_filter_3d> filter_3d_t;
  filter_3d_t fy(fpy);
  xmvg_no_noise_filter_3d traditional_filter(fdy);

  for (double x = -.1; x <= .1; x += .1){
    for (double y = -.1; y <= .1; y += .1){
      for (double z = -.1; z <= .1; z += .1){
    
        xmvg_atomic_filter_2d<double> my_splat = fy.splat(vgl_point_3d<double>(x,y,z), camera);
        xmvg_atomic_filter_2d<double> traditional_splat = traditional_filter.splat(vgl_point_3d<double>(0.,0.,0.), camera);
        vnl_int_2 filter_location = my_splat.location();
        vnl_int_2 filter_size = my_splat.size();
        for (int j = 0; j < filter_size[1]; j++){
          for (int i = 0; i < filter_size[0]; ++i){
            double value = my_splat[i ][j];
            double traditional_value = traditional_splat[i][j];
            
            TEST_NEAR("splat value", value, traditional_value, .1);
    }
  }
      }}}}

TESTMAIN(test_xmvg_parallel_beam_filter_3d);

