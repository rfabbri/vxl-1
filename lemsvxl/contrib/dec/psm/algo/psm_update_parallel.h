#ifndef psm_update_parallel_h_
#define psm_update_parallel_h_

#include <vcl_vector.h>

#include <hsds/hsds_fd_tree.h>
#include <psm/psm_scene.h>
#include <psm/psm_sample.h>
#include <psm/psm_apm_traits.h>
#include <psm/psm_aux_traits.h>
#include <psm/psm_update_sample.h>
#include <psm/psm_mog_grey_processor.h>
#include <psm/psm_simple_grey_processor.h>

#include <vpgl/vpgl_perspective_camera.h>

#include <vil/vil_image_view.h>
#include <vil/vil_math.h>
#include <vil/vil_transform.h>

#include "psm_parallel_raytrace_function.h"


template <psm_apm_type APM>
void psm_update(psm_scene<APM> &scene, vpgl_perspective_camera<double> const& cam, vil_image_view<typename psm_apm_traits<APM>::obs_mathtype> &img, bool black_background = false)
{
  // create a temporary aux_scene
  scene.init_aux_scene<PSM_AUX_UPDATE>();
  psm_aux_scene_base_sptr aux_scene_ptr = scene.get_aux_scene<PSM_AUX_UPDATE>();

  psm_parallel_raytrace_function<psm_update_functor<APM>, APM, PSM_AUX_UPDATE> raytrace_fn(scene, aux_scene_ptr, cam, img.ni(), img.nj());
  psm_update_functor<APM> up_functor(cam, img);
  // for the middlebury datasets, the background is black - update background model accordingly
  if (black_background) {
    vcl_cout << "using black background model" << vcl_endl;
    typename psm_apm_traits<APM>::apm_datatype background_apm;
    for (unsigned int i=0; i<4; ++i) {
      psm_apm_traits<APM>::apm_processor::update(background_apm, 0.0f, 1.0f);
      float peak = psm_apm_traits<APM>::apm_processor::prob_density(background_apm,0.0f);
      //vcl_cout << "p(0) = " << peak <<  vcl_endl;
      //vcl_cout << "sigma = " << vnl_math::two_over_sqrtpi * vnl_math::sqrt1_2 / (2*peak) << vcl_endl;
    }
    up_functor.set_background_model(background_apm);
  }
  raytrace_fn.run(up_functor);

  return;
}


template <psm_apm_type APM>
class psm_update_functor
{
public:
  //: default constructor
  psm_update_functor(vpgl_perspective_camera<double> const& cam, vil_image_view<typename psm_apm_traits<APM>::obs_mathtype> &image) 
    : cam_(cam), obs_(image), max_cell_P_(0.99f), min_cell_P_(0.001f),
    PI_img_(image.ni(),image.nj(),1), alpha_img_(image.ni(),image.nj(),1), alpha_integral_(image.ni(),image.nj(),1), vis_(image.ni(), image.nj(),1), vis_end_(image.ni(),image.nj(),1), 
    pix_weights_(image.ni(),image.nj(),1), pre_img_(image.ni(),image.nj(),1), update_numerator_(image.ni(), image.nj(), 1)
  {
    step_count_ = 0;
    vis_.fill(1.0f);
    pre_img_.fill(0.0f);
    alpha_integral_.fill(0.0f);
  }

  //: accumulate 
  inline bool step_cells(vgl_point_3d<int> const& block_idx, hsds_fd_tree<psm_sample<APM>,3> &block, hsds_fd_tree<psm_aux_traits<PSM_AUX_UPDATE>::sample_datatype,3> &aux_block, vcl_vector<hsds_fd_tree_node_index<3> > &cells)
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
      //if (cell_it->idx == 0x32f20000) {
      //  vcl_cout << "debug_break" << vcl_endl;
      //}
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
        float cell_PI = psm_apm_traits<APM>::apm_processor::prob_density(cell_value.appearance, cell_mean_obs);
        if (!((cell_PI >= 0) && (cell_PI < 1e8)) ) {
          vcl_cout << vcl_endl << "cell_PI = " << cell_PI << vcl_endl;
          vcl_cout << "  cell_obs = " << cell_mean_obs << vcl_endl;
          vcl_cout << "  cell id = " << *cell_it << vcl_endl; 
        }
        // fill obs probability density image
        cube_fill_value(xverts_2d, yverts_2d, visible_faces, PI_img_, cell_PI);
      }

      // find mean cell visibility for update
      float cell_mean_vis = 0.0f;
      if (cube_mean(xverts_2d, yverts_2d, vert_dists, visible_faces, vis_, cell_mean_vis)) {
        // update appearance model
        if (cell_mean_vis > 1e-6) {
          psm_apm_traits<APM>::apm_processor::update(cell_value.appearance, cell_mean_obs, cell_mean_vis);
        }
      }
    }
    // update full alpha_integral image
    vil_math_image_difference(alpha_integral_, alpha_img_, alpha_integral_);
    // compute new vis image
    image_exp_functor exp_fun;
    vil_transform(alpha_integral_,vis_end_,exp_fun);
    // compute weights for each pixel
    vil_math_image_difference(vis_,vis_end_,pix_weights_);

    // multiply PI by vis..
    vil_math_image_product(PI_img_, vis_, update_numerator_);
    // ..and add preX to get numerator term for update
    vil_math_image_sum(pre_img_, update_numerator_, update_numerator_);

    for (cell_it = cells.begin(); cell_it != cells.end(); ++cell_it) {
      psm_sample<APM> &cell_value = block[*cell_it];
      // get vertices of cell in the form of a bounding box (cells are always axis-aligned))
      vbl_bounding_box<double,3> cell_bb = block.cell_bounding_box(*cell_it);
      // project vertices to the image determine which faces of the cell are visible
      project_cube_vertices(cell_bb, cam_, xverts_2d, yverts_2d, vert_dists, visible_faces);
      // determine mean value of update numerator term
      float mean_numerator = 0.0f;
      if (cube_mean(xverts_2d, yverts_2d, vert_dists, visible_faces, update_numerator_, mean_numerator)) {
        // store in auxillary block
        psm_update_sample &aux_cell = aux_block[*cell_it];
        aux_cell.update_multiplier = mean_numerator;
        aux_cell.do_update = true;  
      }
    }
    // multiply cell_PI by cell weights
    vil_math_image_product(pix_weights_, PI_img_, PI_img_);
    // update pre_img 
    vil_math_image_sum(PI_img_, pre_img_, pre_img_);

//#define PSM_DEBUG
#ifdef PSM_DEBUG
    if (step_count_ == 150) {
      vcl_cout << "saving debug images" << vcl_endl;
      vcl_string output_dir = "c:/research/psm/output/";
      vil_save(alpha_img_,(output_dir + "alpha_img.tiff").c_str());
      vil_save(alpha_integral_,(output_dir + "alpha_integral.tiff").c_str());
      vil_save(pix_weights_,(output_dir + "pix_weights.tiff").c_str());
      vil_save(pre_img_,(output_dir + "pre_img.tiff").c_str());
      vil_save(PI_img_,(output_dir + "PI_img.tiff").c_str());
      vil_save(vis_,(output_dir + "vis.tiff").c_str());
      vil_save(vis_end_,(output_dir + "vis_end.tiff").c_str());
      vil_save(update_numerator_,(output_dir + "update_num.tiff").c_str());
      vil_save(obs_,(output_dir + "obs.tiff").c_str());
    }
#endif

    // update vis_image_
    vis_.deep_copy(vis_end_);

    return true;
  }

  void finish(psm_scene<APM> &scene, psm_aux_scene<PSM_AUX_UPDATE> &aux_scene)
  {
    double xverts_2d[8], yverts_2d[8];
    float vert_dists[8];
    psm_cube_face_list visible_faces;

    vil_image_view<float> norm_img(obs_.ni(), obs_.nj(), 1);
    normalization_image(norm_img);

    //vil_save(norm_img, "c:/research/psm/output/norm_img.tiff");

    // loop through all blocks
    vcl_set<vgl_point_3d<int>, vgl_point_3d_cmp<int> > valid_blocks = scene.valid_blocks();
    vcl_set<vgl_point_3d<int>, vgl_point_3d_cmp<int> >::iterator vbit = valid_blocks.begin();
    for (; vbit != valid_blocks.end(); ++vbit) {

      hsds_fd_tree<psm_sample<APM>,3> &block = scene.get_block(*vbit);
      hsds_fd_tree<psm_update_sample,3> &aux_block = aux_scene.get_block(*vbit);

      hsds_fd_tree<psm_sample<APM>,3>::iterator block_it = block.begin();
      hsds_fd_tree<psm_update_sample,3>::iterator aux_block_it = aux_block.begin();
      for (; block_it != block.end(); ++block_it, ++aux_block_it) {
        psm_update_sample &aux_cell = aux_block_it->second;
        psm_sample<APM> &cell = block_it->second;
        if (aux_cell.do_update) {
          // determine mean value of normalization image
          vbl_bounding_box<double,3> cell_bb = block.cell_bounding_box(block_it->first);
          // project vertices to the image determine which faces of the cell are visible
          project_cube_vertices(cell_bb, cam_, xverts_2d, yverts_2d, vert_dists, visible_faces);
          float mean_norm = 0.0f;
          if (cube_mean(xverts_2d, yverts_2d, vert_dists, visible_faces, norm_img, mean_norm)) {
            cell.alpha *= aux_cell.update_multiplier * mean_norm;
            float cell_len = float(cell_bb.xmax() - cell_bb.xmin());
            float max_alpha = -vcl_log(1.0f - max_cell_P_)/cell_len;
            float min_alpha = -vcl_log(1.0f - min_cell_P_)/cell_len;
            if (cell.alpha > max_alpha) {
              cell.alpha = max_alpha;
            }
            if (cell.alpha < min_alpha) {
              cell.alpha = min_alpha;
            }
            if (!((cell.alpha >= min_alpha) && (cell.alpha <= max_alpha)) ){
              vcl_cerr << vcl_endl << "error: cell.alpha = " << cell.alpha << vcl_endl;
              vcl_cerr << "mean_norm = " << mean_norm << vcl_endl;
            }
          }
        }
      }
    }
    return;
  }

  //: return the computed normalization image for use in pass 2
  void normalization_image(vil_image_view<float> &norm_img) 
  {
    // compute probability of background producing observed intensities
    vil_image_view<float> PI_background(norm_img.ni(),norm_img.nj(),1);
    for (unsigned int j=0; j<obs_.nj(); ++j) {
      for (unsigned int i=0; i<obs_.ni(); ++i) {
        // this will have to be modified slightly when we deal with multi-plane images -dec
        PI_background(i,j) = psm_apm_traits<APM>::apm_processor::prob_density(background_model_, obs_(i,j));
      }
    }
    //vil_save(PI_background,"c:/research/psm/output/PI_background.tiff");
    vil_math_image_product(PI_background, vis_, norm_img);
    vil_math_image_sum(pre_img_,norm_img,norm_img);
    safe_inverse_functor inv_func(1e-8f);
    vil_transform(norm_img, inv_func);
    return;
  }

  void set_background_model(typename psm_apm_traits<APM>::apm_datatype &background_apm) {
    background_model_ = background_apm;
  }

private:
  int step_count_;
  const float min_cell_P_;
  const float max_cell_P_;

  vpgl_perspective_camera<double> const& cam_;
  vil_image_view<typename psm_apm_traits<APM>::obs_mathtype> &obs_;
  vil_image_view<float> vis_;
  vil_image_view<float> alpha_integral_;

  // scratch images for computation
  vil_image_view<float> pre_img_;
  vil_image_view<float> PI_img_;
  vil_image_view<float> alpha_img_;
  vil_image_view<float> vis_end_;
  vil_image_view<float> pix_weights_;
  vil_image_view<float> update_numerator_;

  safe_normalize_functor<float> safe_norm_functor_;

  // background appearance model
  typename psm_apm_traits<APM>::apm_datatype background_model_;

  //: Functor class to compute exponential of image
  class image_exp_functor
  {
  public:
    float operator()(float x)       const { return x<0?vcl_exp(x):1.0f; }
  };

  class safe_inverse_functor
  {
  public:
    safe_inverse_functor(float tol = 0.0f) : tol_(tol) {}

    float operator()(float x)      const { return (x > tol_)? 1.0f/x : 0.0f; }

    float tol_;
  };


};




#endif

