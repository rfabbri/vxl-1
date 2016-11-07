#include <testlib/testlib_test.h>

#include <vcl_iostream.h>
#include <vcl_string.h>

#include <vgl/algo/vgl_rotation_3d.h>
#include <vnl/vnl_matrix_fixed.h>

#include <vil/vil_image_view.h>
#include <vil/vil_save.h>
#include <vil/vil_convert.h>

#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/vpgl_calibration_matrix.h>

#include <psm/psm_scene.h>

#include "../psm_render_expected.h"
#include "../psm_render_expected_parallel.h"

static void test_psm_render_expected()
{
  START("render_expected test");

  vgl_point_3d<float> corner(-220.0, -200.0, -35.0);
  vcl_string storage_dir = "c:/research/psm/models/capitol_high";
  vgl_point_3d<double> scene_origin(corner.x(),corner.y(),corner.z());
  double block_length = 512*0.75;
  psm_scene<psm_sample<PSM_APM_MOG_GREY> > scene(scene_origin,block_length,storage_dir);
  scene.set_block_valid(vgl_point_3d<int>(0,0,0),true);
  scene.set_block_valid(vgl_point_3d<int>(0,1,0),true);
  scene.set_block_valid(vgl_point_3d<int>(1,0,0),true);
  scene.set_block_valid(vgl_point_3d<int>(1,1,0),true);
  scene.set_block_valid(vgl_point_3d<int>(0,-1,0),true);
  scene.set_block_valid(vgl_point_3d<int>(-1,0,0),true);
  scene.set_block_valid(vgl_point_3d<int>(-1,-1,0),true);
  scene.set_block_valid(vgl_point_3d<int>(1,-1,0),true);
  scene.set_block_valid(vgl_point_3d<int>(-1,1,0),true);

  // load the camera from file
  //vcl_string camera_filename = "c:/research/data/CapitolSiteHigh/cameras_KRT/camera_00000.txt";
  vcl_string camera_filename = "c:/research/registration/output/capitol_high_train/flyover/cameras_KRT/camera_00307.txt";
  //vcl_string camera_filename = "c:/research/psm/output/capitol_flythrough/camera_high.txt";
  vcl_ifstream ifs(camera_filename.c_str());
  if (!ifs.is_open()) {
    vcl_cerr << "Failed to open file " << camera_filename << vcl_endl;
    return;
  }
  vnl_matrix_fixed<double,3,3> K_matrix;
  vnl_matrix_fixed<double,3,3> R_matrix;
  vnl_vector_fixed<double,3> T_vector;
  ifs >> K_matrix;
  ifs >> R_matrix;
  ifs >> T_vector;

  vpgl_calibration_matrix<double> K(K_matrix);
  vgl_rotation_3d<double> rot(R_matrix);
  vnl_vector_fixed<double,3> center_v = -R_matrix.transpose()*T_vector;
  vgl_point_3d<double> camera_center(center_v[0],center_v[1],center_v[2]);

  vpgl_perspective_camera<double> cam(K,camera_center,rot);
  vil_image_view<float> expected(1280,720,1);
  vil_image_view<float> mask(1280,720,1);

  //psm_render_expected<psm_sample<PSM_APM_MOG_GREY> >(scene,cam,expected,mask);
  psm_render_expected_parallel<psm_sample<PSM_APM_MOG_GREY> >(scene,cam,expected,mask);

  vil_image_view<unsigned char> expected_byte(expected.ni(),expected.nj(),expected.nplanes());
  vil_convert_stretch_range(expected,expected_byte);
  vil_save(expected_byte,"c:/research/psm/output/expected.tiff");
  vil_save(mask,"c:/research/psm/output/expected_mask.tiff");

  return;
}



TESTMAIN( test_psm_render_expected );
