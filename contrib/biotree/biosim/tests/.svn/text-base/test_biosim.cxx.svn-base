#include <testlib/testlib_test.h>
#include <biosim/biosim_ctsim.h>

MAIN( test_biosim )
{
  START ("Can's CT Simulation");
  vcl_cout << "---Testing Default Constructor---" << vcl_endl;
  biosim_ctsim ctsim;
  TEST_NEAR("pixel size", ctsim.pix_size(), 10, 1e-06);
  TEST_NEAR("source to origin distance", ctsim.src_org_d(), 40, 1e-06);
  TEST_NEAR("source to sensor distance", ctsim.src_sns_d(), 160, 1e-06);
  TEST("sensor dimension-x", ctsim.sns_dim().get(0), 256);
  TEST("sensor dimension-y", ctsim.sns_dim().get(1), 256);
  TEST_NEAR("principal point-x", ctsim.prn_pnt().get(0), 128.0, 1e-06);
  TEST_NEAR("principal point-y", ctsim.prn_pnt().get(1), 128.0, 1e-06);
  TEST("initial source position", ctsim.init_src_pos(), vgl_point_3d<double>(40.0,0.0,0.0));
  TEST("rotation axis", ctsim.rot_axis(), vnl_double_3(0.0,0.0,1.0));
  TEST_NEAR("rotation step size", ctsim.rot_step_size(), 1, 1e-06);
  TEST("number of views", ctsim.num_views(), 360);
  xmvg_perspective_camera<double> cam(ctsim.construct_camera(90));
  ctsim.scan("input.txt");
  TEST_NEAR("scan result", ctsim.data(128,128,0), 0.1, 1e-06);
  TEST_NEAR("scan result", ctsim.data(128,128,90), 0.04, 1e-06);
  TEST_NEAR("scan result", ctsim.data(128,128,180), 0.1, 1e-06);
  TEST_NEAR("scan result", ctsim.data(128,128,270), 0.04, 1e-06);

  vcl_cout << "---Testing Constructor with Parameters---" << vcl_endl;
  double pixel_size = 10; 
  double source_origin_dist = 40;
  double source_sensor_distance = 160;
  vnl_int_2 sensor_dimensions(200,200);
  vnl_double_2 principal_point(100.0, 100.0);
  vgl_point_3d<double> initial_source_position(40.0, 0.0, 0.0);
  vnl_double_3 rotation_axis(0.0, 0.0, 1.0);
  double rotation_step_size = 2;
  biosim_ctsim ctsim2(pixel_size, 
                      source_origin_dist, 
                      source_sensor_distance, 
                      sensor_dimensions, 
                      principal_point, 
                      initial_source_position, 
                      rotation_axis,
                      rotation_step_size,
                      "from_file");
  TEST_NEAR("pixel size", ctsim2.pix_size(), 10, 1e-06);
  TEST_NEAR("source to origin distance", ctsim2.src_org_d(), 40, 1e-06);
  TEST_NEAR("source to sensor distance", ctsim2.src_sns_d(), 160, 1e-06);
  TEST("sensor dimension-x", ctsim2.sns_dim().get(0), 200);
  TEST("sensor dimension-y", ctsim2.sns_dim().get(1), 200);
  TEST_NEAR("principal point-x", ctsim2.prn_pnt().get(0), 100.0, 1e-06);
  TEST_NEAR("principal point-y", ctsim2.prn_pnt().get(1), 100.0, 1e-06);
  TEST("initial source position", ctsim2.init_src_pos(), vgl_point_3d<double>(40.0,0.0,0.0));
  TEST("rotation axis", ctsim2.rot_axis(), vnl_double_3(0.0,0.0,1.0));
  TEST_NEAR("rotation step size", ctsim2.rot_step_size(), 2, 1e-06);
  TEST("number of views", ctsim2.num_views(), 180);

//  ctsim2.scan("F:\\MyDocs\\Temp\\input.txt");
//  ctsim2.write_data_2d("F:\\MyDocs\\Temp\\toy_data", "toy");

//  vcl_cout << "---Testing Constructor from Dummy Scan of KK---" << vcl_endl;
//  xscan_dummy_scan scan(10, 40, 160, vnl_int_2(200, 200), vnl_double_2(100, 100),
//      vgl_point_3d<double>(40.0, 0.0, 0.0), vnl_double_3(.0, .0, 1.), 
//      2*vnl_math::pi/180, 5,  "dummy");
//  xscan_dummy_scan dummy_scan;
//  biosim_ctsim ctsim3(scan, "no_noise");
//  ctsim3.scan("F:\\MyDocs\\Temp\\input.txt");
//  ctsim3.write_data_2d("F:\\MyDocs\\Temp\\toy_data", "toy");
  SUMMARY();
}
