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
#include <psm_opt/psm_opt_appearance_estimator.h>
#include <psm_opt/psm_opt_sample.h>


static void test_psm_opt_appearance_estimator()
{
  START("appearance estimator test");

  psm_simple_grey_processor apm_proc;

  vcl_vector<float> obs_vec;
  vcl_vector<float> pre_vec;
  vcl_vector<float> vis_vec;
  psm_simple_grey model;

  obs_vec.push_back(0.1f);
  vis_vec.push_back(1.0f);
  pre_vec.push_back(0.3f);

  obs_vec.push_back(0.9f);
  vis_vec.push_back(1.0f);
  pre_vec.push_back(0.5f);


#if 0
  vcl_ofstream ofs("c:/research/psm/output/scales.txt");
  for (unsigned int i=0; i<1000; ++i) {
    psm_simple_grey model(0.5,1e-4);
    weight_vec[0] = ((float)i) / 10.0f;
    apm_proc.finalize_appearance(obs_vec, weight_vec, model);
    ofs << model.sigma()*1e4 << " ";
  }
  ofs << vcl_endl;
#endif

  psm_opt_appearance_estimator<PSM_APM_SIMPLE_GREY> app_est;
  app_est.compute_appearance(obs_vec, pre_vec, vis_vec, model);

  vcl_cout << "mean = " << model.color() << "  sigma = " << model.sigma() << vcl_endl;
  
}



TESTMAIN( test_psm_opt_appearance_estimator );
