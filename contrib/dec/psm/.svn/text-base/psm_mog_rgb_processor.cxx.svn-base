#include "psm_mog_rgb_processor.h"
//:
// \file
#include <bsta/algo/bsta_adaptive_updater.h>
#include <bsta/bsta_distribution.h>
#include <bsta/bsta_gauss_if3.h>
#include <bsta/bsta_attributes.h>
#include <bsta/bsta_mixture_fixed.h>
#include <bsta/bsta_gaussian_indep.h>


#include <vcl_cassert.h>

//: Return probability density of observing pixel values
float psm_mog_rgb_processor::prob_density(apm_datatype const& appear, obs_datatype const& obs)
{
  if (appear.num_components() ==0) {
    return 1.00f;
  }
  else {
    return appear.prob_density(vnl_vector_fixed<float,3>(obs.r,obs.g,obs.b));
  }
}

//: Return probabilities that pixels are in range [min,max]
float psm_mog_rgb_processor::prob_range(apm_datatype const& appear, obs_datatype const& obs_min, obs_datatype const& obs_max)
{
  if (appear.num_components() ==0) {
    return 1.00f;
  }
  else {
    return appear.probability(vnl_vector_fixed<float,3>(obs_min.r,obs_min.g,obs_min.b),vnl_vector_fixed<float,3>(obs_max.r,obs_max.g,obs_max.b));
  }
}


//: Update with a new sample image
bool psm_mog_rgb_processor::update( apm_datatype &appear, obs_datatype const& obs, float const& weight)
{
  // the model
  const float init_variance = 0.008f;
  const float min_stddev = 0.030f;
  const float g_thresh = 2.5; // number of std devs from mean sample must be

  const bsta_gaussian_indep<float,3>::covar_type init_covar(init_variance);
  const bsta_gauss_if3 init_gauss(vnl_vector_fixed<float,3>(0.0f),init_covar);

  const unsigned int nmodes = psm_apm_traits<PSM_APM_MOG_RGB>::n_gaussian_modes_;

  typedef bsta_num_obs<bsta_gauss_if3> gauss_type;
  typedef bsta_mixture_fixed<gauss_type, nmodes> mix_gauss;

  // the updater
  const bsta_mg_grimson_weighted_updater<mix_gauss> updater(init_gauss, nmodes ,g_thresh, min_stddev);

  if (weight > 0) {
    updater(appear, vnl_vector_fixed<float,3>(obs.r,obs.g,obs.b), weight);
  }

  return true;
}


//: Expected value
psm_mog_rgb_processor::obs_datatype psm_mog_rgb_processor::expected_color(apm_datatype const& appear) 
{

  float total_weight= 0;
  vnl_vector_fixed<float,3> c(0.0f);
  obs_datatype expected(0.0f);

  //should be components used
  for (unsigned i = 0; i< appear.num_components(); ++i)
  {
    total_weight += appear.weight(i);
    c += appear.distribution(i).mean() * appear.weight(i);
  }
  if (total_weight > 0.0f) {
    c /= total_weight;
    expected = obs_datatype(c[0],c[1],c[2]);
  }

  return expected;
}

//: color of the most probable mode in the mixtures in the slab
psm_mog_rgb_processor::obs_datatype psm_mog_rgb_processor::most_probable_color(apm_datatype const& appear)
{

  vnl_vector_fixed<float,3> color(0.0f);

  if (appear.num_components() > 0) {
    if (appear.weight(0) > 0.0f) {
      color = appear.distribution(0).mean();
    }
  }
  return obs_datatype(color[0],color[1],color[2]);
}

void psm_mog_rgb_processor::compute_appearance(vcl_vector<psm_apm_traits<PSM_APM_MOG_RGB>::obs_datatype> const& obs, vcl_vector<float> const& pre, vcl_vector<float> const& vis, psm_apm_traits<PSM_APM_MOG_RGB>::apm_datatype &model, float min_sigma)
{
  vcl_cerr << "ERROR: psm_mog_rgb_processor::compute_appearance NOT IMPLEMENTED YET" << vcl_endl;
  //update_appearance(obs,weights,model,min_sigma);
  //finalize_appearance(obs,weights,model,min_sigma);

  return;
}


void psm_mog_rgb_processor::update_appearance(vcl_vector<psm_apm_traits<PSM_APM_MOG_RGB>::obs_datatype> const& obs, vcl_vector<float> const& weights, psm_apm_traits<PSM_APM_MOG_RGB>::apm_datatype &model, float min_sigma)
{
  static const unsigned int nmodes = psm_apm_traits<PSM_APM_MOG_RGB>::n_gaussian_modes_;
  const float min_var = min_sigma*min_sigma;
  const float big_sigma = (float)vnl_math::sqrt1_2; // maximum possible std. dev for set of samples drawn from [0 1]
  const float big_var = big_sigma * big_sigma;

  model = bsta_num_obs<bsta_mixture_fixed<bsta_num_obs<bsta_gauss_if3>, nmodes> >();

  // convert vil_rgb observations to vnl_vector_fixed
  vcl_vector<vnl_vector_fixed<float,3> > vnl_obs(obs.size());
  vcl_vector<vnl_vector_fixed<float,3> >::iterator vnl_obs_it = vnl_obs.begin();
  vcl_vector<psm_apm_traits<PSM_APM_MOG_RGB>::obs_datatype>::const_iterator obs_it = obs.begin();
  for (; vnl_obs_it != vnl_obs.end(); ++vnl_obs_it, ++obs_it) {
    *vnl_obs_it = vnl_vector_fixed<float,3>(obs_it->r, obs_it->g, obs_it->b);
  }

  // check for degenerate cases
  if (vnl_obs.size() == 0) {
    return;
  }
  if (vnl_obs.size() == 1) {
    bsta_gauss_if3 mode(vnl_obs[0], vnl_vector_fixed<float,3>(big_sigma*big_sigma));
    model.insert(bsta_num_obs<bsta_gauss_if3>(mode), 1.0f);
    return;
  }

  // initialize parameters
  for (unsigned int m=0; m<nmodes; ++m) {
    vnl_vector_fixed<float,3> mean((float(m) + 0.5f) / float(nmodes));
    float sigma = 1.0f;
    float mode_weight = 1.0f / float(nmodes);
    bsta_gauss_if3 mode(mean, vnl_vector_fixed<float,3>(sigma*sigma));
    model.insert(bsta_num_obs<bsta_gauss_if3>(mode), mode_weight);
  }

  // do EM iteration
  const float max_converged_parameter_change = 1e-6f;
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
        float mode_prob = model.distribution(m).prob_density(vnl_obs[n]) * model.weight(m);
        mode_probs[n].push_back(mode_prob);
        total_prob += mode_prob;
      }
      if (total_prob > 1e-6) {
        for (unsigned int m=0; m<nmodes; ++m) {
          mode_probs[n][m] /= total_prob;
        }
      } else {
        for (unsigned int m=0; m<nmodes; ++m) {
          mode_probs[n][m] = 0;
        }
      }
        
    }
    // MAXIMIZATION
    // compute the weighted means and variances for each mode based on the probabilities
    float total_weight_sum = 0.0f;
    vcl_vector<float> mode_weight_sum(nmodes,0.0f);
    // update the mode parameters
    for (unsigned int m=0; m<nmodes; ++m) {
      vcl_vector<float> post_weights(obs.size());
      for (unsigned int n=0; n<obs.size(); ++n) {
        post_weights[n] = mode_probs[n][m] * weights[n];
        mode_weight_sum[m] += post_weights[n];
      }
      total_weight_sum += mode_weight_sum[m];
      vnl_vector_fixed<float,3> mode_mean(model.distribution(m).mean());
      vnl_vector_fixed<float,3> mode_var(model.distribution(m).covar());
      compute_gaussian_params(obs, post_weights, mode_mean, mode_var);
      // make sure variance does not get too small
      for (unsigned int d=0; d<3; ++d) {
        if (!(mode_var[d] > min_var)) {
          mode_var[d] = min_var;
        }
      }
      // update max parameter change
      vnl_vector_fixed<float,3> mean_change = model.distribution(m).mean() - mode_mean;
      float mean_change_squared_mag = mean_change.squared_magnitude();
      if (mean_change_squared_mag > max_parameter_change) {
        max_parameter_change = mean_change_squared_mag;
      }
      vnl_vector_fixed<float,3> var_change = model.distribution(m).covar() - mode_var;
      float var_change_squared_mag = var_change.squared_magnitude();
      if (var_change_squared_mag < max_parameter_change) {
        max_parameter_change = var_change_squared_mag;
      }
      // update mode parameters
      model.distribution(m).set_mean(mode_mean);
      model.distribution(m).set_covar(mode_var);
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
    //vcl_cout << "max parameter change = " << max_parameter_change << vcl_endl;

    if (max_parameter_change <= max_converged_parameter_change) {
      //vcl_cout << "converged after " << i << " iterations." << vcl_endl;
      break;
    }
  }

  // sort the modes based on weight
  model.sort();


  return;
}

void psm_mog_rgb_processor::init_appearance(psm_apm_traits<PSM_APM_MOG_RGB>::obs_datatype const& mean, float variance, psm_apm_traits<PSM_APM_MOG_RGB>::apm_datatype &model)
{
  static const unsigned int nmodes = psm_apm_traits<PSM_APM_MOG_RGB>::n_gaussian_modes_;
  model = bsta_num_obs<bsta_mixture_fixed<bsta_num_obs<bsta_gauss_if3>, nmodes> >();
  bsta_gauss_if3 mode(vnl_vector_fixed<float,3>(mean.r, mean.g, mean.b), vnl_vector_fixed<float,3>(variance));
  model.insert(bsta_num_obs<bsta_gauss_if3>(mode));
  return;
}

void psm_mog_rgb_processor::finalize_appearance(vcl_vector<psm_apm_traits<PSM_APM_MOG_RGB>::obs_datatype> const& obs, vcl_vector<float> const& weights, psm_apm_traits<PSM_APM_MOG_RGB>::apm_datatype &model, float min_sigma)
{
  static const unsigned int nmodes = psm_apm_traits<PSM_APM_MOG_RGB>::n_gaussian_modes_;
  const float big_sigma = (float)vnl_math::sqrt1_2; // maximum possible std. dev for set of samples drawn from [0 1]
  const float big_var = big_sigma*big_sigma;

  const unsigned int nobs = obs.size();
  float expected_nobs = 0.0f;
  vcl_vector<float>::const_iterator wit = weights.begin();
  for (; wit != weights.end(); ++wit) {
    expected_nobs += *wit;
  }
  //float sig_norm = sigma_norm_factor(expected_nobs);
  float sig_norm = sigma_norm_factor(nobs);

  for (unsigned int m=0; m<nmodes; ++m) {

    vnl_vector_fixed<float,3> unbiased_var = model.distribution(m).covar() * sig_norm * sig_norm;
    for (unsigned int d=0; d<3; ++d) {
      if (unbiased_var[d] > big_var) {
        unbiased_var[d] = big_var;
      }
    }
    model.distribution(m).set_covar(unbiased_var);
  }

  return;
}



void psm_mog_rgb_processor::compute_gaussian_params(vcl_vector<psm_apm_traits<PSM_APM_MOG_RGB>::obs_datatype> const& obs, vcl_vector<float> const& weights, vnl_vector_fixed<float,3> &mean, vnl_vector_fixed<float,3> &covar)
{
  const unsigned int nobs = obs.size();
  double w_sum = 0.0;
  double w2_sum = 0.0;
  vnl_vector_fixed<double,3> obs_sum(0.0);

  for (unsigned int i=0; i<nobs; ++i) {
    vnl_vector_fixed<double,3> obs_vec(obs[i].r, obs[i].g, obs[i].b);
    w_sum += weights[i];
    w2_sum += weights[i]*weights[i];
    obs_sum += obs_vec * (double)weights[i];
  }



  if (w_sum > 1e-6) {

    vnl_vector_fixed<double,3> mean_obs(obs_sum / w_sum);
    mean =  vnl_vector_fixed<float,3>((float)mean_obs[0], (float)mean_obs[1], (float)mean_obs[2]);

    if (w2_sum > 1e-6) {

      vnl_vector_fixed<double,3> var_sum(0.0);
      for (unsigned int i=0; i<nobs; ++i) {
        vnl_vector_fixed<double,3> obs_vec(obs[i].r, obs[i].g, obs[i].b);
        const vnl_vector_fixed<double,3> diff = obs_vec - mean_obs;
        var_sum += element_product(diff,diff)*(double)weights[i];
      }
      vnl_vector_fixed<double,3> var(var_sum / (w_sum - (w2_sum/w_sum)));
      covar = vnl_vector_fixed<float,3>((float)(var[0]), (float)(var[1]), (float)(var[2]));

    }
  }


  return;
}



float psm_mog_rgb_processor::sigma_norm_factor(float nobs)
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

float psm_mog_rgb_processor::sigma_norm_factor(unsigned int nobs)
{
  // TODO: recompute these values for 3-dimensional (r,g,b) gaussian
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

