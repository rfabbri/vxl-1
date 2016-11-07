#include <testlib/testlib_test.h>

#include <vcl_vector.h>
#include <vnl/vnl_random.h>

#include "../psm_apm_traits.h"
#include "../psm_simple_grey_processor.h"



static void test_psm_simple_grey_processor()
{
  START("psm_simple_grey_processor test");

  vcl_vector<psm_apm_traits<PSM_APM_SIMPLE_GREY>::obs_datatype> obs_vec;
  vcl_vector<float> vis_vec;
  vcl_vector<float> pre_vec;

  // generate a bunch of random samples with visibility probability 1
  unsigned int nsamples = 10000;
  float true_mean = 0.3f;
  float true_sigma = 0.08f;
  float true_gauss_weight = 1.0f;

  vnl_random rand_gen;
  for (unsigned int i=0; i<nsamples; ++i) {
   float sample = true_mean + (true_sigma*(float)rand_gen.normal());
   float vis = 1.0;
   float pre = 0.0f;
   obs_vec.push_back(sample);
   vis_vec.push_back(vis);
   pre_vec.push_back(pre);
  }

  psm_apm_traits<PSM_APM_SIMPLE_GREY>::apm_datatype model;
  psm_simple_grey_processor::compute_appearance(obs_vec, pre_vec, vis_vec, model, 0.001f);
  //for (unsigned int i=0; i<15; ++i) {
  //  psm_simple_grey_processor::update_appearance(obs_vec, vis_vec, model, 0.001f);
  //}

  float model_mean = model.color();
  float model_sigma = model.sigma();
  float model_gauss_weight = model.gauss_weight();

  TEST_NEAR("Model has correct mean (unity weights)", model_mean, true_mean, 0.01);

  TEST_NEAR("Model has correct sigma (unity weights)", model_sigma, true_sigma, 0.005);

  TEST_NEAR("Model has correct gauss_weight (unity weights)", model_gauss_weight, true_gauss_weight, 0.01);



  // test for graceful degradation with poor input data
  obs_vec.clear();
  vis_vec.clear();
  pre_vec.clear();
  vis_vec.push_back(1.0f); vis_vec.push_back(1.0f); vis_vec.push_back(1.0f); vis_vec.push_back(1.0f);
  pre_vec.push_back(0.0f); pre_vec.push_back(0.0f); pre_vec.push_back(0.0f); pre_vec.push_back(0.7f);
  obs_vec.push_back(0.75f); obs_vec.push_back(0.76f); obs_vec.push_back(0.74f); obs_vec.push_back(0.2f);

  psm_apm_traits<PSM_APM_SIMPLE_GREY>::apm_datatype model3;
  psm_simple_grey_processor::compute_appearance(obs_vec, pre_vec, vis_vec, model3, 0.02f);

  float model3_mean = model3.color();
  float model3_sigma = model3.sigma();
  float model3_gauss_weight = model3.gauss_weight();

  TEST_NEAR("Model has reasonable mean (limited input data)", model3_mean, 0.75, 0.1);
  TEST("Model has reasonable sigma (limited input data)", model3_sigma > 0.005, true);

  return;
}



TESTMAIN( test_psm_simple_grey_processor );
