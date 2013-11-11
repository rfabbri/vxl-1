#include <testlib/testlib_test.h>

#include <vcl_vector.h>
#include <vcl_string.h>
#include <vcl_sstream.h>

#include <psm/psm_scene.h>
#include <psm/psm_apm_traits.h>
#include <psm/psm_aux_traits.h>


#include <vil/vil_image_view.h>
#include <vpgl/vpgl_perspective_camera.h>

#include <vul/vul_file.h>

#include <psm/psm_scene.h>
#include <psm/psm_sample.h>
#include <psm/psm_apm_traits.h>

#include <psm_opt/psm_opt_generate_opt_samples.h>
#include <psm_opt/psm_opt_sample.h>

#include "gen_synthetic_scene.h"


static void test_psm_opt_generate_opt_samples()
{
  START("generate_opt_samples test");

  vcl_vector<vil_image_view<float> > images;
  vcl_vector<vpgl_perspective_camera<double> > cameras;
  vcl_vector<vpgl_camera<double>*> camera_ptrs;
  psm_scene_base_sptr scene_base;

  gen_synthetic_scene(scene_base,cameras,images);
  for (unsigned int i=0; i<cameras.size(); ++i) {
    camera_ptrs.push_back(&cameras[i]);
  }

  psm_scene<PSM_APM_SIMPLE_GREY> *scene = dynamic_cast<psm_scene<PSM_APM_SIMPLE_GREY>*>(scene_base.ptr());

  // generate aux_opt samples from images
  vcl_vector<vcl_string> image_ids;
  for (unsigned int c=0; c<cameras.size(); ++c) {
    vcl_stringstream image_id;
    image_id << "image_" << c;
    psm_opt_generate_opt_samples<PSM_APM_SIMPLE_GREY,PSM_AUX_OPT_GREY>(*scene, &cameras[c], images[c], image_id.str(), false);
    image_ids.push_back(image_id.str());
  }

  // examine aux scenes
  for (unsigned int c=0; c<cameras.size(); ++c) {
    psm_aux_scene_base_sptr aux_scene_base = scene->get_aux_scene<PSM_AUX_OPT_GREY>(image_ids[c]);
    psm_aux_scene<PSM_AUX_OPT_GREY> *aux_scene = dynamic_cast<psm_aux_scene<PSM_AUX_OPT_GREY>*>(aux_scene_base.ptr());
    hsds_fd_tree<psm_opt_sample<float>,3> &opt_block = aux_scene->get_block(vgl_point_3d<int>(0,0,0));
    for (float x=0.5f; x<3.0f; x+=1.0f) {
      for (float y=0.5f; y<3.0f; y+=1.0f) {
        float z = 0.5;
        psm_opt_sample<float> cell = opt_block.data_at(x,y,z);
        vcl_cout << vcl_endl;
        vcl_cout << "cell at " << x << ", " << y << ", " << z << ": " << vcl_endl;
        vcl_cout << "vis = " << cell.vis_ << vcl_endl;
        vcl_cout << "PI = " << cell.PI_ << vcl_endl;
        vcl_cout << "pre = " << cell.pre_ << vcl_endl;
        vcl_cout << "post = " << cell.post_ << vcl_endl;
        vcl_cout << "obs = " << cell.obs_ << vcl_endl;
        vcl_cout << "seg_len = " << cell.seg_len_ << vcl_endl;
        vcl_cout << vcl_endl;
      }
    }
  }

 
  return;
}



TESTMAIN( test_psm_opt_generate_opt_samples );
