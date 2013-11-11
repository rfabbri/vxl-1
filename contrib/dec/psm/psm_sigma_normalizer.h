#ifndef psm_sigma_normalizer_h_
#define psm_sigma_normalizer_h_

#include <vcl_vector.h>

class psm_sigma_normalizer 
{
public:
  psm_sigma_normalizer(float under_estimation_probability);

  float normalization_factor(float number_of_observations);

  float normalization_factor_int(unsigned int number_of_observations);

  void compute_sample_mean_and_variance(vcl_vector<float> samples, float& mean, float& variance);

  void compute_sample_mean_and_variance(vcl_vector<float> samples, vcl_vector<float> weights, float& mean, float& variance);

  void compute_normalized_mean_and_variance(vcl_vector<float> samples, float& mean, float& variance);

  void compute_normalized_mean_and_variance(vcl_vector<float> samples, vcl_vector<float> weights, float& mean, float& variance);

private:
  
  static const unsigned int N_PRECOMPUTED_ = 40;
  vcl_vector<float> unbias_const_;

};

#endif

