#ifndef psm_opt_generate_opt_samples_h_
#define psm_opt_generate_opt_samples_h_

#include <vcl_vector.h>

#include <hsds/hsds_fd_tree.h>
#include <psm/psm_scene.h>
#include <psm/psm_sample.h>
#include <psm/psm_apm_traits.h>
#include <psm/psm_aux_traits.h>
#include <psm/psm_mog_grey_processor.h>
#include <psm/psm_simple_grey_processor.h>
#include <psm/psm_simple_rgb_processor.h>

#include <psm/algo/psm_parallel_raytrace_function.h>

#include <vpgl/vpgl_camera.h>

#include <vil/vil_image_view.h>
#include <vil/vil_math.h>
#include <vil/vil_transform.h>
#include <vil/vil_save.h>

#include "psm_opt_sample.h"




template <psm_apm_type APM, psm_aux_type AUX>
class psm_opt_generate_opt_samples_forward_functor
{
public:
  //: default constructor
  psm_opt_generate_opt_samples_forward_functor(vpgl_camera<double> const* cam, vil_image_view<typename psm_apm_traits<APM>::obs_datatype> &image) 
    : cam_(cam), obs_(image),
    PI_img_(image.ni(),image.nj(),1), alpha_img_(image.ni(),image.nj(),1), alpha_integral_(image.ni(),image.nj(),1), vis_(image.ni(), image.nj(),1), vis_end_(image.ni(),image.nj(),1), 
    pix_weights_(image.ni(),image.nj(),1), pre_img_(image.ni(),image.nj(),1), seg_len_img_(image.ni(),image.nj(),1)
  {
    step_count_ = 0;
    vis_.fill(1.0f);
    pre_img_.fill(0.0f);
    alpha_integral_.fill(0.0f);
  }

  //: accumulate 
  inline bool step_cells(vgl_point_3d<int> const& block_idx, hsds_fd_tree<psm_sample<APM>,3> &block, hsds_fd_tree<typename psm_aux_traits<AUX>::sample_datatype,3> &aux_block, vcl_vector<hsds_fd_tree_node_index<3> > &cells)
  {
    ++step_count_;
    alpha_img_.fill(0.0f);
    PI_img_.fill(0.0f);
    seg_len_img_.fill(0.0f);

    double xverts_2d[8], yverts_2d[8];
    float vert_dists[8];
    psm_cube_face_list visible_faces;

    // project each cell into the image
    vcl_vector<hsds_fd_tree_node_index<3> >::iterator cell_it = cells.begin();

    for (; cell_it != cells.end(); ++cell_it) {
      //if (cell_it->idx == 0x32f20000) {
      //  vcl_cout << "debug_break" << vcl_endl;
      //}
      psm_sample<APM> &cell_value = block[*cell_it];
      // get aux value
      psm_opt_sample<typename psm_apm_traits<APM>::obs_datatype> &aux_value = aux_block[*cell_it];
      // get vertices of cell in the form of a bounding box (cells are always axis-aligned))
      vbl_bounding_box<double,3> cell_bb = block.cell_bounding_box(*cell_it);
      // project vertices to the image determine which faces of the cell are visible
      project_cube_vertices(cell_bb, cam_, xverts_2d, yverts_2d, vert_dists, visible_faces, block.bounding_box().zmax());
      // get ray lengths * alpha
      alpha_seg_len(xverts_2d, yverts_2d, vert_dists, visible_faces, cell_value.alpha, alpha_img_);
      // get weighted mean of vis 
      float mean_vis = 0.0f;
      if (cube_mean(xverts_2d, yverts_2d, vert_dists, visible_faces, vis_, mean_vis)) {
        aux_value.vis_ = mean_vis;
      }
      float mean_pre = 0.0f;
      if (cube_mean(xverts_2d, yverts_2d, vert_dists, visible_faces, pre_img_, mean_pre)) {
        aux_value.pre_ = mean_pre;
      }
#ifdef PSM_OPT_USE_PROB_RANGE
      if(aux_value.pre_ > 1.0f) {
        vcl_cout << "error: mean_pre = " << mean_pre << ", aux_value.pre_ = " << aux_value.pre_ << ", step_count = " << step_count_ << vcl_endl;
      }
#endif
      // if cell is not fully contained in image, keep seg_len_ at 0.  This will preclude this image from being included in optimization of cells values.
      // changed back to cube_visible
      if (full_cube_visible(xverts_2d,yverts_2d,obs_.ni(),obs_.nj())) {
        // get ray lengths
        alpha_seg_len(xverts_2d, yverts_2d, vert_dists, visible_faces, 1.0f, seg_len_img_);
        // get weighted mean of seg_len
        float mean_seg_len = 0.0f;
        if (cube_mean(xverts_2d, yverts_2d, vert_dists, visible_faces, seg_len_img_, mean_seg_len)) {
          aux_value.seg_len_ = mean_seg_len;
        }
      } 
      // get weighted mean of observation at cell
      typename psm_apm_traits<APM>::obs_datatype cell_mean_obs;
      if (cube_mean(xverts_2d, yverts_2d, vert_dists, visible_faces, obs_, cell_mean_obs)) {
        aux_value.obs_ = cell_mean_obs;
        // get probability density of mean observation
#ifdef PSM_OPT_USE_PROB_RANGE
        float cell_PI = psm_apm_traits<APM>::apm_processor::prob_range(cell_value.appearance, cell_mean_obs - obs_range_, cell_mean_obs + obs_range_);
        if (!((cell_PI >= 0) && (cell_PI <= 1)) ) {
          vcl_cout << vcl_endl << "cell_PI = " << cell_PI << vcl_endl;
          vcl_cout << "  cell_obs = " << cell_mean_obs << vcl_endl;
          vcl_cout << "  cell id = " << *cell_it << vcl_endl; 
        }
#else
        float cell_PI = psm_apm_traits<APM>::apm_processor::prob_density(cell_value.appearance, cell_mean_obs);
        if (!(cell_PI >= 0)) {
          vcl_cout << "cell_PI = " << cell_PI << vcl_endl;
          vcl_cout << "   cell_obs = " << cell_mean_obs << vcl_endl;
          vcl_cout << "   cell id = " << *cell_it << vcl_endl;
        }
#endif
        // fill obs probability density image
        cube_fill_value(xverts_2d, yverts_2d, visible_faces, PI_img_, cell_PI);
        aux_value.PI_ = cell_PI;
      }
    }
    // update full alpha_integral image
    vil_math_image_difference(alpha_integral_, alpha_img_, alpha_integral_);
    // compute new vis image
    image_exp_functor exp_fun;
    vil_transform(alpha_integral_,vis_end_,exp_fun);
    // compute weights for each pixel
    vil_math_image_difference(vis_,vis_end_,pix_weights_);
  
#define PSM_DEBUG
#ifdef PSM_DEBUG
    if (step_count_ == 100) {
      vcl_cout << "saving debug images..";
      vcl_string output_dir = "c:/research/psm/output/";
      vil_save(alpha_img_,(output_dir + "alpha_img.tiff").c_str());
      vil_save(alpha_integral_,(output_dir + "alpha_integral.tiff").c_str());
      vil_save(pix_weights_,(output_dir + "pix_weights.tiff").c_str());
      vil_save(pre_img_,(output_dir + "pre_img.tiff").c_str());
      vil_save(PI_img_,(output_dir + "PI_img.tiff").c_str());
      vil_save(vis_,(output_dir + "vis.tiff").c_str());
      vil_save(vis_end_,(output_dir + "vis_end.tiff").c_str());
      vil_save(obs_,(output_dir + "obs.tiff").c_str());
      vcl_cout << "..done" << vcl_endl;
    }
#endif

     // multiply cell_PI by cell weights
    vil_math_image_product(pix_weights_, PI_img_, PI_img_);
    // update pre_img 
    vil_math_image_sum(PI_img_, pre_img_, pre_img_);


    // update vis_image_
    vis_.deep_copy(vis_end_);

    return true;
  }

  void finish(psm_scene<APM> &scene, psm_aux_scene<AUX> &aux_scene)
  {
    
    return;
  }

  void set_background_model(typename psm_apm_traits<APM>::apm_datatype &background_apm) {
    background_model_ = background_apm;
  }

  vil_image_view<float> get_vis() { return vis_; }

private:
  int step_count_;

  //float obs_range_;

  vpgl_camera<double> const* cam_;
  vil_image_view<typename psm_apm_traits<APM>::obs_datatype> &obs_;
  vil_image_view<float> vis_;
  vil_image_view<float> alpha_integral_;
  vil_image_view<float> seg_len_img_;

  // scratch images for computation
  vil_image_view<float> pre_img_;
  vil_image_view<float> PI_img_;
  vil_image_view<float> alpha_img_;
  vil_image_view<float> vis_end_;
  vil_image_view<float> pix_weights_;

  // background appearance model
  typename psm_apm_traits<APM>::apm_datatype background_model_;

  //: Functor class to compute exponential of image
  class image_exp_functor
  {
  public:
    float operator()(float x)       const { return x<0?vcl_exp(x):1.0f; }
  };

};





template <psm_apm_type APM, psm_aux_type AUX>
class psm_opt_generate_opt_samples_backward_functor
{
public:
  //: default constructor
  psm_opt_generate_opt_samples_backward_functor(vpgl_camera<double> const* cam, vil_image_view<typename psm_apm_traits<APM>::obs_datatype> &image) 
    : cam_(cam), obs_(image), 
    alpha_img_(image.ni(),image.nj(),1), post_(image.ni(),image.nj(),1), pass_prob_img_(image.ni(),image.nj(),1), occlusion_prob_img_(image.ni(),image.nj(),1), PI_img_(image.ni(),image.nj(),1)
  {
    step_count_ = 0;
    // initialize post_ with PI_inf
#ifdef PSM_OPT_USE_PROB_RANGE
    post_.fill(obs_range*2); // assumes uniform background model in range [0,1]
#else
    post_.fill(1.0f);
#endif
  }

  //: accumulate 
  inline bool step_cells(vgl_point_3d<int> const& block_idx, hsds_fd_tree<psm_sample<APM>,3> &block, hsds_fd_tree<typename psm_aux_traits<AUX>::sample_datatype,3> &aux_block, vcl_vector<hsds_fd_tree_node_index<3> > &cells)
  {
    ++step_count_;
    alpha_img_.fill(0.0f);
    PI_img_.fill(0.0f);

    double xverts_2d[8], yverts_2d[8];
    float vert_dists[8];
    psm_cube_face_list visible_faces;

    // project each cell into the image
    vcl_vector<hsds_fd_tree_node_index<3> >::iterator cell_it = cells.begin();
    for (; cell_it != cells.end(); ++cell_it) {
 
      psm_sample<APM> &cell_value = block[*cell_it];
      // get aux value
      psm_opt_sample<typename psm_apm_traits<APM>::obs_datatype> &aux_value = aux_block[*cell_it];
      // get vertices of cell in the form of a bounding box (cells are always axis-aligned))
      vbl_bounding_box<double,3> cell_bb = block.cell_bounding_box(*cell_it);
      // project vertices to the image determine which faces of the cell are visible
      project_cube_vertices(cell_bb, cam_, xverts_2d, yverts_2d, vert_dists, visible_faces);
      // get ray lengths * alpha
      alpha_seg_len(xverts_2d, yverts_2d, vert_dists, visible_faces, cell_value.alpha, alpha_img_);
      // get weighted mean of post_
      float mean_post = 0.0f;
      if (cube_mean(xverts_2d, yverts_2d, vert_dists, visible_faces, post_, mean_post)) {
        aux_value.post_ = mean_post;
      }
      // fill obs probability density image
      cube_fill_value(xverts_2d, yverts_2d, visible_faces, PI_img_, aux_value.PI_);
    }
    // compute pass_prob
    image_exp_neg_functor exp_fun;
    vil_transform(alpha_img_,pass_prob_img_,exp_fun);
    // compute occlusion probability
    image_inv_prob_functor inv_prob_fun;
    vil_transform(pass_prob_img_,occlusion_prob_img_,inv_prob_fun);

    // update post_img
    vil_math_image_product(pass_prob_img_,post_,post_);
    vil_math_image_product(occlusion_prob_img_,PI_img_,PI_img_);
    vil_math_image_sum(PI_img_,post_,post_);

#ifdef PSM_DEBUG
    if (step_count_ == 100) {
      vcl_cout << "saving debug images..";
      vcl_string output_dir = "c:/research/psm/output/";
      vil_save(alpha_img_,(output_dir + "alpha_img_post.tiff").c_str());
      vil_save(post_,(output_dir + "post.tiff").c_str());
      vil_save(PI_img_,(output_dir + "PI_img_post.tiff").c_str());
      vil_save(pass_prob_img_,(output_dir + "pass_prob_img.tiff").c_str());
      vil_save(occlusion_prob_img_,(output_dir + "occlusion_prob_img.tiff").c_str());
      vcl_cout << "..done" << vcl_endl;
    }
#endif


    return true;
  }

  void finish(psm_scene<APM> &scene, psm_aux_scene<AUX> &aux_scene)
  {
    
    return;
  }

  
  void set_background_model(typename psm_apm_traits<APM>::apm_datatype &background_apm) {
    //background_model_ = background_apm;
    // compute probability of background producing observed intensities
    for (unsigned int j=0; j<obs_.nj(); ++j) {
      for (unsigned int i=0; i<obs_.ni(); ++i) {
#ifdef PSM_OPT_USE_PROB_RANGE
        post_(i,j) = psm_apm_traits<APM>::apm_processor::prob_range(background_apm, obs_(i,j) - obs_range, obs_(i,j) + obs_range);
#else
        // this will have to be modified slightly when we deal with multi-plane images -dec
        post_(i,j) = psm_apm_traits<APM>::apm_processor::prob_density(background_apm, obs_(i,j));
#endif
      }
    }
  }

private:
  int step_count_;

  //float obs_range_;

  vpgl_camera<double> const* cam_;
  vil_image_view<typename psm_apm_traits<APM>::obs_datatype> &obs_;
  vil_image_view<float> post_;

  // scratch images for computation
  vil_image_view<float> PI_img_;
  vil_image_view<float> alpha_img_;
  vil_image_view<float> pass_prob_img_;
  vil_image_view<float> occlusion_prob_img_;

  // background appearance model
  //typename psm_apm_traits<APM>::apm_datatype background_model_;

  //: Functor class to compute exponential of image
  class image_exp_neg_functor
  {
  public:
    float operator()(float x)       const { return x>0?vcl_exp(-x):1.0f; }
  };

  //: Functor for subtracting image from 1 to get inverse probability
  class image_inv_prob_functor
  {
  public:
    float operator()(float x) const {return (1.0f - x);}
  };

};


template <psm_apm_type APM, psm_aux_type AUX>
void psm_opt_generate_opt_samples(psm_scene<APM> &scene, vpgl_camera<double> const* cam, vil_image_view<typename psm_apm_traits<APM>::obs_datatype> &img, vcl_string image_id, bool black_background = false)
{
  // create a temporary aux_scene
  // assuming greyscale image and scene here.  Need a map from appearance model -> aux_type to make this general.
  scene.template init_aux_scene<AUX>(image_id);
  psm_aux_scene_base_sptr aux_scene_ptr = scene.template get_aux_scene<AUX>(image_id);

  // first pass: traverse in forward direction, filling in "obs", "seg_len", "pre", and "vis" values
  vcl_cout << "first pass.." << vcl_endl;
  psm_parallel_raytrace_function<psm_opt_generate_opt_samples_forward_functor<APM, AUX>, APM, AUX> raytrace_fn1(scene, aux_scene_ptr, cam, img.ni(), img.nj(), false, false);
  psm_opt_generate_opt_samples_forward_functor<APM,AUX> fwd_functor(cam, img);
  raytrace_fn1.run(fwd_functor);

  vcl_cout << "second pass.." << vcl_endl;
  // second pass: traverse in backwards direction, filling in "post" values
  psm_parallel_raytrace_function<psm_opt_generate_opt_samples_backward_functor<APM, AUX>, APM, AUX> raytrace_fn2(scene, aux_scene_ptr, cam, img.ni(), img.nj(), false, true);
  psm_opt_generate_opt_samples_backward_functor<APM,AUX> back_functor(cam, img);

  // for the middlebury datasets, the background is black - update background model accordingly
  if (black_background) {
    vcl_cout << "using black background model" << vcl_endl;
    psm_apm_traits<APM>::obs_datatype black(0.0f);
    float background_std_dev = 4.0f/255;
    typename psm_apm_traits<APM>::apm_datatype background_apm(black, background_std_dev);

    float peak = psm_apm_traits<APM>::apm_processor::prob_density(background_apm,0.0f);
    vcl_cout << "p(0) = " << peak <<  vcl_endl;
    vcl_cout << "sigma = " << vnl_math::two_over_sqrtpi * vnl_math::sqrt1_2 / (2*peak) << vcl_endl;

    back_functor.set_background_model(background_apm);
  }

  raytrace_fn2.run(back_functor);
  vcl_cout << "done." << vcl_endl;

  return;
}

#endif

