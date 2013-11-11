#include <psm/psm_scene.h>
#include <psm/psm_scene_base.h>
#include <psm/psm_apm_traits.h>
#include <psm/psm_sample.h>
#include <psm/algo/psm_render_expected_aa.h>

#include <hsds/hsds_fd_tree.h>
#include <vcl_vector.h>

#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/algo/vgl_rotation_3d.h>

#include <vil/vil_image_view.h>
#include <vul/vul_file.h>

#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/vpgl_calibration_matrix.h>

#include "gen_synthetic_scene.h"


void gen_synthetic_scene(psm_scene_base_sptr &scene, vcl_vector<vpgl_perspective_camera<double> > &cameras, vcl_vector<vil_image_view<float> > &images)
{
  // create scene
  vgl_point_3d<double> origin(0.0, 0.0, 0.0);
  double block_len = 4.0;
  vcl_string storage_dir = "./synthetic_scene_test";
  vul_file::make_directory(storage_dir);

  scene = new psm_scene<PSM_APM_SIMPLE_GREY>(origin, block_len, storage_dir, bgeo_lvcs_sptr(), 9);
  psm_scene<PSM_APM_SIMPLE_GREY> *scene_ptr = static_cast<psm_scene<PSM_APM_SIMPLE_GREY>*>(scene.ptr());
  scene_ptr->init_block(vgl_point_3d<int>(0,0,0),2);


  // fill in scene
  hsds_fd_tree<psm_sample<PSM_APM_SIMPLE_GREY>,3> &block = scene_ptr->get_block(vgl_point_3d<int>(0,0,0));
  psm_sample<PSM_APM_SIMPLE_GREY> sample;
  sample.alpha = 0.0f;

  block.fill(sample);
  sample.alpha = 1000.0f;
  sample.appearance = psm_simple_grey(0.33f, 0.01f, 1.0f);
  block.data_at(vnl_vector_fixed<double,3>(1.5, 0.5, 0.5)) = sample;
  sample.appearance = psm_simple_grey(0.0f, 0.01f, 1.0f);
  block.data_at(vnl_vector_fixed<double,3>(0.5, 1.5, 0.5)) = sample;
  sample.appearance = psm_simple_grey(0.5f, 0.01f, 1.0f);
  block.data_at(vnl_vector_fixed<double,3>(1.5, 1.5, 0.5)) = sample;
  sample.appearance = psm_simple_grey(1.0f, 0.01f, 1.0f);
  block.data_at(vnl_vector_fixed<double,3>(2.5, 1.5, 0.5)) = sample;
  sample.appearance = psm_simple_grey(0.66f, 0.01f, 1.0f);
  block.data_at(vnl_vector_fixed<double,3>(1.5, 2.5, 0.5)) = sample;
  

  // generate cameras and expected images
  cameras.clear();
  images.clear();

  double d = 10000.0; // distance from camera to scene
  double focal_len = d - 20;

  vpgl_calibration_matrix<double> K(focal_len, vgl_point_2d<double>(1.5,0.5));
  double camz = 0.5 + 1e-6;
  vgl_point_3d<double> cam_center_a(1.5, d+4.0, camz);
  vgl_point_3d<double> cam_center_b(d+4.0, 1.5, camz);
  vgl_point_3d<double> cam_center_c(1.5, -d, camz);
  vgl_point_3d<double> cam_center_d(-d, 1.5, camz);

  vnl_matrix_fixed<double,3,3> R_a(0.0); R_a(0,0) = -1.0; R_a(2,1) = -1.0; R_a(1,2) = -1.0;
  vnl_matrix_fixed<double,3,3> R_b(0.0); R_b(2,0) = -1.0; R_b(0,1) = 1.0;  R_b(1,2) = -1.0;
  vnl_matrix_fixed<double,3,3> R_c(0.0); R_c(0,0) = 1.0;  R_c(2,1) = 1.0;  R_c(1,2) = -1.0;
  vnl_matrix_fixed<double,3,3> R_d(0.0); R_d(2,0) = 1.0;  R_d(0,1) = -1.0; R_d(1,2) = -1.0;


  vpgl_perspective_camera<double> camera_a(K, cam_center_a, vgl_rotation_3d<double>(R_a));
  cameras.push_back(camera_a);
  vpgl_perspective_camera<double> camera_b(K, cam_center_b, vgl_rotation_3d<double>(R_b));
  cameras.push_back(camera_b);
  vpgl_perspective_camera<double> camera_c(K, cam_center_c, vgl_rotation_3d<double>(R_c));
  cameras.push_back(camera_c);
  vpgl_perspective_camera<double> camera_d(K, cam_center_d, vgl_rotation_3d<double>(R_d));
  cameras.push_back(camera_d);

  // generate images
  for (unsigned int c=0; c<cameras.size(); ++c) {
    vil_image_view<float> img(3,1);
    vil_image_view<float> mask(3,1);
    psm_render_expected(*scene_ptr,&cameras[c],img,mask);
    vcl_cout << "image " << c << " : ";
    for (unsigned int i=0; i<3; ++i) {
      vcl_cout << img(i,0) << " ";
    }
    vcl_cout << vcl_endl;

    images.push_back(img);
  }

  return;
}


