#ifndef psm_update_vis_implicit_h_
#define psm_update_vis_implicit_h_

#include <vcl_vector.h>

#include <hsds/hsds_fd_tree.h>
#include <psm/psm_scene.h>
#include <psm/psm_sample.h>
#include <psm/psm_apm_traits.h>
#include <psm/psm_mog_grey_processor.h>
#include <psm/psm_simple_grey_processor.h>

#include <vpgl/vpgl_perspective_camera.h>

#include <vil/vil_image_view.h>
#include <vil/vil_math.h>
#include <vil/vil_transform.h>



#include "psm_parallel_raytrace_function.h"



template <psm_apm_type APM>
void psm_update_vis_implicit(psm_scene<APM> &scene, psm_aux_scene_base_sptr vis_implicit_aux_scene, vpgl_perspective_camera<double> const& cam, unsigned int ni, unsigned int nj)
{

  psm_parallel_raytrace_function<psm_update_vis_implicit_functor<APM>, APM, PSM_AUX_VIS_IMPLICIT> raytrace_fn(scene, vis_implicit_aux_scene, cam, ni, nj);

  psm_update_vis_implicit_functor<APM> functor(cam, ni, nj);
  raytrace_fn.run(functor);

  return;
}

template <psm_apm_type APM>
class psm_update_vis_implicit_functor
{
public:
  //: default constructor
  psm_update_vis_implicit_functor(vpgl_perspective_camera<double> const& cam, unsigned int ni, unsigned int nj) 
    : cam_(cam), ni_(ni), nj_(nj),
    alpha_img_(ni,nj,1), alpha_integral_(ni,nj,1), seg_len_img_(ni,nj,1),  
    cell_front_dist_img_(ni,nj,1), cell_back_dist_img_(ni,nj,1), vis_(ni,nj,1) 
  {
    step_count_ = 0;
    vis_.fill(1.0f);
    alpha_integral_.fill(0.0f);
  }

  //: accumulate 
   inline bool step_cells(vgl_point_3d<int> const& block, hsds_fd_tree<psm_sample<APM>,3> &block, hsds_fd_tree<psm_vis_implicit_sample,3> &aux_block, vcl_vector<hsds_fd_tree_node_index<3> > &cells)
 {
    ++step_count_;
    cell_front_dist_img_.fill(0.0f);
    cell_back_dist_img_.fill(0.0f);
    alpha_img_.fill(0.0f);

    // project each cell into the image
    vcl_vector<hsds_fd_tree_node_index<3> >::iterator cell_it = cells.begin();
    for (; cell_it != cells.end(); ++cell_it) {
      psm_sample<APM> const& cell_value = block[*cell_it];
      vbl_bounding_box<double,3> cell_bb = block.cell_bounding_box(*cell_it);
      // get ray lengths
      cube_camera_distance(cell_bb, cam_, cell_front_dist_img_, cell_back_dist_img_);
      // fill alpha image
      cube_fill_value(cell_bb, cam_, alpha_img_, cell_value.alpha);
    }

    // subtract front and back distances to get length of pixel rays through the cells
    vil_math_image_difference(cell_back_dist_img_,cell_front_dist_img_,seg_len_img_);

    for (cell_it = cells.begin(); cell_it != cells.end(); ++cell_it) {
      vbl_bounding_box<double,3> cell_bb = block.cell_bounding_box(*cell_it);
      psm_vis_implicit_sample &vis_implicit_sample = aux_block[*cell_it];
      // get (weighted) mean vis for cell
      float mean_vis = 1.0f;
      if (cube_weighted_mean(cell_bb, cam_, vis_, seg_len_img_, mean_vis)) {
        if (!((mean_vis >= 0.0f) && (mean_vis <= 1.0f)) ) {
          vcl_cerr << vcl_cout << "error: mean_vis = " << mean_vis << vcl_endl;
          continue;
        }
        if (!((vis_implicit_sample.max_vis >= 0.0f) && (vis_implicit_sample.max_vis <= 1.0f)) ) {
          vcl_cerr << vcl_cout << "error: cell.max_vis = " << vis_implicit_sample.max_vis << vcl_endl;
          continue;
        }
        // update max vis for cell in aux_sample
        if (mean_vis > vis_implicit_sample.max_vis) {
          vis_implicit_sample.max_vis = mean_vis;
        }
        ++vis_implicit_sample.n_obs;
      }
    }
    // multiply alpha image by seg length to get integral 
    vil_math_image_product(seg_len_img_,alpha_img_,alpha_img_);
    // update full alpha_integral image
    vil_math_image_difference(alpha_integral_, alpha_img_, alpha_integral_);
    // update new vis image
    image_exp_functor exp_fun;
    vil_transform(alpha_integral_,vis_,exp_fun);


    return true;
  }

   void finish(psm_scene<APM> &scene, psm_aux_scene<PSM_AUX_VIS_IMPLICIT> &aux_scene){}

private:
  int step_count_;
  unsigned int ni_, nj_;

  vpgl_perspective_camera<double> const& cam_;
  vil_image_view<float> alpha_integral_;

  // scratch images for computation
  vil_image_view<float> vis_;
  vil_image_view<float> alpha_img_;
  vil_image_view<float> cell_front_dist_img_;
  vil_image_view<float> cell_back_dist_img_;
  vil_image_view<float> seg_len_img_;

  //: Functor class to compute exponential of image
  class image_exp_functor
  {
  public:
    float operator()(float x)       const { return x<0?vcl_exp(x):1.0f; }
  };


};





#endif

