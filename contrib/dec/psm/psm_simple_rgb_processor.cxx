#include <vnl/vnl_erf.h>
#include <vil/algo/vil_colour_space.h>

#include "psm_simple_rgb_processor.h"
//:
// \file

//const float psm_simple_rgb_processor::one_over_sigma_ = 1.0f/0.04f;
const static bool USE_UNIFORM_COMPONENT = false;

//: Return probability density of observing pixel values
float psm_simple_rgb_processor::prob_density(apm_datatype const& appear, obs_datatype const& obs)
{
  vnl_vector_fixed<float,3> obs_vec;
  obs_to_vector(obs,obs_vec);

  const vnl_vector_fixed<float,3> norm =  appear.one_over_sigma() * float(vnl_math::sqrt1_2 * vnl_math::two_over_sqrtpi * 0.5);

  const vnl_vector_fixed<float,3> diff = obs_vec - appear.color();

  const vnl_vector_fixed<float,3> one_over_sigma = appear.one_over_sigma();

  float exponent = 0.0;
  for (unsigned int i=0; i<3; ++i) {
    const float exp_term = diff[i]*one_over_sigma[i];
    exponent -= exp_term*exp_term*0.5f;
  }
  float p = norm[0]*norm[1]*norm[2]*vcl_exp(exponent);
  // normalize by area of distribution between 0 and 1
  //p /= total_prob(appear);
  return p;
}

//: Return probabilities that pixels are in range [min,max]
float psm_simple_rgb_processor::prob_range(apm_datatype const& appear, obs_datatype const& obs_min, obs_datatype const& obs_max)
{
  // make sure obs_min and obs_max are within bounds
  vnl_vector_fixed<float,3> obs_min_v;
  obs_to_vector(obs_min, obs_min_v);
 
  vnl_vector_fixed<float,3> obs_max_v;
  obs_to_vector(obs_max, obs_max_v);

  for (unsigned int i=0; i<3; ++i) {
    if (obs_min_v[i] < 0.0f)
      obs_min_v[i] = 0.0f;
    if (obs_max_v[i] > 1.0f)
      obs_max_v[i] = 1.0f;
  }

  const vnl_vector_fixed<float,3> diff_low = obs_min_v - appear.color();
  const vnl_vector_fixed<float,3> diff_high = obs_max_v - appear.color();

  const vnl_vector_fixed<float,3> norm = appear.one_over_sigma()*(float)vnl_math::sqrt1_2;

  float P = 1.0f;
  for (unsigned int i=0; i<3; ++i) {
    const float double_cdf_low_minus_1 = (float)vnl_erf(diff_low[i]*norm[i]);
    const float double_cdf_high_minus_1 = (float)vnl_erf(diff_high[i]*norm[i]);
    P *= (0.5f * (double_cdf_high_minus_1 - double_cdf_low_minus_1));
  }
  //P / total_prob(appear);
  return P;
}

//: Return probabilities that pixels are in range [0,1] - used for normalizing 
float psm_simple_rgb_processor::total_prob(apm_datatype const& appear)
{
  vcl_cerr << "WARNING:: psm_simple_rgb_processor::total_prob not implemented!" << vcl_endl;
  return 1.0f;
}
#if 0
float psm_simple_rgb_processor::total_prob(apm_datatype const& appear)
{
  const vnl_vector_fixed<float,3> diff_low =  vnl_vector_fixed<float,3>(0.0f) - appear.color_vec();
  const vnl_vector_fixed<float,3> diff_high = vnl_vector_fixed<float,3>(1.0f) - appear.color_vec();

  const vnl_vector_fixed<float,3> norm = appear.one_over_sigma()*(float)vnl_math::sqrt1_2;

  float P = 1.0f;
  for (unsigned int i=0; i<3; ++i) {
    const float double_cdf_low_minus_1 = (float)vnl_erf(diff_low[i]*norm[i]);
    const float double_cdf_high_minus_1 = (float)vnl_erf(diff_high[i]*norm[i]);
    P *= (0.5f * (double_cdf_high_minus_1 - double_cdf_low_minus_1));
  }
  return P;
}
#endif

//: Update with a new sample image
bool psm_simple_rgb_processor::update( apm_datatype &appear, obs_datatype const& obs, float const& weight)
{
  vnl_vector_fixed<float,3> obs_v;
  obs_to_vector(obs,obs_v);
  appear = psm_simple_rgb(obs_v,appear.sigma());
  return true;
}


//: Expected value
psm_apm_traits<PSM_APM_SIMPLE_RGB>::obs_datatype psm_simple_rgb_processor::expected_color(psm_apm_traits<PSM_APM_SIMPLE_RGB>::apm_datatype const& appear) 
{
  vil_rgb<float> expected_obs;
  vector_to_obs(appear.color(), expected_obs);
  return expected_obs;
}

//: color of the most probable mode in the mixtures in the slab
psm_simple_rgb_processor::obs_datatype psm_simple_rgb_processor::most_probable_color(apm_datatype const& appear)
{
  vil_rgb<float> most_prob_obs;
  vector_to_obs(appear.color(), most_prob_obs);
  return most_prob_obs;
}

void psm_simple_rgb_processor::init_appearance(psm_apm_traits<PSM_APM_SIMPLE_RGB>::obs_datatype const& mean, float variance, psm_apm_traits<PSM_APM_SIMPLE_RGB>::apm_datatype &model)
{
  vnl_vector_fixed<float,3> mean_v;
  obs_to_vector(mean,mean_v);
  model = psm_apm_traits<PSM_APM_SIMPLE_RGB>::apm_datatype(mean_v, vcl_sqrt(variance));
  return;
}


void psm_simple_rgb_processor::compute_appearance(vcl_vector<psm_apm_traits<PSM_APM_SIMPLE_RGB>::obs_datatype> const& obs, vcl_vector<float> const& pre, vcl_vector<float> const& vis, psm_apm_traits<PSM_APM_SIMPLE_RGB>::apm_datatype &model, float min_sigma)
{
  vcl_cerr << "ERROR: psm_simple_rgb_processor:::compute_appearance NOT IMPLEMENTED YET" << vcl_endl;
  //update_appearance(obs,weights,model,min_sigma);
  //finalize_appearance(obs,weights,model);
  return;
}


void psm_simple_rgb_processor::update_appearance(vcl_vector<psm_apm_traits<PSM_APM_SIMPLE_RGB>::obs_datatype> const& obs, vcl_vector<float> const& weights, psm_apm_traits<PSM_APM_SIMPLE_RGB>::apm_datatype &model, float min_sigma)
{
  const float big_sigma = (float)vnl_math::sqrt1_2; // maximum possible std. dev for set of samples drawn from [0 1]

  const unsigned int nobs = obs.size();
  vcl_vector<vnl_vector_fixed<float,3> > obs_v(nobs);
  for (unsigned int i=0; i<nobs; ++i) {
    obs_to_vector(obs[i], obs_v[i]);
  }

  if (nobs == 0) {
    // zero observations. nothing to do here.
    return;
  }
  if (nobs == 1) {
    // one observation: Just return the value as the mean, and a big sigma.
    model = psm_apm_traits<PSM_APM_SIMPLE_RGB>::apm_datatype(obs_v[0], vnl_vector_fixed<float,3>(big_sigma));
    return;
  } 
  else {
    // compute estimate of gaussian weight by summing probabilities
    vcl_vector<float> obs_gauss_weights = weights;
    float gauss_weight = 0.0f;
    if (USE_UNIFORM_COMPONENT) {
      float weight_sum = 0.0f;

      for (unsigned int n=0; n<nobs; ++n) {
        const float p_total = psm_apm_traits<PSM_APM_SIMPLE_RGB>::apm_processor::prob_density(model,obs[n]);
        const float p_uniform = 1.0f - model.gauss_weight();
        const float p_gauss = p_total - p_uniform;
        if (p_total > 0.0f) {
          obs_gauss_weights[n] = weights[n] * p_gauss / p_total;
        } else {
          obs_gauss_weights[n] = 0.0f;
        }
        gauss_weight += obs_gauss_weights[n];
        weight_sum += weights[n];
      }
      if (weight_sum > 0.0f) {
        gauss_weight /= weight_sum;
      }
    } 
    else {
      gauss_weight = 1.0f;
    }
    // Initialize the estimates
    vnl_vector_fixed<float,3> mean_est(0.0f);
    vnl_vector_fixed<float,3> sigma_est(0.0f);

    compute_gaussian_params(obs_v, obs_gauss_weights, mean_est, sigma_est);

    //sigma_est *= sigma_norm_factor(nobs);

    for (unsigned int i=0; i<3; ++i) {
      // make sure standard deviation is not too small
      if (sigma_est[i] < min_sigma) {
        sigma_est[i] = min_sigma;
      }
      // or too big
      if (sigma_est[i] > big_sigma) {
        sigma_est[i] = big_sigma; 
      }
    }

    model = psm_simple_rgb(mean_est, sigma_est);
  }
  return;
}

void psm_simple_rgb_processor::finalize_appearance(vcl_vector<psm_apm_traits<PSM_APM_SIMPLE_RGB>::obs_datatype> const& obs, vcl_vector<float> const& weights, psm_apm_traits<PSM_APM_SIMPLE_RGB>::apm_datatype &model)
{
  const float big_sigma = (float)vnl_math::sqrt1_2; // maximum possible std. dev for set of samples drawn from [0 1]

  unsigned int nobs = obs.size();
  vnl_vector_fixed<float,3> unbiased_sigma(big_sigma);


  if (nobs > 1) {
    unbiased_sigma = model.sigma() * sigma_norm_factor(nobs);
  }
  for (unsigned int i=0; i<3; ++i) {
    if (unbiased_sigma[i] > big_sigma) {
      unbiased_sigma[i] = big_sigma;
    }
  }

  model = psm_apm_traits<PSM_APM_SIMPLE_RGB>::apm_datatype(model.color(), unbiased_sigma);
  return;
}


void psm_simple_rgb_processor::compute_gaussian_params(vcl_vector<vnl_vector_fixed<float,3> > const& obs, vcl_vector<float> const& weights, vnl_vector_fixed<float,3> &mean, vnl_vector_fixed<float,3> &sigma)
{
  const unsigned int nobs = obs.size();
  double w_sum = 0.0;
  double w2_sum = 0.0;
  vnl_vector_fixed<float,3> obs_sum(0.0);

  for (unsigned int i=0; i<nobs; ++i) {
    w_sum += weights[i];
    w2_sum += weights[i]*weights[i];
    obs_sum += obs[i] * weights[i];
  }
  mean = obs_sum / (float)w_sum;

  vnl_vector_fixed<float,3> var_sum(0.0);
  for (unsigned int i=0; i<nobs; ++i) {
    const vnl_vector_fixed<float,3> diff = obs[i] - mean;
    var_sum += element_product(diff,diff)*weights[i];
  }
  const vnl_vector_fixed<float,3> var = var_sum / (float)(w_sum - (w2_sum/w_sum));

  sigma = vnl_vector_fixed<float,3>((float)vcl_sqrt(var[0]), (float)vcl_sqrt(var[1]), (float)vcl_sqrt(var[2]));
}

float psm_simple_rgb_processor::sigma_norm_factor(unsigned int nobs)
{
  // computed by matlab function "calculate_std_scale_factors" with beta=0.1
  static const float unbias_const[] = {0, 1e3f, 7.9057f, 3.0787f, 2.2646f, 1.9389f, 1.7617f, 1.6496f, 1.5716f, 1.5140f, 1.4693f, 1.4336f, 1.4043f, 1.3797f, 1.3587f, 1.3406f, 1.3248f, 1.3107f, 1.2983f, 1.2871f, 1.2770f, 1.2678f, 1.2594f, 1.2517f, 1.2446f, 1.2380f, 1.2319f, 1.2262f, 1.2209f, 1.2159f, 1.2112f, 1.2068f, 1.2026f, 1.1987f, 1.1949f, 1.1914f };

  if (nobs < 2) {
    return unbias_const[1];
  }
  if (nobs < 36) {
    return unbias_const[nobs];
  }
  // else nobs >= 36
  // approximate for big n with function a = m /nobs + b
  static const float m = (unbias_const[35] - unbias_const[30])/(1.0f/35.0f - 1.0f/30.0f);
  static const float b = unbias_const[35]  - m*(1.0f/35.0f);
  return m/nobs + b;
}

// store colors as YIQ to seperate luminance
void psm_simple_rgb_processor::obs_to_vector(psm_apm_traits<PSM_APM_SIMPLE_RGB>::obs_datatype const& obs, vnl_vector_fixed<float,3> &v)
{
  const vnl_vector_fixed<float,3> obs_v(obs.r,obs.g,obs.b);
  vil_colour_space_RGB_to_YIQ(obs_v.data_block(), v.data_block());
  v[1] *= 5.0f;
  v[2] *= 5.0f;
}

void psm_simple_rgb_processor::vector_to_obs(vnl_vector_fixed<float,3> const& v, psm_apm_traits<PSM_APM_SIMPLE_RGB>::obs_datatype &obs)
{
  vnl_vector_fixed<float,3> v2(v[0], v[1]/5.0f, v[2]/5.0f);
  vnl_vector_fixed<float,3> obs_v;
  //vil_colour_space_YIQ_to_RGB(v2.data_block(), obs_v.data_block());
  obs = vil_rgb<float>(obs_v[0],obs_v[1],obs_v[2]);
}

    

vcl_ostream& operator<<(vcl_ostream &os, psm_simple_rgb const& apm) 
{ 
  os << "color: " << apm.color() << ", one_over_sigma: " << apm.one_over_sigma() << vcl_endl;
  return os;
}

