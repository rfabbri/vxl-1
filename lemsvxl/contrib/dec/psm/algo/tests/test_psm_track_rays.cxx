#include <testlib/testlib_test.h>

#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vcl_iomanip.h>

#include <vgl/algo/vgl_rotation_3d.h>
#include <vnl/vnl_matrix_fixed.h>

#include <vil/vil_image_view.h>
#include <vil/vil_save.h>

#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/vpgl_calibration_matrix.h>

#include <psm/psm_scene.h>

//#include "../psm_track_rays.h"
#include "../psm_track_rays_parallel.h"

static void test_psm_track_rays()
{
  START("track rays test");

  bool do_init = false;

  vgl_point_3d<float> corner(-220.0, -200.0, -35.0);
  vcl_string storage_dir = "c:/research/psm/models/capitol_high";
  vgl_point_3d<double> scene_origin(corner.x(),corner.y(),corner.z());
  double block_length = 512*0.75;

  psm_scene<psm_sample<PSM_APM_MOG_GREY> > scene(scene_origin,block_length,storage_dir);

  if (do_init) {
    scene.init_block(vgl_point_3d<int>(0,0,0),4);
  }
  else {
    scene.set_block_valid(vgl_point_3d<int>(0,0,0),true);
  }
 
  // load the camera from file
  //vcl_string camera_filename = "c:/research/data/CapitolSiteHigh/cameras_KRT/camera_00000.txt";
  //vcl_string camera_filename = "c:/research/registration/output/capitol_high_train/flyover/cameras_KRT/camera_00116.txt";
  vcl_vector<unsigned> frames;
  for (unsigned int i=0; i < 254; i+=50) {
    frames.push_back(i);
  }
  
  for (unsigned int i=0; i < frames.size(); ++i) {
    vcl_stringstream camera_filename_ss;
    camera_filename_ss << "c:/research/data/CapitolSiteHigh/cameras_KRT_lores/camera_" << vcl_setw(5) << vcl_setfill('0') << frames[i] << ".txt";
    vcl_string camera_filename = camera_filename_ss.str();

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
    unsigned int ni = 1280/8, nj = 720/8;

    vcl_stringstream track_filename_ss;
    track_filename_ss << "c:/research/psm/output/ray_tracks_" << vcl_setw(5) << vcl_setfill('0') << frames[i] <<".txt";
    vcl_string track_filename = track_filename_ss.str();
    //psm_track_rays<psm_sample<PSM_APM_MOG_GREY> >(scene,cam,0, ni, 0, nj, track_filename);
    psm_track_rays_parallel<psm_sample<PSM_APM_MOG_GREY> >(scene,cam,ni,nj,track_filename);
  }
  return;
}



TESTMAIN( test_psm_track_rays );
