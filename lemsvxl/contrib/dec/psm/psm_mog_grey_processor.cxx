#include "psm_mog_grey_processor.h"
//:
// \file
#include <bsta/algo/bsta_adaptive_updater.h>
#include <bsta/bsta_distribution.h>
#include <bsta/bsta_gauss_f1.h>
#include <bsta/bsta_attributes.h>
#include <bsta/bsta_mixture_fixed.h>
#include <bsta/bsta_gaussian_indep.h>


#include <vcl_cassert.h>

//: Return probability density of observing pixel values
float psm_mog_grey_processor::prob_density(apm_datatype const& appear, obs_datatype const& obs)
{
  if (appear.num_components() ==0) {
    return 1.00f;
  }
  else {
    return appear.prob_density(obs);
  }
}

//: Return probabilities that pixels are in range [min,max]
float psm_mog_grey_processor::prob_range(apm_datatype const& appear, obs_datatype const& obs_min, obs_datatype const& obs_max)
{
  if (appear.num_components() ==0) {
    return 1.00f;
  }
  else {
    return appear.probability(obs_min, obs_max);
  }
}


//: Update with a new sample image
bool psm_mog_grey_processor::update( apm_datatype &appear, obs_datatype const& obs, float const& weight)
{
  // the model
  float init_variance = 0.008f;
  float min_stddev = 0.030f; //changed from 0.02 for dinoRing dataset -dec
  float g_thresh = 2.5; // number of std devs from mean sample must be
  bsta_gauss_f1 this_gauss(0.0f, init_variance);

  const unsigned int nmodes = psm_apm_traits<PSM_APM_MOG_GREY>::n_gaussian_modes_;

  typedef bsta_num_obs<bsta_gauss_f1> gauss_type;
  typedef bsta_mixture_fixed<gauss_type, nmodes> mix_gauss;

  // the updater
  bsta_mg_grimson_weighted_updater<mix_gauss> updater(this_gauss, nmodes ,g_thresh,min_stddev);

  if (weight > 0) {
    updater(appear, obs, weight);
  }

  return true;
}


//: Expected value
psm_apm_traits<PSM_APM_MOG_GREY>::obs_datatype psm_mog_grey_processor::expected_color(psm_apm_traits<PSM_APM_MOG_GREY>::apm_datatype const& appear) 
{

  float total_weight= 0;
  obs_datatype c = 0;
  obs_datatype expected = 0;

  //should be components used
  for (unsigned i = 0; i< appear.num_components(); ++i)
  {
    total_weight += appear.weight(i);
    c += appear.distribution(i).mean() * appear.weight(i);
  }
  if (total_weight > 0.0f) {
    expected = c/total_weight;
  }

  return expected;
}

//: color of the most probable mode in the mixtures in the slab
psm_mog_grey_processor::obs_datatype psm_mog_grey_processor::most_probable_color(apm_datatype const& appear)
{

  obs_datatype color = 0;

  if (appear.num_components() > 0) {
    if (appear.weight(0) > 0.0f) {
      color = appear.distribution(0).mean();
    }
  }

  return color;
}

void psm_mog_grey_processor::init_appearance(psm_apm_traits<PSM_APM_MOG_GREY>::obs_datatype const& mean, float variance, psm_apm_traits<PSM_APM_MOG_GREY>::apm_datatype &model)
{
  static const unsigned int nmodes = psm_apm_traits<PSM_APM_MOG_GREY>::n_gaussian_modes_;
  model = bsta_num_obs<bsta_mixture_fixed<bsta_num_obs<bsta_gauss_f1>, nmodes> >();
  bsta_gauss_f1 mode(mean,variance);
  model.insert(bsta_num_obs<bsta_gauss_f1>(mode));
  return;
}


void psm_mog_grey_processor::compute_appearance(vcl_vector<psm_apm_traits<PSM_APM_MOG_GREY>::obs_datatype> const& obs, vcl_vector<float> const& pre, vcl_vector<float> const& vis, psm_apm_traits<PSM_APM_MOG_GREY>::apm_datatype &model, float min_sigma)
{
  static const unsigned int nmodes = psm_apm_traits<PSM_APM_MOG_GREY>::n_gaussian_modes_;
  const float min_var = min_sigma*min_sigma;
  const float big_sigma = (float)vnl_math::sqrt1_2; // maximum possible std. dev for set of samples drawn from [0 1]
  const float big_var = big_sigma * big_sigma;

  model = bsta_num_obs<bsta_mixture_fixed<bsta_num_obs<bsta_gauss_f1>, nmodes> >();

  unsigned int nobs = obs.size();
  if (nobs == 0) {
    // nothing to do.
    return;
  }
  if (obs.size() == 1) {
    bsta_gauss_f1 mode(obs[0], big_sigma*big_sigma);
    model.insert(bsta_num_obs<bsta_gauss_f1>(mode), 1.0f);
    return;
  }
  // use EM to refine estimate until convergence.
  vcl_vector<vcl_vector<float> > mode_probs(nobs);
  for (unsigned int n=0; n<nobs; ++n) {
    for (unsigned int m=0; m<nmodes; ++m) {
      mode_probs[n].push_back(0.0f);
    }
  }
  vcl_vector<float> mode_weight_sum(nmodes,0.0f);

  // initialize parameters
  for (unsigned int m=0; m<nmodes; ++m) {
    float mean = (float(m) + 0.5f) / float(nmodes);
    float sigma = 0.3f;
    float mode_weight = 1.0f / float(nmodes);
    bsta_gauss_f1 mode(mean, sigma*sigma);
    model.insert(bsta_num_obs<bsta_gauss_f1>(mode), mode_weight);
  }
  // run EM algorithm to maximize expected probability of observations
  const unsigned int max_its = 100;
  const float max_converged_weight_change = 1e-4f;

  for (unsigned int i=0; i<max_its; ++i) {
    float max_weight_change = 0.0f;
    // EXPECTATION
    for (unsigned int n=0; n<nobs; ++n) {
      // for each observation, assign probabilities to each mode of appearance model (and to a "previous cell")
      float total_prob = 0.0f;
      vcl_vector<float> new_mode_probs(nmodes);
      for (unsigned int m=0; m<nmodes; ++m) {
        // compute probability that nth data point was produced by mth mode
        const float new_mode_prob = vis[n] * model.distribution(m).prob_density(obs[n]) * model.weight(m);
        new_mode_probs[m] = new_mode_prob; 
        total_prob += new_mode_prob;
      }
      // compute the probability the observation came from an occluding cell
      const float prev_prob = pre[n];
      total_prob += prev_prob;
      if (total_prob > 1e-6) {
        for (unsigned int m=0; m<nmodes; ++m) {
          new_mode_probs[m] /= total_prob;
          const float weight_change = vcl_fabs(new_mode_probs[m] - mode_probs[n][m]);
          if (weight_change > max_weight_change) {
            max_weight_change = weight_change;
          }
          mode_probs[n][m] = new_mode_probs[m];
        }
      }
    }
    // check for convergence
    if (max_weight_change < max_converged_weight_change) {
      break;
    }
    // MAXIMIZATION
    // computed the weighted means and variances for each mode based on the probabilities
    float total_weight_sum = 0.0f;

    // update the mode parameters
    for (unsigned int m=0; m<nmodes; ++m) {
      mode_weight_sum[m] = 0.0f;
      vcl_vector<float> obs_weights(obs.size());
      for (unsigned int n=0; n<obs.size(); ++n) {
        obs_weights[n] = mode_probs[n][m];
        mode_weight_sum[m] += obs_weights[n];
      }
      total_weight_sum += mode_weight_sum[m];
      float mode_mean(0.5f);
      float mode_var(1.0f);
      compute_gaussian_params(obs, obs_weights, mode_mean, mode_var);

      // unbias variance based on number of observations
      //float unbias_factor = sigma_norm_factor(mode_weight_sum[m]);
     // mode_var *= (unbias_factor*unbias_factor);

      // make sure variance does not get too big
      if (!(mode_var < big_var)) {
        mode_var = big_var;
      }
      // or too small
      if (!(mode_var > min_var)) {
        mode_var = min_var;
      }

      // update mode parameters
      model.distribution(m).set_mean(mode_mean);
      model.distribution(m).set_var(mode_var);
    }
    // update mode probabilities
    if (total_weight_sum > 1e-6) {
      for (unsigned int m=0; m<nmodes; ++m) {
        const float mode_weight = mode_weight_sum[m] / total_weight_sum;
        // update mode weight
        model.set_weight(m, mode_weight);
      }
    }
  }

  // unbias variance based on number of observations
  for (unsigned int m=0; m<nmodes; ++m) {
    //float unbias_factor = sigma_norm_factor(mode_weight_sum[m]);
    //float unbias_factor = sigma_norm_factor(model.weight(m) * nobs);
    float unbias_factor = sigma_norm_factor(nobs);

    float mode_var = model.distribution(m).var();
    mode_var *= (unbias_factor*unbias_factor);

    // make sure variance does not get too big
    if (!(mode_var < big_var)) {
      mode_var = big_var;
    }
    // or too small
    if (!(mode_var > min_var)) {
      mode_var = min_var;
    }
    model.distribution(m).set_var(mode_var);
  }

  // sort the modes based on weight
  model.sort();

  return;
}


void psm_mog_grey_processor::update_appearance(vcl_vector<psm_apm_traits<PSM_APM_MOG_GREY>::obs_datatype> const& obs, vcl_vector<float> const& weights, psm_apm_traits<PSM_APM_MOG_GREY>::apm_datatype &model, float min_sigma)
{
  static const unsigned int nmodes = psm_apm_traits<PSM_APM_MOG_GREY>::n_gaussian_modes_;
  const float min_var = min_sigma*min_sigma;
  const float big_sigma = (float)vnl_math::sqrt1_2; // maximum possible std. dev for set of samples drawn from [0 1]
  const float big_var = big_sigma * big_sigma;

  model = bsta_num_obs<bsta_mixture_fixed<bsta_num_obs<bsta_gauss_f1>, nmodes> >();

  // check for degenerate cases
  if (obs.size() == 0) {
    return;
  }
  if (obs.size() == 1) {
    bsta_gauss_f1 mode(obs[0], big_sigma*big_sigma);
    model.insert(bsta_num_obs<bsta_gauss_f1>(mode), 1.0f);
    return;
  }

  // initialize parameters
  for (unsigned int m=0; m<nmodes; ++m) {
    float mean = (float(m) + 0.5f) / float(nmodes);
    float sigma = 0.3f;
    float mode_weight = 1.0f / float(nmodes);
    bsta_gauss_f1 mode(mean, sigma*sigma);
    model.insert(bsta_num_obs<bsta_gauss_f1>(mode), mode_weight);
  }

  // do EM iteration
  const float max_converged_parameter_change = 1e-5f;
  const unsigned int max_its = 100;
  for (unsigned int i=0; i<max_its; ++i) {
    float max_parameter_change = 0.0f; // to determine if we have converged
    // EXPECTATION
    // compute probabilities of the data points belonging to each mode
    vcl_vector<vcl_vector<float> > mode_probs(obs.size());
    for (unsigned int n=0; n<obs.size(); ++n) {
      float total_prob = 0.0f;
      for (unsigned int m=0; m<nmodes; ++m) {
        // compute probability that nth data point was produced by mth mode
        float mode_prob = model.distribution(m).prob_density(obs[n]) * model.weight(m);
        mode_probs[n].push_back(mode_prob);
        total_prob += mode_prob;
      }
      if (total_prob > 1e-6) {
        for (unsigned int m=0; m<nmodes; ++m) {
          mode_probs[n][m] /= total_prob;
        }
      }
    }
    // MAXIMIZATION
    // computed the weighted means and variances for each mode based on the probabilities
    float total_weight_sum = 0.0f;
    vcl_vector<float> mode_weight_sum(nmodes,0.0f);
    // update the mode parameters
    for (unsigned int m=0; m<nmodes; ++m) {
      mode_weight_sum[m] = 0.0f;
      vcl_vector<float> post_weights(obs.size());
      for (unsigned int n=0; n<obs.size(); ++n) {
        post_weights[n] = mode_probs[n][m] * weights[n];
        mode_weight_sum[m] += post_weights[n];
      }
      total_weight_sum += mode_weight_sum[m];
      float mode_mean(0.5f);
      float mode_var(1.0f);
      compute_gaussian_params(obs, post_weights, mode_mean, mode_var);
      // make sure variance does not get too small
      if (!(mode_var > min_var)) {
        mode_var = min_var;
      }
      // update max parameter change
      float mean_change = model.distribution(m).mean() - mode_mean;
      if (mean_change*mean_change > max_parameter_change) {
        max_parameter_change = mean_change*mean_change;
      }
      float var_change = model.distribution(m).var() - mode_var;
      if (var_change*var_change < max_parameter_change) {
        max_parameter_change = var_change*var_change;
      }
      // update mode parameters
      model.distribution(m).set_mean(mode_mean);
      model.distribution(m).set_var(mode_var);
    }
    // update mode probabilities
    if (total_weight_sum > 1e-6) {
      for (unsigned int m=0; m<nmodes; ++m) {
        float mode_weight = mode_weight_sum[m] / total_weight_sum;
        // update max parameter change
        float weight_change = model.weight(m) - mode_weight;
        if (weight_change*weight_change > max_parameter_change) {
          max_parameter_change = weight_change*weight_change;
        }
        // update mode weight
        model.set_weight(m, mode_weight);
      }
    }
    // check for convergence
    if (max_parameter_change <= max_converged_parameter_change) {
      //vcl_cout << "converged after " << i << " iterations." << vcl_endl;
      break;
    }
  }

  // sort the modes based on weight
  model.sort();

  return;
}



void psm_mog_grey_processor::finalize_appearance(vcl_vector<psm_apm_traits<PSM_APM_MOG_GREY>::obs_datatype> const& obs, vcl_vector<float> const& weights, psm_apm_traits<PSM_APM_MOG_GREY>::apm_datatype &model, float min_sigma)
{
  static const unsigned int nmodes = psm_apm_traits<PSM_APM_MOG_GREY>::n_gaussian_modes_;
  const float big_sigma = (float)vnl_math::sqrt1_2; // maximum possible std. dev for set of samples drawn from [0 1]
  const float big_var = big_sigma*big_sigma;

  const unsigned int nobs = obs.size();
  float expected_nobs = 0.0f;
  vcl_vector<float>::const_iterator wit = weights.begin();
  for (; wit != weights.end(); ++wit) {
    expected_nobs += *wit;
  }
  for (unsigned int m=0; m<nmodes; ++m) {
    //float sig_norm = sigma_norm_factor(expected_nobs);
    float sig_norm = sigma_norm_factor(nobs);
    float unbiased_var = model.distribution(m).var() * sig_norm * sig_norm;

    if (unbiased_var > big_var) {
      unbiased_var = big_var;
    }
    model.distribution(m).set_var(unbiased_var);
  }

  return;
}



void psm_mog_grey_processor::compute_gaussian_params(vcl_vector<psm_apm_traits<PSM_APM_MOG_GREY>::obs_datatype> const& obs, vcl_vector<float> const& weights, psm_apm_traits<PSM_APM_MOG_GREY>::obs_datatype &mean, float &variance)
{
  const unsigned int nobs = obs.size();
  double w_sum = 0.0;
  double w2_sum = 0.0;
  double obs_sum = 0.0;

  for (unsigned int i=0; i<nobs; ++i) {
    w_sum += weights[i];
    w2_sum += weights[i]*weights[i];
    obs_sum += obs[i] * weights[i];
  }
  if (w_sum > 1e-6) {
    double mean_obs = obs_sum / w_sum;

    double var_sum = 0.0;
    for (unsigned int i=0; i<nobs; ++i) {
      const double diff = obs[i] - mean_obs;
      var_sum += diff*diff*weights[i];
    }

    variance = (float)(var_sum / (w_sum - (w2_sum/w_sum)));
    mean = (float)mean_obs;
  }

}

float psm_mog_grey_processor::sigma_norm_factor(float nobs)
{
 
  if (nobs <= 1.0f) {
    return sigma_norm_factor((unsigned int)1);
  }

  // linearly interpolate between integer values
  float nobs_floor = vcl_floor(nobs);
  float nobs_ceil = vcl_ceil(nobs);
  float floor_weight = nobs_ceil - nobs;
  float norm_factor = (sigma_norm_factor((unsigned int)nobs_floor) * floor_weight) + (sigma_norm_factor((unsigned int)nobs_ceil) * (1.0f - floor_weight));

  return norm_factor;
}

float psm_mog_grey_processor::sigma_norm_factor(unsigned int nobs)
{
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

