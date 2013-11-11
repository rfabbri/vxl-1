#include <testlib/testlib_test.h>

#include <vpgl/vpgl_perspective_camera.h>
#include <vil/vil_image_view.h>
#include <vil/vil_save.h>

#include "../psm_raytrace_operations.h"

static void test_psm_raytrace_operations()
{
  START("psm_raytrace_operations test");

  vil_image_view<float> test_img(8,8,1);
  for (unsigned j=0; j<8; ++j) {
    for (unsigned i=0; i<8; ++i) {
      test_img(i,j) = (float)i;
    }
  }

  double xverts_2d[] = {2.0, 5.0, 2.0, 5.0, 2.0, 5.0, 2.0, 5.0};
  double yverts_2d[] = {5.0, 5.0, 2.0, 2.0, 5.0, 5.0, 2.0, 2.0}; 
  float vert_dists[] = {100.0f, 101.0f, 100.0f, 101.0f, 100.0f, 100.0f, 100.0f, 100.0f};

  float mean_val = -1.0f;
  if (cube_mean_aa(xverts_2d,yverts_2d,vert_dists,psm_cube_face::Z_HIGH, test_img, mean_val)) {
    vcl_cout << "mean_val = " << mean_val << vcl_endl;
  }
  TEST_NEAR("cube_mean_aa x_varying",mean_val,31.0/9,1e-4);

  float wmean_val = -1.0f;
  cube_weighted_mean_aa(xverts_2d, yverts_2d, vert_dists, psm_cube_face::Z_HIGH, test_img, test_img, wmean_val);
  TEST_NEAR("cube_weighted_mean_aa x_varying",wmean_val,111.0/6 * (6.0/31),1e-4);


  for (unsigned j=0; j<8; ++j) {
    for (unsigned i=0; i<8; ++i) {
      test_img(i,j) = (float)j;
    }
  }
  float vert_dists2[] = {101.0f, 101.0f, 100.0f, 100.0f, 100.0f, 100.0f, 100.0f, 100.0f};
  mean_val = -1.0f;
  cube_mean_aa(xverts_2d,yverts_2d,vert_dists2,psm_cube_face::Z_HIGH, test_img, mean_val);

  TEST_NEAR("cube_mean_aa y_varying",mean_val,31.0/9,1e-4);

  wmean_val = 1.0f;
  cube_weighted_mean_aa(xverts_2d, yverts_2d, vert_dists2, psm_cube_face::Z_HIGH, test_img, test_img, wmean_val);
  TEST_NEAR("cube_weighted_mean_aa y_varying",wmean_val,111.0/6 * (6.0/31),1e-4);


  vil_image_view<float> aa_img(8,8,1);
  aa_img.fill(0.0f);
  test_img.fill(0.0f);
  cube_fill_value_aa(xverts_2d,yverts_2d,psm_cube_face::Z_HIGH,test_img,aa_img,3.0f);

  vil_save(test_img,"c:/research/psm/output/test_img.tiff");
  vil_save(aa_img,"c:/research/psm/output/aa_img.tiff");


  vil_image_view<float> test_img_sm(4,4,1);
  for (unsigned j=0; j<4; ++j) {
    for (unsigned i=0; i<4; ++i) {
      test_img_sm(i,j) = (float)i + j*4;
    }
  }

  double xverts_2d_sm[] = {1.5, 2.5, 1.5, 2.5, 1.5, 2.5, 1.5, 2.5};
  double yverts_2d_sm[] = {2.5, 2.5, 1.5, 1.5, 2.5, 2.5, 1.5, 1.5}; 
  float vert_dists_sm[] = {101.0f, 101.0f, 101.0f, 101.0f, 100.0f, 100.0f, 100.0f, 100.0f};

  mean_val = -1.0f;
  cube_mean_aa(xverts_2d_sm,yverts_2d_sm,vert_dists_sm,psm_cube_face::Z_HIGH, test_img_sm, mean_val);

  TEST_NEAR("small cube_mean_aa x_varying",mean_val,30.0f/4,1e-4);

  wmean_val = -1.0f;
  cube_weighted_mean_aa(xverts_2d_sm, yverts_2d_sm, vert_dists_sm, psm_cube_face::Z_HIGH, test_img_sm, test_img_sm, wmean_val);
  TEST_NEAR("small cube_weighted_mean_aa x_varying",wmean_val, 242.0f/30 ,1e-4);

  vil_image_view<float> aa_img_sm(4,4,1);
  aa_img_sm.fill(0.0f);
  test_img_sm.fill(0.0f);
  cube_fill_value_aa(xverts_2d_sm,yverts_2d_sm,psm_cube_face::Z_HIGH,test_img_sm,aa_img_sm,4.0f);

  vil_save(test_img_sm,"c:/research/psm/output/test_img_sm.tiff");
  vil_save(aa_img_sm,"c:/research/psm/output/aa_img_sm.tiff");


   vil_image_view<float> test_img_vsm(4,4,1);
  for (unsigned j=0; j<4; ++j) {
    for (unsigned i=0; i<4; ++i) {
      test_img_vsm(i,j) = (float)i + j*4;
    }
  }

  double xverts_2d_vsm[] = {1.25, 1.75, 1.25, 1.75, 1.25, 1.75, 1.25, 1.75};
  double yverts_2d_vsm[] = {1.75, 1.75, 1.25, 1.25, 1.75, 1.75, 1.25, 1.25}; 
  float vert_dists_vsm[] = {101.0f, 101.0f, 101.0f, 101.0f, 100.0f, 100.0f, 100.0f, 100.0f};

  mean_val = -1.0f;
  cube_mean_aa(xverts_2d_vsm,yverts_2d_vsm,vert_dists_vsm,psm_cube_face::Z_HIGH, test_img_vsm, mean_val);

  TEST_NEAR("very small cube_mean_aa",mean_val, 5.0, 1e-4);

  wmean_val = -1.0f;
  cube_weighted_mean_aa(xverts_2d_vsm, yverts_2d_vsm, vert_dists_vsm, psm_cube_face::Z_HIGH, test_img_vsm, test_img_vsm, wmean_val);
  TEST_NEAR("very small cube_weighted_mean_aa",wmean_val, 5.0 ,1e-4);

  vil_image_view<float> aa_img_vsm(4,4,1);
  aa_img_vsm.fill(0.0f);
  test_img_vsm.fill(0.0f);
  cube_fill_value_aa(xverts_2d_vsm,yverts_2d_vsm,psm_cube_face::Z_HIGH,test_img_vsm,aa_img_vsm,4.0f);

  vil_save(test_img_vsm,"c:/research/psm/output/test_img_vsm.tiff");
  vil_save(aa_img_vsm,"c:/research/psm/output/aa_img_vsm.tiff");

  vgl_point_3d<double> box_low(0.0, 0.0, 0.0);
  vgl_point_3d<double> box_high(100.0, 100.0, 100.0);
  vgl_point_3d<double> cam_center(300.0, 300.0, 300.0);
  vbl_bounding_box<double,3> bbox;
  bbox.update(box_low.x(), box_low.y(), box_low.z());
  bbox.update(box_high.x(), box_high.y(), box_high.z());

  vgl_rotation_3d<double> rot;
  vpgl_calibration_matrix<double> K(1.0, vgl_point_2d<double>(100.0, 100.0));
  vpgl_perspective_camera<double> cam(K, cam_center, rot);
  cam.look_at(vgl_homg_point_3d<double>(box_high));
  psm_cube_face_list visible_faces = psm_cube_face::ALL;
  project_cube_vertices(bbox, cam, xverts_2d, yverts_2d, vert_dists, visible_faces);

  TEST_EQUAL("project_cube_vertices: visible_faces",visible_faces,psm_cube_face::Z_HIGH | psm_cube_face::X_HIGH | psm_cube_face::Y_HIGH);

  TEST_NEAR("project_cube_vertices: vert_dists0",vert_dists[0],vcl_sqrt(double(300*300 + 300*300 + 300*300)),1e-4);
  TEST_NEAR("project_cube_vertices: vert_dists7",vert_dists[7],vcl_sqrt(double(200*200 + 200*200 + 200*200)),1e-4);


  return;
}




TESTMAIN( test_psm_raytrace_operations );

