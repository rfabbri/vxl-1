

































































































































































































































































































































































































































#include <testlib/testlib_test.h>

#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vcl_sstream.h>
#include <vcl_iomanip.h>

#include <vgl/algo/vgl_rotation_3d.h>
#include <vnl/vnl_matrix_fixed.h>

#include <vil/vil_image_view.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_save.h>
#include <vil/vil_load.h>
#include <vil/vil_convert.h>

#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/vpgl_calibration_matrix.h>

#include <psm/psm_scene.h>

#include "../psm_update.h"
#include "../psm_update_parallel.h"

#include "../psm_refine_scene.h"
#include "../psm_render_expected.h"
#include "../psm_render_expected_parallel.h"

static void test_psm_update()
{
  START("update test");

  vgl_point_3d<float> corner(-220.0, -200.0, -35.0);
  vcl_string storage_dir = "c:/research/psm/models/capitol_high";
  vgl_point_3d<double> scene_origin(corner.x(),corner.y(),corner.z());
  double block_length = 512*0.75;

  for (unsigned int it =5; it < 6; ++it) {
    vcl_cout << "Iteration " << it << vcl_endl;

    psm_scene<psm_sample<PSM_APM_MOG_GREY> > scene(scene_origin,block_length,storage_dir);
    if (it == 0) {
      scene.init_block(vgl_point_3d<int>(0,0,0),6); 
      scene.init_block(vgl_point_3d<int>(0,1,0),6);
      scene.init_block(vgl_point_3d<int>(1,0,0),6);
      scene.init_block(vgl_point_3d<int>(1,1,0),6);
      scene.init_block(vgl_point_3d<int>(0,-1,0),6);
      scene.init_block(vgl_point_3d<int>(-1,0,0),6);
      scene.init_block(vgl_point_3d<int>(-1,-1,0),6);
      scene.init_block(vgl_point_3d<int>(1,-1,0),6);
      scene.init_block(vgl_point_3d<int>(-1,1,0),6);
    }
    else {
      scene.set_block_valid(vgl_point_3d<int>(0,0,0),true);
      scene.set_block_valid(vgl_point_3d<int>(0,1,0),true);
      scene.set_block_valid(vgl_point_3d<int>(1,0,0),true);
      scene.set_block_valid(vgl_point_3d<int>(1,1,0),true);
      scene.set_block_valid(vgl_point_3d<int>(0,-1,0),true);
      scene.set_block_valid(vgl_point_3d<int>(-1,0,0),true);
      scene.set_block_valid(vgl_point_3d<int>(-1,-1,0),true);
      scene.set_block_valid(vgl_point_3d<int>(1,-1,0),true);
      scene.set_block_valid(vgl_point_3d<int>(-1,1,0),true);

    }

    // split nodes with high density
    psm_refine_scene(scene);

    vcl_vector<unsigned> frames;
#define DO_ALL
#ifdef DO_ALL
    for (unsigned int i=0; i < 255; i++) {
      unsigned int x = (i*32) % 255;
      //unsigned int x = i;
      frames.push_back(x);
    }
#else
    for (unsigned int i=0; i < 128; ++i) {
      unsigned int x = ((i*17) % 128)*2;
      frames.push_back(x);
    }
#endif
    for (unsigned int i=0; i < frames.size(); ++i) {
      vcl_stringstream camera_filename_ss;
      vcl_string res_string;
      if (it < 1)
        res_string = "_lores";
      else if (it < 3)
        res_string = "_halfres";
      else
        res_string = "";

      camera_filename_ss << "c:/research/data/CapitolSiteHigh/cameras_KRT" << res_string << "/camera_" << vcl_setw(5) << vcl_setfill('0') << frames[i] << ".txt";
      vcl_string camera_filename = camera_filename_ss.str();

      vcl_stringstream image_filename_ss;
      image_filename_ss << "c:/research/data/CapitolSiteHigh/video_grey" << res_string << "/frame_" << vcl_setw(5) << vcl_setfill('0') << frames[i] << ".png";
      vcl_string image_filename = image_filename_ss.str();

      vcl_cout << "updating with image " << image_filename << " and camera " << camera_filename << vcl_endl;

      // load the camera from file
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

      vil_image_resource_sptr img_base = vil_load_image_resource(image_filename.c_str());
      vil_image_view<unsigned char> img_byte = img_base->get_view();
      vil_image_view<float> img_float(img_byte.ni(),img_byte.nj(),img_byte.nplanes());
      vil_convert_stretch_range(img_byte,img_float,0.0f,1.0f);

      //psm_update<psm_sample<PSM_APM_MOG_GREY> >(scene,cam,img_float);
      psm_update_parallel<psm_sample<PSM_APM_MOG_GREY> >(scene,cam,img_float);

    }
    vcl_stringstream  expected_fname;
    expected_fname << "c:/research/psm/output/expected_f" << it << ".tiff";

    vcl_string exp_camera_filename = "c:/research/registration/output/capitol_high_train/flyover/cameras_KRT/camera_00116.txt";
    vcl_ifstream ifs(exp_camera_filename.c_str());
    if (!ifs.is_open()) {
      vcl_cerr << "Failed to open file " << exp_camera_filename << vcl_endl;
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

    vpgl_perspective_camera<double> exp_cam(K,camera_center,rot);
    vil_image_view<float> expected(1280,720,1);
    vil_image_view<float> mask(1280,720,1);

    psm_render_expected_parallel<psm_sample<PSM_APM_MOG_GREY> >(scene,exp_cam,expected,mask);
   

    vil_image_view<unsigned char> expected_byte(expected.ni(),expected.nj(),expected.nplanes());
    vil_convert_stretch_range(expected,expected_byte);
    vil_save(expected_byte,expected_fname.str().c_str());
    vil_save(mask,"c:/research/psm/output/expected_mask.tiff");
  }
  return;
}



TESTMAIN( test_psm_update );
