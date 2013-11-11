#ifndef psm_sample_backprojection_h_
#define psm_sample_backprojection_h_

#include <vcl_vector.h>

#include <vil/vil_image_view.h>

#include <vnl/vnl_random.h>

#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_line_3d_2_points.h>

#include <psm/psm_apm_traits.h>
#include <psm/psm_scene.h>

#include "psm_ray_probe.h"

template<psm_apm_type APM>
bool psm_sample_backprojection(psm_scene<APM> &scene, vil_image_view<typename psm_apm_traits<APM>::obs_datatype> const& img, const vpgl_camera<double>* cam, float i, float j, float ij_sigma, bool use_appearance, vcl_vector<vgl_point_3d<double>> &samples, unsigned int n_img_samples = 10, unsigned int n_samples_per_ray = 100)
{

  samples.clear();

  // make sure camera is perspective
  const vpgl_perspective_camera<double>* pcam = dynamic_cast<const vpgl_perspective_camera<double>*>(cam);
  if (!pcam) {
    vcl_cerr << "Error: only perspective cameras supported at this time." << vcl_endl;
    return false;
  }
  vgl_point_3d<double> camera_center(pcam->camera_center());

  // generate a set of image samples
  vnl_random randgen;
  // for each image sample, compute a set of 3-d samples along camera ray
  for (unsigned int n=0; n<n_img_samples; ++n) {
    float rand_i = i + (float)(randgen.normal() * ij_sigma);
    float rand_j = j + (float)(randgen.normal() * ij_sigma);

    vcl_cout << "i = " << rand_i << " j = " << rand_j << vcl_endl;
    
    // generate a ray probe at this pixel location
    vcl_vector<float> depth_vals, alpha_vals, appearance_prob_vals;
    vcl_vector<psm_cell_id> cell_ids;
    psm_ray_probe(scene, img, cam, rand_i, rand_j, depth_vals, alpha_vals, appearance_prob_vals, cell_ids);

    // convert the probe to a probability density function
    vcl_vector<float> cdf(depth_vals.size());
    double alpha_int = 0;
    cdf[0] = 0;
    for (unsigned int x=0; x<cdf.size()-1; ++x) {
      float vis = (float)vcl_exp(-alpha_int);
      double seg_len = depth_vals[x+1] - depth_vals[x];
      if (seg_len <= 0) {
        vcl_cout << "error : depth_val[" << x+1 <<"] = " << depth_vals[x+1] << " depth_val[" << x << "] = " << depth_vals[x] << vcl_endl;
        //return false;
        seg_len = 0;
      }
      alpha_int += alpha_vals[x] * seg_len;
      if (alpha_vals[x] < 0) {
        vcl_cerr << "error: alpha_vals[" << x << "] = " << alpha_vals[x] << vcl_endl;
        return false;
      }
      if (use_appearance) {
        cdf[x+1] = (float)(cdf[x] + vis * (1 - vcl_exp(-alpha_vals[x]*seg_len)) * appearance_prob_vals[x]);
      }
      else {
        cdf[x+1] = (float)(cdf[x] + vis * (1 - vcl_exp(-alpha_vals[x]*seg_len)));
      }
      if (cdf[x+1] < 0) {
        vcl_cerr << "error: cdf[" << x+1 << "] = " << cdf[x+1] << vcl_endl;
        return false;
      }

    }
    // normalize
    vcl_cout << " cdf : ";
    for (unsigned int x=0; x<cdf.size(); ++x) {
      cdf[x] /= cdf[cdf.size()-1];
      vcl_cout << cdf[x] << " ";
    }
    vcl_cout << vcl_endl;

    // compute camera ray
    vgl_line_3d_2_points<double> ray_2pts = pcam->backproject(vgl_point_2d<double>(rand_i,rand_j));
    vgl_vector_3d<double> ray(normalize(ray_2pts.direction()));

    // sample from the pdf
    for (unsigned int s=0; s<n_samples_per_ray; ++s) {
      // generate uniform [0 1] sample, convert using inverse pdf
      float u = (float)randgen.drand64();
      unsigned int x1 = 1;
      for (; x1 < cdf.size() - 1; ++x1) {
        if (cdf[x1] > u) {
          break;
        }
      }
      unsigned int x0 = x1 - 1;
      // linearly interpolate between x0 and x1
      float x0_coef = (cdf[x1] - u)/(cdf[x1] - cdf[x0]);
      float depth_sample = depth_vals[x0] * x0_coef + depth_vals[x1] * (1 - x0_coef);

       // convert the depth sample to a 3-d point
      vgl_point_3d<double> sample = camera_center + ray*depth_sample;
      //vcl_cout << " sample : " << sample << vcl_endl;
      samples.push_back(sample);

    }
  }
  vcl_cout << "returning " << vcl_endl;
  return true;
}


#endif

