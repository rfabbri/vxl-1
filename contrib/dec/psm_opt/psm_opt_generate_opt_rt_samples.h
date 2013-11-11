#ifndef psm_opt_generate_opt_rt_samples_h_
#define psm_opt_generate_opt_rt_samples_h_

#include <vcl_vector.h>

#include <hsds/hsds_fd_tree.h>
#include <hsds/hsds_fd_tree_node_index.h>
#include <psm/psm_scene.h>
#include <psm/psm_sample.h>
#include <psm/psm_apm_traits.h>
#include <psm/psm_aux_traits.h>
#include <psm/psm_mog_grey_processor.h>
#include <psm/psm_mog_rgb_processor.h>
#include <psm/psm_simple_grey_processor.h>
#include <psm/psm_simple_rgb_processor.h>

#include <psm/algo/psm_raytrace_function.h>

#include <vpgl/vpgl_camera.h>

#include <vil/vil_image_view.h>
#include <vil/vil_math.h>
#include <vil/vil_transform.h>
#include <vil/vil_save.h>
#include <vil/vil_crop.h>

#include "psm_opt_rt_sample.h"

template <psm_apm_type APM, psm_aux_type AUX>
class psm_opt_generate_opt_rt_samples_pass0_functor
{
public:
  psm_opt_generate_opt_rt_samples_pass0_functor(vil_image_view<typename psm_apm_traits<APM>::obs_datatype> const& image)
    : obs_(image)
  {}

  inline bool step_cell(unsigned int i, unsigned int j, vgl_point_3d<double> s0, vgl_point_3d<double> s1, psm_sample<APM> &cell_value, typename psm_aux_traits<AUX>::sample_datatype &aux_value, psm_cell_id const& cell_id)
  {
    // compute segment length
    const float seg_len = (float)(s1 - s0).length();
    aux_value.obs_ += obs_(i,j) * seg_len;
    aux_value.seg_len_ += seg_len;
    return true;
  }

private:

  vil_image_view<typename psm_apm_traits<APM>::obs_datatype> const& obs_;

};


template <psm_apm_type APM, psm_aux_type AUX>
class psm_opt_generate_opt_rt_samples_pass1_functor
{
public:
  //: default constructor
  psm_opt_generate_opt_rt_samples_pass1_functor(vil_image_view<typename psm_apm_traits<APM>::obs_datatype> const& image, vil_image_view<float> &pre_inf, vil_image_view<float> &vis_inf) 
    : obs_(image), pre_img_(pre_inf), vis_img_(vis_inf), alpha_integral_(image.ni(), image.nj(), 1)
  {
    alpha_integral_.fill(0.0f);
    pre_img_.fill(0.0f);
    vis_img_.fill(1.0f);
  }

  inline bool step_cell(unsigned int i, unsigned int j, vgl_point_3d<double> s0, vgl_point_3d<double> s1, psm_sample<APM> &cell_value, typename psm_aux_traits<AUX>::sample_datatype &aux_value, psm_cell_id const& cell_id)
  {
    // compute segment length
    const float seg_len = (float)(s1 - s0).length();
    // compute appearance probability of observation
    const float PI = aux_value.PI_;//psm_apm_traits<APM>::apm_processor::prob_density(cell_value.appearance, aux_value.obs_);
    // update alpha integral
    alpha_integral_(i,j) += cell_value.alpha * seg_len;
    // compute new visibility probability with updated alpha_integral
    const float vis_prob_end = vcl_exp(-alpha_integral_(i,j));
    // compute weight for this cell
    const float Omega = vis_img_(i,j) - vis_prob_end;
    // and update pre 
    pre_img_(i,j) +=  PI * Omega;
    // update visibility probabilty
    vis_img_(i,j) = vis_prob_end;

    return true;
  }

private:

  vil_image_view<typename psm_apm_traits<APM>::obs_datatype> const& obs_;
  vil_image_view<float> &vis_img_;
  vil_image_view<float> &pre_img_;
  vil_image_view<float> alpha_integral_;

};

template <psm_apm_type APM, psm_aux_type AUX>
class psm_opt_generate_opt_rt_samples_pass2_functor
{
public:
  //: default constructor
  psm_opt_generate_opt_rt_samples_pass2_functor(vil_image_view<typename psm_apm_traits<APM>::obs_datatype> const& image, vil_image_view<float> const& Beta_denom) 
    : obs_(image), Beta_denom_(Beta_denom), pre_img_(image.ni(), image.nj(), 1), vis_img_(image.ni(), image.nj(), 1), alpha_integral_(image.ni(), image.nj(), 1)
  {
    alpha_integral_.fill(0.0f);
    pre_img_.fill(0.0f);
    vis_img_.fill(1.0f);
  }

  inline bool step_cell(unsigned int i, unsigned int j, vgl_point_3d<double> s0, vgl_point_3d<double> s1, psm_sample<APM> &cell_value, typename psm_aux_traits<AUX>::sample_datatype &aux_value, psm_cell_id const& cell_id)
  {
    // compute segment length
    const float seg_len = (float)(s1 - s0).length();
    // compute appearance probability of observation
    const float PI = aux_value.PI_;
    // update alpha integral
    alpha_integral_(i,j) += cell_value.alpha * seg_len;
    // compute new visibility probability with updated alpha_integral
    const float vis_prob_end = vcl_exp(-alpha_integral_(i,j));
    // grab this cell's pre and vis value
    const float pre = pre_img_(i,j);
    const float vis = vis_img_(i,j);
    // compute weight for this cell
    const float Omega = vis - vis_prob_end;
    // update vis and pre
    pre_img_(i,j) +=  PI * Omega;
    vis_img_(i,j) = vis_prob_end;

    // accumulate aux sample values
    //aux_value.obs_ += obs_(i,j) * seg_len;
    aux_value.pre_ += pre * seg_len;
    //aux_value.Omega_ += Omega * seg_len;
    aux_value.vis_ += vis * seg_len;
    //float post_prob = Beta_denom_(i,j) - pre - PI*Omega;
    //if (post_prob < -1e-5) {
    //  vcl_cerr << "error: post_prob = " << post_prob << "  setting to 0." << vcl_endl;
    //  vcl_cerr << "Beta_denom = " << Beta_denom_(i,j) << ", pre = " << pre << ", PI = " << PI << " Omega = " << Omega << vcl_endl;
    //}
    //if (post_prob < 0) {
    //  post_prob = 0;
    //}
    //aux_value.post_prob_ += post_prob * seg_len;

    const float Beta_num = pre + vis*PI;
    float Beta = 1.0f;
    if (Beta_denom_(i,j) < 1e-6) {
      if (Beta_num > 1e-5) {
        Beta = 10.0f;
      }
    }
    else {
      Beta = Beta_num / Beta_denom_(i,j);
    }
    if (Beta < 0) {
      if (Beta < -1e-5) {
        vcl_cerr << " error: beta = " << Beta << "  setting to 0. " << vcl_endl;
      }
      Beta = 0;
    }
    const float old_PQ = (1.0f - (float)vcl_exp(-seg_len * cell_value.alpha));
    float new_PQ = Beta * old_PQ;

    const double obs_prob = 1.0;//vis;
    double pass_prob = 1.0 - new_PQ;
    if (pass_prob < 1e-5)
      pass_prob = 1e-5; // to avoid taking log of 0

    aux_value.updated_alpha_sum_ += (float)(vcl_log(pass_prob)*obs_prob);
    aux_value.weighted_seg_len_ += (float)(seg_len * obs_prob);

#define PSM_DEBUG
#ifdef PSM_DEBUG
    const psm_cell_id debug_cell(vgl_point_3d<int>(-1,-1,0),hsds_fd_tree_node_index<3>(3911065088,8));
    const vcl_string debug_fname("c:/research/psm/output/psm_debug.txt");
    if (cell_id == debug_cell) {
      // open debug file in append mode
      vcl_ofstream ofs(debug_fname.c_str(),vcl_ios::app);
      ofs << aux_value.obs_ << " " << seg_len << " " << pre << " " << vis << " " << PI << " " << Beta_denom_(i,j) << " " << old_PQ << " " << new_PQ << vcl_endl;
    }
#endif

    return true;
  }

private:

  vil_image_view<typename psm_apm_traits<APM>::obs_datatype> const& obs_;
  vil_image_view<float> const& Beta_denom_;
  //vil_image_view<float> const& pre_inf_;
  vil_image_view<float> vis_img_;
  vil_image_view<float> pre_img_;
  vil_image_view<float> alpha_integral_;

};




template <psm_apm_type APM, psm_aux_type AUX>
void psm_opt_generate_opt_rt_samples(psm_scene<APM> &scene, vpgl_camera<double> const* cam, vil_image_view<typename psm_apm_traits<APM>::obs_datatype> const& img, vcl_string image_id, bool black_background = false)
{
  // create a temporary aux_scene
  // assuming greyscale image and scene here.  Need a map from appearance model -> aux_type to make this general.
  scene.template init_aux_scene<AUX>(image_id);
  psm_aux_scene_base_sptr aux_scene_ptr = scene.template get_aux_scene<AUX>(image_id);

#if 0
  vcl_cout << "computing mean cell projections.. " << vcl_endl;
  // First, compute and store mean observation and PI value for each cell.
  psm_aux_scene<AUX> *aux_scene = static_cast<psm_aux_scene<AUX>*>(aux_scene_ptr.ptr());
  typename psm_aux_scene<AUX>::block_index_iterator block_index_it = scene.block_index_begin();
  for (; block_index_it != scene.block_index_end(); ++block_index_it) {
    //hsds_fd_tree<psm_sample<APM>,3> &block = scene.get_block(*block_index_it);
    hsds_fd_tree<typename psm_aux_traits<AUX>::sample_datatype,3> &aux_block = aux_scene->get_block(*block_index_it);
    // iterate through cells in block
    //typename hsds_fd_tree<psm_sample<APM>>::iterator block_it = block.begin();
    typename hsds_fd_tree<typename psm_aux_traits<AUX>::sample_datatype,3>::iterator aux_block_it = aux_block.begin();
    for (; aux_block_it != aux_block.end(); ++aux_block_it) {
      // get bounding box of cell
      vbl_bounding_box<double,3> cube = aux_block.cell_bounding_box(aux_block_it->first);
      vbl_bounding_box<unsigned int,2> projection;
      double xverts_3d[] = {cube.xmin(), cube.xmax()};
      double yverts_3d[] = {cube.ymin(), cube.ymax()};
      double zverts_3d[] = {cube.zmin(), cube.zmax()};
      // project cell into image
      for (unsigned int k=0; k<2; ++k) {
        for (unsigned int j=0; j<2; ++j) {
          for (unsigned int i=0; i<2; ++i) {
            double u,v;
            cam->project(xverts_3d[i],yverts_3d[j],zverts_3d[k],u,v);
            const unsigned int u_int = vcl_min((unsigned int)(vcl_max(u,0.0)),img.ni()-1);
            const unsigned int v_int = vcl_min((unsigned int)(vcl_max(v,0.0)),img.nj()-1);
            projection.update(u_int,v_int);
          }
        }
      }
      // get mean value of pixels within projection bounding box
      const unsigned int xdim = projection.xmax() - projection.xmin() + 1;
      const unsigned int ydim = projection.ymax() - projection.ymin() + 1;
      vil_image_view<typename psm_apm_traits<APM>::obs_datatype> cell_view = vil_crop(img,projection.xmin(),xdim,projection.ymin(),ydim);
      typename psm_apm_traits<APM>::obs_datatype mean_obs;
      vil_math_mean(mean_obs,cell_view,0);
      //vcl_cout << "mean = " << mean_obs << "  xdim = " << xdim << "  ydim = " << ydim << vcl_endl;
      // update aux cell
      aux_block_it->second.obs_ = mean_obs;

    }
  }
#endif
  // 0th pass: traverse and compute mean cell observations.
  vcl_cout << "compute mean cell observations.." << vcl_endl;
  psm_raytrace_function<psm_opt_generate_opt_rt_samples_pass0_functor<APM,AUX>, APM, AUX> raytrace_fn0(scene, aux_scene_ptr, cam, img.ni(), img.nj(), false);
  psm_opt_generate_opt_rt_samples_pass0_functor<APM,AUX> pass0_functor(img);
  raytrace_fn0.run(pass0_functor);

  vcl_cout << "normalizing.." << vcl_endl;
  // normalize appearance values
  psm_aux_scene<AUX> *aux_scene = static_cast<psm_aux_scene<AUX>*>(aux_scene_ptr.ptr());
  typename psm_aux_scene<AUX>::block_index_iterator block_index_it = scene.block_index_begin();
  for (; block_index_it != scene.block_index_end(); ++block_index_it) {
    hsds_fd_tree<psm_sample<APM>,3> &block = scene.get_block(*block_index_it);
    hsds_fd_tree<typename psm_aux_traits<AUX>::sample_datatype,3> &aux_block = aux_scene->get_block(*block_index_it);
    // iterate through cells in block
    typename hsds_fd_tree<psm_sample<APM>,3>::iterator block_it = block.begin();
    typename hsds_fd_tree<typename psm_aux_traits<AUX>::sample_datatype,3>::iterator aux_block_it = aux_block.begin();
    for (; aux_block_it != aux_block.end(); ++aux_block_it, ++block_it) {
      const float total_seg_len = aux_block_it->second.seg_len_;
      if (total_seg_len > 0) {
        aux_block_it->second.obs_ /= total_seg_len;
        aux_block_it->second.PI_ = psm_apm_traits<APM>::apm_processor::prob_density(block_it->second.appearance, aux_block_it->second.obs_);
        //aux_block_it->second.seg_len_ = 0;
      }
    }
  }

  // first pass: traverse in forward direction, accumulating vis_inf and pre_inf
  vcl_cout << "first pass.." << vcl_endl;
  vil_image_view<float> vis_inf(img.ni(), img.nj());
  vil_image_view<float> pre_inf(img.ni(), img.nj());
  psm_raytrace_function<psm_opt_generate_opt_rt_samples_pass1_functor<APM,AUX>, APM, AUX> raytrace_fn1(scene, aux_scene_ptr, cam, img.ni(), img.nj(), false);
  psm_opt_generate_opt_rt_samples_pass1_functor<APM,AUX> pass1_functor(img, pre_inf, vis_inf);
  raytrace_fn1.run(pass1_functor);

  vil_image_view<float> PI_inf(img.ni(), img.nj());
  if (black_background) {
    vcl_cout << "using black background" << vcl_endl;
    psm_apm_traits<APM>::obs_datatype black(0.0f);
    float background_std_dev = 8.0f/255;
    typename psm_apm_traits<APM>::apm_datatype background_apm;
    psm_apm_traits<APM>::apm_processor::init_appearance(black, background_std_dev*background_std_dev, background_apm);
    vil_image_view<typename psm_apm_traits<APM>::obs_datatype>::const_iterator img_it = img.begin();
    vil_image_view<float>::iterator PI_it = PI_inf.begin();
    for (; img_it != img.end(); ++img_it, ++PI_it) {
      *PI_it = psm_apm_traits<APM>::apm_processor::prob_density(background_apm,*img_it);
    }
  }
  else {
    PI_inf.fill(1.0f); // uniform distribution [0, 1]
  }

  vil_image_view<float> inf_term(img.ni(), img.nj());
  vil_math_image_product(vis_inf,PI_inf,inf_term);
  vil_image_view<float> Beta_denom_img(img.ni(), img.nj());
  vil_math_image_sum(pre_inf,inf_term,Beta_denom_img);

  vcl_cout << "saving debug images.." << vcl_endl;
  vil_save(vis_inf,"c:/research/psm/output/vis_inf.tiff");
  vil_save(pre_inf,"c:/research/psm/output/pre_inf.tiff");
  vil_save(Beta_denom_img,"c:/research/psm/output/beta_denom_img.tiff");

  vcl_cout << "second pass.." << vcl_endl;
  psm_raytrace_function<psm_opt_generate_opt_rt_samples_pass2_functor<APM,AUX>, APM, AUX> raytrace_fn2(scene, aux_scene_ptr, cam, img.ni(), img.nj(), false);
  psm_opt_generate_opt_rt_samples_pass2_functor<APM,AUX> pass2_functor(img, Beta_denom_img);
  raytrace_fn2.run(pass2_functor);

  vcl_cout << "done." << vcl_endl;

  return;
}

#endif

