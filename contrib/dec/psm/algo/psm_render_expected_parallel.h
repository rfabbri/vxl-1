#ifndef psm_render_expected_parallel_h_
#define psm_render_expected_parallel_h_

#include <vcl_vector.h>

#include <hsds/hsds_fd_tree.h>
#include <psm/psm_scene.h>
#include <psm/psm_sample.h>
#include <psm/psm_apm_traits.h>
#include <psm/psm_aux_traits.h>
#include <psm/psm_mog_grey_processor.h>
#include <psm/psm_simple_grey_processor.h>

#include <vpgl/vpgl_perspective_camera.h>

#include <vil/vil_image_view.h>
#include <vil/vil_math.h>
#include <vil/vil_transform.h>
#include <vil/vil_save.h>

#include "psm_parallel_raytrace_function.h"

#define USE_AA



template <psm_apm_type APM>
class psm_render_expected_functor
{
public:
  //: default constructor
  psm_render_expected_functor(vpgl_perspective_camera<double> const& cam, vil_image_view<typename psm_apm_traits<APM>::obs_mathtype> &image, vil_image_view<float> &mask) 
    : cam_(cam), expected_(image), vis_(mask), 
    alpha_img_(image.ni(),image.nj(),1), alpha_integral_(image.ni(),image.nj(),1), seg_len_img_(image.ni(),image.nj(),1), vis_end_(image.ni(),image.nj(),1), 
    cell_expected_img_(image.ni(),image.nj(),psm_apm_traits<APM>::obs_dim), pix_weights_(image.ni(),image.nj(),1), cell_front_dist_img_(image.ni(),image.nj(),1), cell_back_dist_img_(image.ni(),image.nj(),1) 
#ifdef USE_AA
    , color_aa_weights_(image.ni(), image.nj(),1)
#endif
  {
    step_count_ = 0;
    vis_.fill(1.0f);
    alpha_integral_.fill(0.0f);
  }

  //: accumulate 
  inline bool step_cells(vgl_point_3d<int> &block_idx, hsds_fd_tree<psm_sample<APM>,3> &block, hsds_fd_tree<psm_aux_traits<PSM_AUX_NULL>::sample_datatype,3> &aux_block, vcl_vector<hsds_fd_tree_node_index<3> > &cells)
  {
    ++step_count_;

    alpha_img_.fill(0.0f);
#ifdef USE_AA
    color_aa_weights_.fill(0.0f);
#else
    cell_front_dist_img_.fill(0.0f);
    cell_back_dist_img_.fill(0.0f);
#endif
    cell_expected_img_.fill((typename psm_apm_traits<APM>::obs_mathtype)0);

    double xverts_2d[8], yverts_2d[8];
    float alpha_vert_distances[8];
    psm_cube_face_list visible_faces;

    // project each cell into the image
    vcl_vector<hsds_fd_tree_node_index<3> >::iterator cell_it = cells.begin();
    for (; cell_it != cells.end(); ++cell_it) {
      psm_sample<APM> const& cell_value = block[*cell_it];
      if (cell_value.alpha > 0.001) {
        // get vertices of cell in the form of a bounding box (cells are always axis-aligned))
        vbl_bounding_box<double,3> cell_bb = block.cell_bounding_box(*cell_it);
        // project vertices to the image determine which faces of the cell are visible
        project_cube_vertices(cell_bb, cam_, xverts_2d, yverts_2d, alpha_vert_distances, visible_faces, cell_value.alpha);
        // get expected color of cell
        typename psm_apm_traits<APM>::obs_datatype cell_expected  = 
          //psm_apm_traits<APM>::apm_processor::most_probable_color(cell_value.appearance);
          psm_apm_traits<APM>::apm_processor::expected_color(cell_value.appearance);

#ifdef USE_AA
        //vcl_cout << "1" << vcl_endl;
        // get ray lengths * alpha
        alpha_seg_len_aa(xverts_2d, yverts_2d, alpha_vert_distances, visible_faces, alpha_img_);       
        // fill expected value image
        //vcl_cout << "2" << vcl_endl;
        cube_fill_value_aa(xverts_2d, yverts_2d, visible_faces, cell_expected_img_, color_aa_weights_, cell_expected);
        //vcl_cout << "3" << vcl_endl;
#else
        // get ray lengths
        cube_camera_distance(cell_bb, cam_, cell_front_dist_img_, cell_back_dist_img_);
        // fill alpha image
        cube_fill_value(cell_bb, cam_, alpha_img_, cell_value.alpha);
        // fill expected value image
        cube_fill_value(cell_bb, cam_, cell_expected_img_, cell_expected);
#endif
      }
    }

#ifdef USE_AA
    // normalize cell_expected_img based on sum of antialiasing weight
    //vcl_cout << "transforming image " << vcl_endl;
     vil_transform2(color_aa_weights_, cell_expected_img_, normalize_cell_expected_func_);
     //vcl_cout <<"done transforming. " << vcl_endl;
#endif

#ifndef USE_AA
    // subtract front and back distances to get length of pixel rays through the cells
    vil_math_image_difference(cell_back_dist_img_,cell_front_dist_img_,seg_len_img_);
    // multiply alpha image by seg length to get integral 
    vil_math_image_product(seg_len_img_,alpha_img_,alpha_img_);
#endif
    // update full alpha_integral image
    vil_math_image_difference(alpha_integral_, alpha_img_, alpha_integral_);
    // compute new vis image
    image_exp_functor exp_fun;
    vil_transform(alpha_integral_,vis_end_,exp_fun);
    // compute weights for each pixel
    vil_math_image_difference(vis_,vis_end_,pix_weights_);
    // scale cell expected image by weighting function..
    vil_math_image_product(pix_weights_,cell_expected_img_,cell_expected_img_);
    // ..and use result to update final expected image
    vil_math_image_sum(cell_expected_img_,expected_,expected_);

#if 0
    // DEBUG
    if (step_count_ == 100) {
      vcl_cout << vcl_endl << "writing debug images " << vcl_endl;
      vil_save(vis_end_,"c:/research/psm/output/vis.tiff");
      vil_save(alpha_img_,"c:/research/psm/output/alpha_img.tiff");
      vil_save(color_aa_weights_,"c:/research/psm/output/pix_weights.tiff");
      vil_save(cell_expected_img_,"c:/research/psm/output/cell_expected.tiff");
    }
#endif
    // update vis_image_
    vis_.deep_copy(vis_end_);

    return true;
  }

  void finish(psm_scene<APM> &scene, psm_aux_scene<PSM_AUX_NULL> &aux_scene){}

private:

  //: functor used for normalizing cell_expected image
  class normalize_cell_expected_functor
  {
  public:
    normalize_cell_expected_functor(float tol = 1e-6) : tol_(tol) {}

    void operator()(float pix_aa_coverage, typename psm_apm_traits<APM>::obs_mathtype &pix) const 
    {
      if (pix_aa_coverage >= tol_)
        pix /= pix_aa_coverage;
      else
        pix = 0;
    }
    float tol_;
  };


  int step_count_;

  vpgl_perspective_camera<double> const& cam_;
  vil_image_view<typename psm_apm_traits<APM>::obs_mathtype> &expected_;
  vil_image_view<float> &vis_;
  vil_image_view<float> alpha_integral_;

  // scratch images for computation
  vil_image_view<typename psm_apm_traits<APM>::obs_mathtype> cell_expected_img_;
  vil_image_view<float> alpha_img_;
  vil_image_view<float> cell_front_dist_img_;
  vil_image_view<float> cell_back_dist_img_;
  vil_image_view<float> seg_len_img_;
  vil_image_view<float> vis_end_;
  vil_image_view<float> pix_weights_;
#ifdef USE_AA
  vil_image_view<float> color_aa_weights_;
  normalize_cell_expected_functor normalize_cell_expected_func_;
#endif
  //: Functor class to compute exponential of image
  class image_exp_functor
  {
  public:
    float operator()(float x)       const { return x<0?vcl_exp(x):1.0f; }
  };


};


template <psm_apm_type APM>
void psm_render_expected(psm_scene<APM> &scene, vpgl_perspective_camera<double> const& cam, vil_image_view<typename psm_apm_traits<APM>::obs_mathtype> &expected, vil_image_view<float> &mask)
{

  psm_parallel_raytrace_function<psm_render_expected_functor<APM>, APM> raytrace_fn(scene, cam, expected.ni(), expected.nj());

  psm_render_expected_functor<APM> functor(cam, expected, mask);
  raytrace_fn.run(functor);

  //: Functor class to normalize expected image
  class normalize_expected_functor
  {
  public:
    normalize_expected_functor(float tol = 1e-4) : tol_(tol) {}

    void operator()(float vis_inf, typename psm_apm_traits<APM>::obs_mathtype &pix) const 
    {
      float nvis = 1-vis_inf;
      if (nvis >= tol_)
        pix /= nvis;
      else
        pix = 0;
    }

    float tol_;
  };

  // normalize image
  normalize_expected_functor norm_fn;
  vil_transform2(mask,expected,norm_fn);

  return;
}



#endif

