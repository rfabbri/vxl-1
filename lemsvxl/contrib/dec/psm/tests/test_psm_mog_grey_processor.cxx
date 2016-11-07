#include <testlib/testlib_test.h>

#include <vcl_vector.h>
#include <vnl/vnl_random.h>

#include "../psm_apm_traits.h"
#include "../psm_mog_grey_processor.h"



static void test_psm_mog_grey_processor()
{
  START("psm_mog_grey_processor test");

  vcl_vector<psm_apm_traits<PSM_APM_MOG_GREY>::obs_datatype> obs_vec;
  vcl_vector<float> vis_vec;
  vcl_vector<float> pre_vec;

  // generate a bunch of random samples with visibility probability 1
  unsigned int nsamples = 5000;
  unsigned int nmodes = 3;
  vnl_vector_fixed<float,3> true_means(0.85f, 0.5f, 0.16f);
  vnl_vector_fixed<float,3> true_sigmas(0.05f, 0.03f, 0.03f);
  vnl_vector_fixed<float,3> true_mode_weights(0.5f, 0.3f, 0.2f);

  vnl_random rand_gen;
  for (unsigned int i=0; i<nsamples; ++i) {
    // decide on mode to pick from
   double r = rand_gen.drand32(0.0, 1.0);
   unsigned int mode = 0;
   double cdf = 0.0;
   for (unsigned int m=0; m<nmodes; ++m) { 
     cdf += true_mode_weights[m];
     if (r <= cdf) {
       mode = m;
       break;
     }
   }
   float sample = true_means[mode] + (true_sigmas[mode]*(float)rand_gen.normal());
   float vis = 1.0f;
   float pre = 0.0f;
   obs_vec.push_back(sample);
   vis_vec.push_back(vis);
   pre_vec.push_back(pre);
  }

  psm_apm_traits<PSM_APM_MOG_GREY>::apm_datatype model;
  psm_mog_grey_processor::compute_appearance(obs_vec, pre_vec, vis_vec, model, 0.001f);

  for (unsigned int m = 0; m < nmodes; ++m) {
    vcl_cout << "-- Mode " << m << " --" << vcl_endl;
    TEST_NEAR("Mode has correct mean", model.distribution(m).mean(), true_means[m], 0.01);

    TEST_NEAR("Mode has correct sigma", vcl_sqrt(model.distribution(m).var()), true_sigmas[m], 0.02);

    TEST_NEAR("Mode has correct weight", model.weight(m), true_mode_weights[m], 0.10);
  }

  
  // test using small number of samples
  obs_vec.clear();
  vis_vec.clear();
  pre_vec.clear();
  obs_vec.push_back(0.2f); obs_vec.push_back(0.21f); obs_vec.push_back(0.19); obs_vec.push_back(0.81); obs_vec.push_back(0.8f); obs_vec.push_back(0.79f);
  vis_vec.push_back(1.0f); vis_vec.push_back(1.0f); vis_vec.push_back(1.0f); vis_vec.push_back(1.0f); vis_vec.push_back(1.0f); vis_vec.push_back(1.0f);
  pre_vec.push_back(0.0f); pre_vec.push_back(0.0f); pre_vec.push_back(0.0f); pre_vec.push_back(0.0f); pre_vec.push_back(0.0f); pre_vec.push_back(0.0f);
  
  psm_mog_grey_processor::compute_appearance(obs_vec, pre_vec, vis_vec, model, 0.05f);

  for (unsigned int m=0; m<3; ++m) {
    vcl_cout << "-- Mode " << m << " --" << vcl_endl;
    vcl_cout << "weight = " << model.weight(m) << vcl_endl;
    vcl_cout << "mean = " << model.distribution(m).mean() << vcl_endl;
    vcl_cout << "std_dev = " << vcl_sqrt(model.distribution(m).var()) << vcl_endl;
  }


  return;
}



TESTMAIN( test_psm_mog_grey_processor );
