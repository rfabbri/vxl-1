#ifndef psm_render_expected_aa_h_
#define psm_render_expected_aa_h_

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
#include <vil/vil_save.h>
#include <vil/vil_view_as.h>

#include "psm_parallel_raytrace_function.h"




template <psm_apm_type APM>
class psm_render_expected_functor
{
public:
  //: default constructor
  psm_render_expected_functor(vpgl_camera<double> const* cam, vil_image_view<typename psm_apm_traits<APM>::obs_datatype> &image, vil_image_view<float> &mask) 
    : cam_(cam), expected_(image), vis_(mask), 
    alpha_img_(image.ni(),image.nj(),1), alpha_integral_(image.ni(),image.nj(),1), vis_end_(image.ni(),image.nj(),1), 
    cell_expected_img_(image.ni(),image.nj(),1), pix_weights_(image.ni(),image.nj(),1), color_aa_weights_(image.ni(), image.nj(),1)

  {
    step_count_ = 0;
    expected_.fill(0.0f);
    vis_.fill(1.0f);
    alpha_integral_.fill(0.0f);
  }

  //: accumulate 
  inline bool step_cells(vgl_point_3d<int> &block_idx, hsds_fd_tree<psm_sample<APM>,3> &block, hsds_fd_tree<psm_aux_traits<PSM_AUX_NULL>::sample_datatype,3> &aux_block, vcl_vector<hsds_fd_tree_node_index<3> > &cells)
  {
    ++step_count_;

    alpha_img_.fill(0.0f);
    color_aa_weights_.fill(0.0f);
    cell_expected_img_.fill((typename psm_apm_traits<APM>::obs_datatype)0);

    double xverts_2d[8], yverts_2d[8];
    float vert_distances[8];
    psm_cube_face_list visible_faces;

    // project each cell into the image
    vcl_vector<hsds_fd_tree_node_index<3> >::iterator cell_it = cells.begin();
    for (; cell_it != cells.end(); ++cell_it) {
      psm_sample<APM> const& cell_value = block[*cell_it];
      if (cell_value.alpha > 0.001) {
        // get vertices of cell in the form of a bounding box (cells are always axis-aligned))
        vbl_bounding_box<double,3> cell_bb = block.cell_bounding_box(*cell_it);
        // project vertices to the image determine which faces of the cell are visible
        project_cube_vertices(cell_bb, cam_, xverts_2d, yverts_2d, vert_distances, visible_faces);
        // get expected color of cell
        typename psm_apm_traits<APM>::obs_datatype cell_expected  = 
          //psm_apm_traits<APM>::apm_processor::most_probable_color(cell_value.appearance);
          psm_apm_traits<APM>::apm_processor::expected_color(cell_value.appearance);
        // get ray lengths * alpha
        alpha_seg_len_aa(xverts_2d, yverts_2d, vert_distances, visible_faces, cell_value.alpha, alpha_img_);       
        // fill expected value image
        cube_fill_value_aa(xverts_2d, yverts_2d, visible_faces, cell_expected_img_, color_aa_weights_, cell_expected);
      }
    }
    // normalize cell_expected_img based on sum of antialiasing weight
    vil_transform2(color_aa_weights_, cell_expected_img_, normalize_cell_expected_func_);

     /*
     if (step_count_ == 15) {
       vcl_cout << vcl_endl << "saving debug images" << vcl_endl;
       vil_save(vis_,"c:/research/psm/output/vis.tiff");
       vil_save(color_aa_weights_,"c:/research/psm/output/pix_weights.tiff");
       vil_save(cell_expected_img_,"c:/research/psm/output/cell_expected.tiff");
       vil_save(alpha_img_,"c:/research/psm/output/alpha_img.tiff");
     }
     */

    // update full alpha_integral image
    vil_math_image_difference(alpha_integral_, alpha_img_, alpha_integral_);
    // compute new vis image
    image_exp_functor exp_fun;
    vil_transform(alpha_integral_,vis_end_,exp_fun);
    // compute weights for each pixel
    vil_math_image_difference(vis_,vis_end_,pix_weights_);
    // scale cell expected image by weighting function..
    //vil_image_view<psm_apm_traits<APM>::obs_mathtype> cell_expected_planes = vil_view_as_planes(cell_expected_img_);
    vil_math_image_product( cell_expected_img_,pix_weights_, cell_expected_img_);
    // ..and use result to update final expected image
    vil_math_image_sum(cell_expected_img_,expected_,expected_);

    // update vis_image_
    vis_.deep_copy(vis_end_);

    return true;
  }

  void finish(psm_scene<APM> &scene, psm_aux_scene<PSM_AUX_NULL> &aux_scene){}
 

private:

  int step_count_;

  vpgl_camera<double> const* cam_;
  vil_image_view<typename psm_apm_traits<APM>::obs_datatype> &expected_;
  vil_image_view<float> &vis_;
  vil_image_view<float> alpha_integral_;

  // scratch images for computation
  vil_image_view<typename psm_apm_traits<APM>::obs_datatype> cell_expected_img_;
  vil_image_view<float> alpha_img_;
  vil_image_view<float> vis_end_;
  vil_image_view<float> pix_weights_;

  vil_image_view<float> color_aa_weights_;
  safe_normalize_functor<typename psm_apm_traits<APM>::obs_datatype> normalize_cell_expected_func_;

  //: Functor class to compute exponential of image
  class image_exp_functor
  {
  public:
    float operator()(float x)       const { return x<0?vcl_exp(x):1.0f; }
  };

};


//: Functor class to normalize expected image
template<psm_apm_type APM>
class normalize_expected_functor
{
public:
  normalize_expected_functor(bool use_black_background) : use_black_background_(use_black_background) {}

  void operator()(float mask, typename psm_apm_traits<APM>::obs_datatype &pix) const 
  {
    if (!use_black_background_) {
      pix += mask*0.5f;
    }
  }
  bool use_black_background_;
};


template <psm_apm_type APM>
void psm_render_expected(psm_scene<APM> &scene, vpgl_camera<double> const* cam, vil_image_view<typename psm_apm_traits<APM>::obs_datatype> &expected, vil_image_view<float> &mask, bool use_black_background = false)
{
  vcl_cout << "render expected: use_black_background = " << use_black_background << vcl_endl;
  
  psm_parallel_raytrace_function<psm_render_expected_functor<APM>, APM> raytrace_fn(scene, cam, expected.ni(), expected.nj(), false);

  psm_render_expected_functor<APM> functor(cam, expected, mask);
  raytrace_fn.run(functor);

  // normalize image
  normalize_expected_functor<APM> norm_fn(use_black_background);
  vil_transform2<float,typename psm_apm_traits<APM>::obs_datatype, normalize_expected_functor<APM> >(mask,expected,norm_fn);

  return;
}


#endif

