#ifndef psm_update_vis_implicit_aa_h_
#define psm_update_vis_implicit_aa_h_

#include <vcl_vector.h>

#include <hsds/hsds_fd_tree.h>
#include <psm/psm_scene.h>
#include <psm/psm_sample.h>
#include <psm/psm_apm_traits.h>
#include <psm/psm_mog_grey_processor.h>
#include <psm/psm_simple_grey_processor.h>

#include <vpgl/vpgl_camera.h>

#include <vil/vil_image_view.h>
#include <vil/vil_math.h>
#include <vil/vil_transform.h>



#include "psm_parallel_raytrace_function.h"


template <psm_apm_type APM>
class psm_update_vis_implicit_aa_functor
{
public:
  //: default constructor
  psm_update_vis_implicit_aa_functor(vpgl_camera<double> const* cam, unsigned int ni, unsigned int nj) 
    : cam_(cam), ni_(ni), nj_(nj),
    alpha_img_(ni,nj,1), alpha_integral_(ni,nj,1), vis_(ni,nj,1) 
  {
    step_count_ = 0;
    vis_.fill(1.0f);
    alpha_integral_.fill(0.0f);
  }

  //: accumulate 
  inline bool step_cells(vgl_point_3d<int> const& block_idx, hsds_fd_tree<psm_sample<APM>,3> &block, hsds_fd_tree<psm_vis_implicit_sample,3> &aux_block, vcl_vector<hsds_fd_tree_node_index<3> > &cells)
  {
    ++step_count_;

    alpha_img_.fill(0.0f);
    
    double xverts_2d[8], yverts_2d[8];
    float vert_distances[8];
    psm_cube_face_list visible_faces;

    // project each cell into the image
    vcl_vector<hsds_fd_tree_node_index<3> >::iterator cell_it = cells.begin();
    for (; cell_it != cells.end(); ++cell_it) {
      psm_sample<APM> const& cell_value = block[*cell_it];
      psm_vis_implicit_sample &vis_implicit_sample = aux_block[*cell_it];
      // get vertices of cell in the form of a bounding box (cells are always axis-aligned))
      vbl_bounding_box<double,3> cell_bb = block.cell_bounding_box(*cell_it);
      // project vertices to the image determine which faces of the cell are visible
      project_cube_vertices(cell_bb, cam_, xverts_2d, yverts_2d, vert_distances, visible_faces);

      // get (weighted) mean vis for cell
      float mean_vis = 1.0f;
      if (cube_mean_aa(xverts_2d, yverts_2d, vert_distances, visible_faces, vis_, mean_vis)) {
        if (!((mean_vis >= -0.01f) && (mean_vis <= 1.01f)) ) {
          vcl_cerr  << "error: mean_vis = " << mean_vis << vcl_endl;
          continue;
        }
        if (mean_vis > 1.0f) {
          mean_vis = 1.0f;
        }
        if (mean_vis < 0.0f) {
          mean_vis = 0.0f;
        }
        if (!((vis_implicit_sample.max_vis >= 0.0f) && (vis_implicit_sample.max_vis <= 1.01f)) ) {
          vcl_cerr << "error: cell.max_vis = " << vis_implicit_sample.max_vis << vcl_endl;
          continue;
        }
        // update max vis for cell in aux_sample
        if (mean_vis > vis_implicit_sample.max_vis) {
          vis_implicit_sample.max_vis = mean_vis;
        }
        ++vis_implicit_sample.n_obs;
      }
      // get ray lengths * alpha
      alpha_seg_len_aa(xverts_2d, yverts_2d, vert_distances, visible_faces, cell_value.alpha, alpha_img_);       
    }

    // update full alpha_integral image
    vil_math_image_difference(alpha_integral_, alpha_img_, alpha_integral_);
    // compute new vis image
    image_exp_functor exp_fun;
    vil_transform(alpha_integral_,vis_,exp_fun);

    return true;

  }

  void finish(psm_scene<APM> &scene, psm_aux_scene<PSM_AUX_VIS_IMPLICIT> &aux_scene){}

private:
  int step_count_;
  unsigned int ni_, nj_;

  vpgl_camera<double> const* cam_;
  vil_image_view<float> alpha_integral_;

  // scratch images for computation
  vil_image_view<float> vis_;
  vil_image_view<float> alpha_img_;


};

template <psm_apm_type APM>
void psm_update_vis_implicit(psm_scene<APM> &scene, psm_aux_scene_base_sptr vis_implicit_aux_scene, vpgl_camera<double> const* cam, unsigned int ni, unsigned int nj)
{

  psm_parallel_raytrace_function<psm_update_vis_implicit_aa_functor<APM>, APM, PSM_AUX_VIS_IMPLICIT> raytrace_fn(scene, vis_implicit_aux_scene, cam, ni, nj);

  psm_update_vis_implicit_aa_functor<APM> functor(cam, ni, nj);
  raytrace_fn.run(functor);

  return;
}



#endif

