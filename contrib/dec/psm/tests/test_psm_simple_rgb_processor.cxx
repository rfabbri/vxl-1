#include <testlib/testlib_test.h>

#include <vcl_vector.h>
#include <vnl/vnl_random.h>
#include <vnl/vnl_erf.h>

#include "../psm_apm_traits.h"
#include "../psm_simple_rgb_processor.h"



static void test_psm_simple_rgb_processor()
{
  START("psm_simple_rgb_processor test");

  vcl_cout << "test default model " << vcl_endl;
  psm_apm_traits<PSM_APM_SIMPLE_RGB>::apm_datatype default_model;
  
  vil_rgb<float> default_mean = psm_simple_rgb_processor::expected_color(default_model);
  float default_prob = psm_simple_rgb_processor::prob_density(default_model,default_mean);

  vcl_cout << "default_mean = " << default_mean << vcl_endl;
  vcl_cout << "mean prob = " << default_prob << vcl_endl;


  vcl_vector<psm_apm_traits<PSM_APM_SIMPLE_RGB>::obs_datatype> obs_vec;
  vcl_vector<float> vis_vec;

  // generate a bunch of random samples with visibility probability 1
  unsigned int nsamples = 10000;
  vil_rgb<float> true_mean(0.3f, 0.8f, 0.4f);
  vnl_vector_fixed<float,3> true_sigma(0.08f, 0.06f, 0.1f);

  vnl_random rand_gen;
  for (unsigned int i=0; i<nsamples; ++i) {
   vil_rgb<float> sample;
   sample.r = true_mean.r + (true_sigma[0]*(float)rand_gen.normal());
   sample.g = true_mean.g + (true_sigma[1]*(float)rand_gen.normal());
   sample.b = true_mean.b + (true_sigma[2]*(float)rand_gen.normal());
   float vis = 1.0;
   obs_vec.push_back(sample);
   vis_vec.push_back(vis);
  }

  psm_apm_traits<PSM_APM_SIMPLE_RGB>::apm_datatype model;
  //psm_simple_rgb_processor::compute_appearance(obs_vec, vis_vec, model, 0.001f);


  vil_rgb<float> model_mean = psm_simple_rgb_processor::expected_color(model);
  vnl_vector_fixed<float,3> model_sigma = model.sigma();

  TEST_NEAR("Model has correct mean red   (unity weights)", model_mean.r, true_mean.r, 0.01);
  TEST_NEAR("Model has correct mean green (unity weights)", model_mean.g, true_mean.g, 0.01);
  TEST_NEAR("Model has correct mean blue  (unity weights)", model_mean.b, true_mean.b, 0.01);

  //TEST_NEAR("Model has correct sigma red   (unity weights)", model_sigma[0], true_sigma[0], 0.005);
  //TEST_NEAR("Model has correct sigma green (unity weights)", model_sigma[1], true_sigma[1], 0.005);
  //TEST_NEAR("Model has correct sigma blue  (unity weights)", model_sigma[2], true_sigma[2], 0.005);

   // now test probability and prob_density functions
  // first compute total probability in range [0,1]
  double total_prob = 0.5 * (1.0 + vnl_erf((1.0 - true_mean.r)/(true_sigma[0] * vnl_math::sqrt2))) - 0.5 * (1.0 + vnl_erf((0.0 - true_mean.r)/(true_sigma[0] * vnl_math::sqrt2)));
  total_prob *= 0.5 * (1.0 + vnl_erf((1.0 - true_mean.g)/(true_sigma[1] * vnl_math::sqrt2))) - 0.5 * (1.0 + vnl_erf((0.0 - true_mean.g)/(true_sigma[1] * vnl_math::sqrt2)));
  total_prob *= 0.5 * (1.0 + vnl_erf((1.0 - true_mean.b)/(true_sigma[2] * vnl_math::sqrt2))) - 0.5 * (1.0 + vnl_erf((0.0 - true_mean.b)/(true_sigma[2] * vnl_math::sqrt2)));

  vil_rgb<float> sample_point(0.30f, 0.80f, 0.40f);
  // compute probability density at sample point and compare to rgb_processor's result
  double true_prob_density = vnl_math::one_over_sqrt2pi / true_sigma[0] * vcl_exp(-(sample_point.r - true_mean.r)*(sample_point.r - true_mean.r)/(2*true_sigma[0]*true_sigma[0]));
  true_prob_density *= vnl_math::one_over_sqrt2pi / true_sigma[1] * vcl_exp(-(sample_point.g - true_mean.g)*(sample_point.g - true_mean.g)/(2*true_sigma[1]*true_sigma[1]));
  true_prob_density *= vnl_math::one_over_sqrt2pi / true_sigma[2] * vcl_exp(-(sample_point.b - true_mean.b)*(sample_point.b - true_mean.b)/(2*true_sigma[2]*true_sigma[2]));

  float prob_density = psm_simple_rgb_processor::prob_density(model, sample_point);
  vcl_cout << "true_density = " << true_prob_density << "  total_prob = " << total_prob << "  prob_density = " << prob_density << vcl_endl;
  TEST_NEAR_REL("Correct probability density", prob_density, true_prob_density/total_prob, 0.1);

  // compute probability of a range of values
  float range = 0.03f;
  double true_prob = 0.5 * (1.0 + vnl_erf((sample_point.r + range - true_mean.r)/(true_sigma[0] * vnl_math::sqrt2))) - 0.5 * (1.0 + vnl_erf((sample_point.r - range - true_mean.r)/(true_sigma[0] * vnl_math::sqrt2)));
  true_prob *= 0.5 * (1.0 + vnl_erf((sample_point.g + range - true_mean.g)/(true_sigma[1] * vnl_math::sqrt2))) - 0.5 * (1.0 + vnl_erf((sample_point.g - range - true_mean.g)/(true_sigma[1] * vnl_math::sqrt2)));
  true_prob *= 0.5 * (1.0 + vnl_erf((sample_point.b + range - true_mean.b)/(true_sigma[2] * vnl_math::sqrt2))) - 0.5 * (1.0 + vnl_erf((sample_point.b - range - true_mean.b)/(true_sigma[2] * vnl_math::sqrt2)));

  float prob = psm_simple_rgb_processor::prob_range(model, sample_point - range, sample_point + range);


  TEST_NEAR_REL("Correct probability of range", prob, true_prob/total_prob, 0.1);


  // generate a bunch of random samples, coming from normal distribution with probability vis
  obs_vec.clear();
  vis_vec.clear();

  for (unsigned int i=0; i<nsamples; ++i) {
    float vis_prob = (float)rand_gen.drand32();
    vis_vec.push_back(vis_prob);
    bool from_dist = rand_gen.drand32() < vis_prob;
    if (from_dist) {
      vil_rgb<float> sample;
      sample.r = true_mean.r + (true_sigma[0]*(float)rand_gen.normal());
      sample.g = true_mean.g + (true_sigma[1]*(float)rand_gen.normal());
      sample.b = true_mean.b + (true_sigma[2]*(float)rand_gen.normal());
      obs_vec.push_back(sample);
    }
    else {
      // if sample is not coming from the distribution, generate from uniform dist [0,1]
      vil_rgb<float> sample;
      sample.r = (float)rand_gen.drand32();
      sample.g = (float)rand_gen.drand32();
      sample.b = (float)rand_gen.drand32();
      obs_vec.push_back(sample);
    }
  }

  psm_apm_traits<PSM_APM_SIMPLE_RGB>::apm_datatype model2;
//  psm_simple_rgb_processor::compute_appearance(obs_vec, vis_vec, model2, 0.001f);

  vil_rgb<float> model2_mean = psm_simple_rgb_processor::expected_color(model2);

  TEST_NEAR("Model has correct mean red   (variable visibility)", model2_mean.r, true_mean.r, 0.01);
  TEST_NEAR("Model has correct mean green (variable visibility)", model2_mean.g, true_mean.g, 0.01);
  TEST_NEAR("Model has correct mean blue  (variable visibility)", model2_mean.b, true_mean.b, 0.01);

  //TEST_NEAR("Model has correct sigma red   (variable visibility)", model2_sigma[0], true_sigma[0], 0.005);
  //TEST_NEAR("Model has correct sigma green (variable visibility)", model2_sigma[1], true_sigma[1], 0.005);
  //TEST_NEAR("Model has correct sigma blue  (variable visibility)", model2_sigma[2], true_sigma[2], 0.005);


 

  return;
}




TESTMAIN( test_psm_simple_rgb_processor );
