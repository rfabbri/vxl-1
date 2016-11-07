#ifndef psm_pixel_probability_h_
#define psm_pixel_probability_h_

#include <vcl_vector.h>

#include <hsds/hsds_fd_tree.h>
#include <psm/psm_scene.h>
#include <psm/psm_sample.h>
#include <psm/psm_apm_traits.h>
#include <psm/psm_aux_traits.h>

#include <psm/psm_mog_grey_processor.h>
#include <psm/psm_simple_grey_processor.h>
#include <psm/psm_mog_rgb_processor.h>
#include <psm/psm_simple_rgb_processor.h>

#include <vpgl/vpgl_camera.h>

#include <vil/vil_image_view.h>
#include <vil/vil_math.h>
#include <vil/vil_transform.h>

#include "psm_parallel_raytrace_function.h"




template <psm_apm_type APM>
class psm_pixel_probability_functor
{
public:
  //: default constructor
  psm_pixel_probability_functor(vpgl_camera<double> const* cam, vil_image_view<typename psm_apm_traits<APM>::obs_datatype> const& image, vil_image_view<float> &prob, float obs_range) 
    : cam_(cam), obs_(image), prob_(prob), obs_range_(obs_range),
    PI_img_(image.ni(),image.nj(),1), alpha_img_(image.ni(),image.nj(),1), alpha_integral_(image.ni(),image.nj(),1), vis_(image.ni(), image.nj(),1), vis_end_(image.ni(),image.nj(),1), 
    pix_weights_(image.ni(),image.nj(),1), pre_img_(image.ni(),image.nj(),1)
  {
    step_count_ = 0;
    vis_.fill(1.0f);
    pre_img_.fill(0.0f);
    alpha_integral_.fill(0.0f);
  }

  //: accumulate 
  inline bool step_cells(vgl_point_3d<int> const& block_idx, hsds_fd_tree<psm_sample<APM>,3> &block, hsds_fd_tree<psm_aux_traits<PSM_AUX_NULL>::sample_datatype,3> &aux_block, vcl_vector<hsds_fd_tree_node_index<3> > &cells)
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
      // get vertices of cell in the form of a bounding box (cells are always axis-aligned))
      vbl_bounding_box<double,3> cell_bb = block.cell_bounding_box(*cell_it);
      // project vertices to the image determine which faces of the cell are visible
      project_cube_vertices(cell_bb, cam_, xverts_2d, yverts_2d, vert_dists, visible_faces);
      // get ray lengths * alpha
      alpha_seg_len(xverts_2d, yverts_2d, vert_dists, visible_faces, cell_value.alpha, alpha_img_);       
      // get weighted mean of observation at cell
      typename psm_apm_traits<APM>::obs_datatype cell_mean_obs;
      if (cube_mean(xverts_2d, yverts_2d, vert_dists, visible_faces, obs_, cell_mean_obs)) {
        // get probability density of mean observation
        float cell_PI = psm_apm_traits<APM>::apm_processor::prob_range(cell_value.appearance, cell_mean_obs - obs_range_, cell_mean_obs + obs_range_);
        if (!((cell_PI >= 0) && (cell_PI < 1e8)) ) {
          vcl_cout << vcl_endl << "cell_PI = " << cell_PI << vcl_endl;
          vcl_cout << "  cell_obs = " << cell_mean_obs << vcl_endl;
          vcl_cout << "  cell id = " << *cell_it << vcl_endl; 
        }
        // fill obs probability density image
        cube_fill_value(xverts_2d, yverts_2d, visible_faces, PI_img_, cell_PI);
      }
    }
    // update full alpha_integral image
    vil_math_image_difference(alpha_integral_, alpha_img_, alpha_integral_);
    // compute new vis image
    image_exp_functor exp_fun;
    vil_transform(alpha_integral_,vis_end_,exp_fun);
    // compute weights for each pixel
    vil_math_image_difference(vis_,vis_end_,pix_weights_);

    // multiply cell_PI by cell weights
    vil_math_image_product(pix_weights_, PI_img_, PI_img_);
    // update pre_img 
    vil_math_image_sum(PI_img_, pre_img_, pre_img_);

    // update vis_image_
    vis_.deep_copy(vis_end_);

    return true;
  }

  void finish(psm_scene<APM> &scene, psm_aux_scene<PSM_AUX_NULL> &aux_scene)
  {
    // compute probability of background producing observed intensities
    vil_image_view<float> PI_background(obs_.ni(), obs_.nj(), 1);
    for (unsigned int j=0; j<obs_.nj(); ++j) {
      for (unsigned int i=0; i<obs_.ni(); ++i) {
        // this will have to be modified slightly when we deal with multi-plane images -dec
        PI_background(i,j) = psm_apm_traits<APM>::apm_processor::prob_range(background_model_, obs_(i,j) - obs_range_, obs_(i,j) + obs_range_);
      }
    }
    vil_image_view<float> back_component(obs_.ni(),obs_.nj(),1);
    vil_math_image_product(PI_background, vis_, back_component);
    vil_math_image_sum(pre_img_, back_component, prob_);
 
    return;
  }

  void set_background_model(typename psm_apm_traits<APM>::apm_datatype &background_apm) {
    background_model_ = background_apm;
  }

private:
  int step_count_;

  float obs_range_;

  vpgl_camera<double> const* cam_;
  vil_image_view<typename psm_apm_traits<APM>::obs_datatype> const& obs_;
  vil_image_view<float> &prob_;
  vil_image_view<float> vis_;
  vil_image_view<float> alpha_integral_;

  // scratch images for computation
  vil_image_view<float> pre_img_;
  vil_image_view<float> PI_img_;
  vil_image_view<float> alpha_img_;
  vil_image_view<float> vis_end_;
  vil_image_view<float> pix_weights_;

  safe_normalize_functor<float> safe_norm_functor_;

  // background appearance model
  typename psm_apm_traits<APM>::apm_datatype background_model_;
};



template <psm_apm_type APM>
void psm_pixel_probability_range(psm_scene<APM> &scene, vpgl_camera<double> const* cam, vil_image_view<typename psm_apm_traits<APM>::obs_datatype> const& img, vil_image_view<float> &probabilities, float obs_range, bool black_background = false)
{
  typename psm_apm_traits<APM>::apm_datatype background_apm;
  // for the middlebury datasets, the background is black - update background model accordingly
  if (black_background) {
    //vcl_cout << "using black background model" << vcl_endl;
    for (unsigned int i=0; i<4; ++i) {
      psm_apm_traits<APM>::apm_processor::update(background_apm, 0.0f, 1.0f);
      float peak = psm_apm_traits<APM>::apm_processor::prob_density(background_apm,0.0f);
      //vcl_cout << "p(0) = " << peak <<  vcl_endl;
      //vcl_cout << "sigma = " << vnl_math::two_over_sqrtpi * vnl_math::sqrt1_2 / (2*peak) << vcl_endl;
    }
  }

  psm_parallel_raytrace_function<psm_pixel_probability_functor<APM>, APM> raytrace_fn(scene, cam, img.ni(), img.nj());
  psm_pixel_probability_functor<APM> pix_prob_functor(cam, img, probabilities, obs_range);
  pix_prob_functor.set_background_model(background_apm);

  //vcl_cout << "computing pixel probabilities" << vcl_endl;
  raytrace_fn.run(pix_prob_functor);
  
  return;
}


#endif

