#include <testlib/testlib_test.h>

#include <vnl/vnl_random.h>

#include "../psm_sigma_normalizer.h"



static void test_psm_sigma_normalizer()
{
  
  START("psm_sigma_normalizer test");

  vnl_random rand_gen;

  // generate random sets of samples from a normal distribution and compare sigma estimate to true value
  unsigned int n_trials = 10000;

  unsigned int n_observations = 6;
  float true_sigma = 0.5f;
  float true_mean = 2.5f;
  float under_estimation_prob = 0.5f;
  unsigned int under_estimates = 0;

  psm_sigma_normalizer normalizer(under_estimation_prob);

  for (unsigned int t=0; t<n_trials; ++t) {
   vcl_vector<float> samples;
   for (unsigned int n=0; n<n_observations; ++n) {
     float sample = true_mean + true_sigma*(float)rand_gen.normal();
     samples.push_back(sample);
   }
   float mean = 0.0f, variance = 0.0f;
   normalizer.compute_normalized_mean_and_variance(samples,mean,variance);
   if (variance < true_sigma*true_sigma) {
     ++under_estimates;
   }
  }
  float under_estimate_ratio = (float)under_estimates / n_trials;

  TEST_NEAR("correct ratio of sigma under-estimates", under_estimate_ratio, under_estimation_prob, 0.05);

  return;
}

 



TESTMAIN( test_psm_sigma_normalizer );

