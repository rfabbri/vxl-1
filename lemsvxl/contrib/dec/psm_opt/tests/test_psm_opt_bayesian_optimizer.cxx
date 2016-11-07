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
#include <psm_opt/psm_opt_bayesian_optimizer.h>
#include <psm_opt/psm_opt_sample.h>

#include "gen_synthetic_scene.h"


static void test_psm_opt_bayesian_optimizer()
{
  START("bayesian_optimizer test");

  vcl_vector<vil_image_view<float> > images;
  vcl_vector<vpgl_perspective_camera<double> > cameras;
  psm_scene_base_sptr scene_base_true;

  gen_synthetic_scene(scene_base_true,cameras,images);

  psm_scene<PSM_APM_SIMPLE_GREY> *scene_true = dynamic_cast<psm_scene<PSM_APM_SIMPLE_GREY>*>(scene_base_true.ptr());


  // initialize scene with true values, make sure it does not diverge.
  {
    vcl_string storage_dir = "./bayesian_opt_test1";
    vul_file::make_directory(storage_dir);
    psm_scene<PSM_APM_SIMPLE_GREY> scene(scene_true->origin(), scene_true->block_len(), storage_dir, bgeo_lvcs_sptr());
    scene.init_block(vgl_point_3d<int>(0,0,0),2);
    hsds_fd_tree<psm_sample<PSM_APM_SIMPLE_GREY>,3> &block = scene.get_block(vgl_point_3d<int>(0,0,0));
    hsds_fd_tree<psm_sample<PSM_APM_SIMPLE_GREY>,3>::iterator bit = block.begin();
    hsds_fd_tree<psm_sample<PSM_APM_SIMPLE_GREY>,3> const& block_true = scene_true->get_block(vgl_point_3d<int>(0,0,0));
    hsds_fd_tree<psm_sample<PSM_APM_SIMPLE_GREY>,3>::const_iterator bit0 = block_true.begin();
    hsds_fd_tree<psm_sample<PSM_APM_SIMPLE_GREY>,3>::iterator bit1 = block.begin();

    for (; bit0 != block_true.end(); ++bit0, ++bit1) {
      bit1->second = bit0->second;
    }


    for (unsigned int i=0; i<10; ++i) {
      // generate aux_opt samples from images
      vcl_vector<vcl_string> image_ids;
      for (unsigned int c=0; c<cameras.size(); ++c) {
        vcl_stringstream image_id;
        image_id << "image_" << c;
        psm_opt_generate_opt_samples<PSM_APM_SIMPLE_GREY,PSM_AUX_OPT_GREY>(scene, &cameras[c], images[c], image_id.str(), false);
        image_ids.push_back(image_id.str());
      }
      psm_opt_bayesian_optimizer<PSM_APM_SIMPLE_GREY, PSM_AUX_OPT_GREY> opt(scene, image_ids);
      opt.optimize_cells(0.5);
    }

    bit0 = block_true.begin();
    bit1 = block.begin();

    for (; bit0 != block_true.end(); ++bit0, ++bit1) {
      // only examine cells on bottom "z" layer.
      if (block.cell_bounding_box(bit0->first).zmax() < 1.1) {
        vcl_stringstream test_name;
        test_name << "alpha does not diverge from truth (cell " << bit0->first << ")";
        if (bit0->second.alpha > 0.1) {
          TEST(test_name.str().c_str(), bit1->second.alpha >= bit0->second.alpha, true);
        } else {
          TEST_NEAR(test_name.str().c_str(), bit1->second.alpha ,bit0->second.alpha, 0.01);
        }
        vcl_cout << "true alpha = " << bit0->second.alpha << " opt alpha = " << bit1->second.alpha << vcl_endl;
        vcl_cout << "true color = " << bit0->second.appearance.color() << " opt color = " << bit1->second.appearance.color() << vcl_endl;
      }
    }
  }

  // make sure scene converges to true values
  {
    vcl_string storage_dir = "./bayesian_opt_test2";
    vul_file::make_directory(storage_dir);
    psm_scene<PSM_APM_SIMPLE_GREY> scene(scene_true->origin(), scene_true->block_len(), storage_dir, bgeo_lvcs_sptr());

    scene.init_block(vgl_point_3d<int>(0,0,0),2);

    for (unsigned int i=0; i<1; ++i) {
      // generate aux_opt samples from images
      vcl_vector<vcl_string> image_ids;
      for (unsigned int c=0; c<cameras.size(); ++c) {
        vcl_stringstream image_id;
        image_id << "image_" << c;
        psm_opt_generate_opt_samples<PSM_APM_SIMPLE_GREY,PSM_AUX_OPT_GREY>(scene, &cameras[c], images[c], image_id.str(), false);
        image_ids.push_back(image_id.str());
      }
      psm_opt_bayesian_optimizer<PSM_APM_SIMPLE_GREY, PSM_AUX_OPT_GREY> opt(scene, image_ids);
      opt.optimize_cells(0.5);
    }

    // make sure scene does not move from true values
    hsds_fd_tree<psm_sample<PSM_APM_SIMPLE_GREY>,3> const& block_true = scene_true->get_block(vgl_point_3d<int>(0,0,0));
    hsds_fd_tree<psm_sample<PSM_APM_SIMPLE_GREY>,3> const& block = scene.get_block(vgl_point_3d<int>(0,0,0));

    hsds_fd_tree<psm_sample<PSM_APM_SIMPLE_GREY>,3>::const_iterator bit0 = block_true.begin();
    hsds_fd_tree<psm_sample<PSM_APM_SIMPLE_GREY>,3>::const_iterator bit1 = block.begin();

    for (; bit0 != block_true.end(); ++bit0, ++bit1) {
      // only examine cells on bottom "z" layer.
      if (block.cell_bounding_box(bit0->first).zmax() < 1.1) {
        vcl_stringstream test_name;
        test_name << "alpha does not diverge from truth (cell " << bit0->first << ")";
        if (bit0->second.alpha > 0.1) {
          TEST(test_name.str().c_str(), bit1->second.alpha >= bit0->second.alpha, true);
        } else {
          TEST_NEAR(test_name.str().c_str(), bit1->second.alpha ,bit0->second.alpha, 0.01);
        }
        vcl_cout << "true alpha = " << bit0->second.alpha << " opt alpha = " << bit1->second.alpha << vcl_endl;
        vcl_cout << "true color = " << bit0->second.appearance.color() << " opt color = " << bit1->second.appearance.color() << vcl_endl;
      }
    }


  }

}



TESTMAIN( test_psm_opt_bayesian_optimizer );
