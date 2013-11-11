#include <testlib/testlib_test.h>
#include <biocbs/biocbs_scan.h>
#include <vnl/vnl_math.h>

MAIN( test_biocbs )
{
  START ("Cone-Beam Simulator");

  unsigned short fill_value = 0;
  vbl_array_3d<unsigned short> vol(10, 20, 30, fill_value);
  vol(5,10,15) = 1;
  
  biocbs_scan cbs(46.8, 261.5, 345.712, vnl_int_2(200,200),
    vnl_double_2(100.0,100.0), vgl_point_3d<double> (0.0, -261.5, 0.0),
    vnl_double_3(0.0, 0.0, 1.0), 2*vnl_math::pi/400, 400,
    35.4, &vol, vgl_point_3d<double> (0.0, 0.0, 0.0), NO_INTERP);

  TEST("Sensor pixel size test", cbs.sensor_pixel_size(), 46.8);
  TEST("Source to origin distance test", cbs.source_origin_dist(), 261.5);
  TEST("Source to sensor distance test", cbs.source_sensor_dist(), 345.712);
  TEST("Sensor dimensions test", cbs.sensor_dim(), vnl_int_2(200, 200));
  TEST("Principal point test", cbs.principal_point(), vnl_double_2(100.0, 100.0));
  TEST("Initial source position test", cbs.init_sorce_pos(), 
       vgl_point_3d<double> (0.0, -261.5, 0.0));
  TEST("Rotation axis test", cbs.rot_axis(), vnl_double_3 (0.0, 0.0, 1.0));
  TEST("Rotation step angle test", cbs.rot_step_angle(), 2*vnl_math::pi/400);
  TEST("Number of views test", cbs.number_of_views(), 400);
  TEST("Grid resolution test", cbs.grid_resolution(), 35.4);
  TEST("Volume center position test", cbs.vol_center_pos(), 
       vgl_point_3d<double> (0.0, 0.0, 0.0));
  TEST("Volume value test", cbs.vol_val(0,0,0), 0);
  TEST("Volume value test", cbs.vol_val(5,10,15), 1);
  TEST("Volume dimensions test", cbs.vol_dim(), vnl_int_3(10, 20, 30));
  TEST("Volume center test", cbs.vol_center(), vnl_int_3(4, 9, 14));

  vgl_point_3d<double> world1(cbs.grid_to_world(vnl_double_3(4.0, 9.0, 14.0)));
  vgl_point_3d<double> world2(cbs.grid_to_world(vnl_double_3(0.0, 0.0, 0.0)));
  vgl_point_3d<double> world3(cbs.grid_to_world(vnl_double_3(9.0, 19.0, 29.0)));

  TEST_NEAR("Grid to world conversion test", world1.x(), 0.0, 1e-10);
  TEST_NEAR("Grid to world conversion test", world1.y(), 0.0, 1e-10);
  TEST_NEAR("Grid to world conversion test", world1.z(), 0.0, 1e-10);

  TEST_NEAR("Grid to world conversion test", world2.x(), -0.1416, 1e-10);
  TEST_NEAR("Grid to world conversion test", world2.y(), -0.3186, 1e-10);
  TEST_NEAR("Grid to world conversion test", world2.z(), -0.4956, 1e-10);

  TEST_NEAR("Grid to world conversion test", world3.x(), 0.177, 1e-10);
  TEST_NEAR("Grid to world conversion test", world3.y(), 0.354, 1e-10);
  TEST_NEAR("Grid to world conversion test", world3.z(), 0.531, 1e-10);

  vnl_double_3 grid1(cbs.world_to_grid(vgl_point_3d<double>(0.0, 0.0, 0.0)));
  vnl_double_3 grid2(cbs.world_to_grid(vgl_point_3d<double>(-0.1416, -0.3186, -0.4956)));
  vnl_double_3 grid3(cbs.world_to_grid(vgl_point_3d<double>(0.177, 0.354, 0.531)));

  TEST_NEAR("World to grid conversion test", grid1.get(0), 4.0, 1e-10);
  TEST_NEAR("World to grid conversion test", grid1.get(1), 9.0, 1e-10);
  TEST_NEAR("World to grid conversion test", grid1.get(2), 14.0, 1e-10);

  TEST_NEAR("World to grid conversion test", grid2.get(0), 0.0, 1e-10);
  TEST_NEAR("World to grid conversion test", grid2.get(1), 0.0, 1e-10);
  TEST_NEAR("World to grid conversion test", grid2.get(2), 0.0, 1e-10);

  TEST_NEAR("World to grid conversion test", grid3.get(0), 9.0, 1e-10);
  TEST_NEAR("World to grid conversion test", grid3.get(1), 19.0, 1e-10);
  TEST_NEAR("World to grid conversion test", grid3.get(2), 29.0, 1e-10);

  xmvg_perspective_camera<double> cam(cbs.camera(0));
  xmvg_perspective_camera<double> cam90(cbs.camera(100));
  xmvg_perspective_camera<double> cam180(cbs.camera(200));

  TEST_NEAR("First camera center test x", cam.get_camera_center().x(), 0.0, 1e-10);
  TEST_NEAR("First camera center test y", cam.get_camera_center().y(), -261.5, 1e-10);
  TEST_NEAR("First camera center test z", cam.get_camera_center().z(), 0.0, 1e-10);

  TEST_NEAR("First camera principal axis test x", cam.principal_axis().x(), 0.0, 1e-10);
  TEST_NEAR("First camera principal axis test y", cam.principal_axis().y(), 1.0, 1e-10);
  TEST_NEAR("First camera principal axis test z", cam.principal_axis().z(), 0.0, 1e-10);

  TEST_NEAR("90 degrees camera center test x", cam90.get_camera_center().x(), 261.5, 1e-10);
  TEST_NEAR("90 degrees camera center test y", cam90.get_camera_center().y(), 0.0, 1e-10);
  TEST_NEAR("90 degrees camera center test z", cam90.get_camera_center().z(), 0.0, 1e-10);

  TEST_NEAR("180 degrees camera center test x", cam180.get_camera_center().x(), 0.0, 1e-10);
  TEST_NEAR("180 degrees camera center test y", cam180.get_camera_center().y(), 261.5, 1e-10);
  TEST_NEAR("180 degrees camera center test z", cam180.get_camera_center().z(), 0.0, 1e-10);

  TEST_NEAR("box minimum point test", cbs.box().min_x(), -0.1416, 1e-10);
  TEST_NEAR("box minimum point test", cbs.box().min_y(), -0.3186, 1e-10);
  TEST_NEAR("box minimum point test", cbs.box().min_z(), -0.4956, 1e-10);

  TEST_NEAR("box maximum point test", cbs.box().max_x(), 0.177, 1e-10);
  TEST_NEAR("box maximum point test", cbs.box().max_y(), 0.354, 1e-10);
  TEST_NEAR("box maximum point test", cbs.box().max_z(), 0.531, 1e-10);

  TEST_NEAR("box centroid test", cbs.box().centroid().x(), 0.0177, 1e-10);
  TEST_NEAR("box centroid test", cbs.box().centroid().y(), 0.0177, 1e-10);
  TEST_NEAR("box centroid test", cbs.box().centroid().z(), 0.0177, 1e-10);

  vgl_homg_line_3d_2_points<double> line;
  vgl_point_3d<double> ray_start;
  vgl_vector_3d<double> ray_direction;
  // from ray direction to principal axis angle
  double rd_to_pa_angle;

  line = cam.xmvg_backproject(vgl_homg_point_2d<double> (100.0, 100.0, 1.0));
  ray_start = (line.point_finite());
  ray_direction.set( line.point_infinite().x(), 
                     line.point_infinite().y(), 
                     line.point_infinite().z() );
  // if the ray direction is given in the wrong direction, 
  // i.e. not towards the image plane, but away from it
  // then it should be corrected
  rd_to_pa_angle = vcl_acos(dot_product(ray_direction, cam.principal_axis()) 
    / (ray_direction.length() * cam.principal_axis().length()));
  if(rd_to_pa_angle > vnl_math::pi_over_2)
    ray_direction = -ray_direction;

  TEST_NEAR("Back-projection ray start test x", ray_start.x(), 0.0, 1e-10);
  TEST_NEAR("Back-projection ray start test y", ray_start.y(), -261.5, 1e-10);
  TEST_NEAR("Back-projection ray start test z", ray_start.z(), 0.0, 1e-10);

  TEST_NEAR("Back-projection ray direction test x", ray_direction.x(), 0.0, 1e-10);
  TEST_NEAR("Back-projection ray direction test y", ray_direction.y(), 1.0, 1e-10);
  TEST_NEAR("Back-projection ray direction test z", ray_direction.z(), 0.0, 1e-10);

  SUMMARY();
}
