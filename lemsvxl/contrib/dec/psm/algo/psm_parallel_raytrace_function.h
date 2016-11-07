#ifndef psm_parallel_raytrace_function_h_
#define psm_parallel_raytrace_function_h_

#include <vcl_cassert.h>

#include <psm/psm_apm_traits.h>
#include <psm/psm_aux_traits.h>
#include <psm/psm_scene.h>
#include <psm/psm_aux_scene.h>

#include <vpgl/vpgl_camera.h>


#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_homg_point_3d.h>
#include <vgl/vgl_homg_line_3d_2_points.h>
#include <vgl/vgl_line_3d_2_points.h>

#include "psm_raytrace_operations.h"
#include "psm_cell_visibility_iterator.h"
#include "psm_block_visibility_iterator.h"


template<class F, psm_apm_type APM, psm_aux_type AUX_T = PSM_AUX_NULL>
class psm_parallel_raytrace_function
{
public:
  //: constructor
  psm_parallel_raytrace_function(psm_scene<APM> &scene, vpgl_camera<double> const* cam, unsigned int ni, unsigned int nj, bool full_vis_needed = false, bool reverse_traversal = false)
    :scene_(scene), aux_scene_ptr_(new psm_aux_scene<AUX_T>()), cam_(cam), img_ni_(ni), img_nj_(nj), use_aux_(false), debug_lvl_(0), full_vis_needed_(full_vis_needed), reverse_traversal_(reverse_traversal) {}

  //: constructor taking an auxilary scene
  psm_parallel_raytrace_function(psm_scene<APM> &scene, psm_aux_scene_base_sptr aux_scene, vpgl_camera<double> const* cam, unsigned int ni, unsigned int nj, bool full_vis_needed = false, bool reverse_traversal = false)
    :scene_(scene), aux_scene_ptr_(aux_scene), cam_(cam), img_ni_(ni), img_nj_(nj), use_aux_(true), debug_lvl_(0), full_vis_needed_(full_vis_needed), reverse_traversal_(reverse_traversal) {}

  //: run the function
  bool run(F& step_functor) 
  {
    const int max_blocks = 9;

    // find block containing camera center
    //vgl_point_3d<int> cam_block_idx = scene_.block_index(cam_center);
    psm_block_visibility_iterator<APM> block_vis_it(scene_,cam_, 0, 0, img_ni_, img_nj_, reverse_traversal_);

    int block_count = 0;
    bool continue_trace = true;

    psm_aux_scene<AUX_T> *aux_scene = dynamic_cast<psm_aux_scene<AUX_T>*>(aux_scene_ptr_.ptr());
    if (!aux_scene) {
      vcl_cerr << "error: psm_parallel_raytrace_function: failed to cast aux_scene to correct type." << vcl_endl;
      return false;
    }

    while(continue_trace && block_vis_it.next())
    {
      if (++block_count > max_blocks) {
        continue_trace = false;
        break;
      }
      vcl_vector<vgl_point_3d<int> > vis_blocks;
      block_vis_it.current_blocks(vis_blocks);
      // traverse each visible block one at a time
      vcl_vector<vgl_point_3d<int> >::iterator block_it = vis_blocks.begin();
      for (; block_it != vis_blocks.end(); ++block_it) {

        if (debug_lvl_ > 0) vcl_cout << "processing block at index (" << block_it->x() << ", " << block_it->y() << ", " << block_it->z() << ")" << vcl_endl;
        // make sure block projects to inside of image
        vbl_bounding_box<double,3> block_bb = scene_.block_bounding_box(*block_it);
        if (!cube_visible(block_bb, cam_, 0,0, img_ni_, img_nj_, false)) {
          continue;
        }

        hsds_fd_tree<psm_sample<APM>,3> &block = scene_.get_block(*block_it);
        hsds_fd_tree<typename psm_aux_traits<AUX_T>::sample_datatype,3> &aux_block = aux_scene->get_block(*block_it);

        if (debug_lvl_ > 0) vcl_cout << "building visibility graph for block..";
        psm_cell_visibility_iterator<APM> cell_vis_it(block, cam_, img_ni_, img_nj_, full_vis_needed_, reverse_traversal_);
        if (debug_lvl_ > 0) vcl_cout << "..done" << vcl_endl;

        while(cell_vis_it.next()) {
          vcl_vector<hsds_fd_tree_node_index<3> > to_process_indices;
          cell_vis_it.current_cells(to_process_indices);
          // do the processing 
          if (debug_lvl_ > 0) vcl_cout << "+";
            continue_trace = step_functor.step_cells(*block_it,block,aux_block,to_process_indices);

          if (!continue_trace) {
            break;
          }
          if (debug_lvl_ > 0) vcl_cout << "-";
        }
        if (debug_lvl_ > 0) vcl_cout << "done" << vcl_endl;
        if (!continue_trace) {
          // stop raytrace. no need to search for next block.
          break;
        }
      }
    }
    if (debug_lvl_ > 0) vcl_cout << "done with all blocks. " << vcl_endl;

    step_functor.finish(scene_, *aux_scene);

    return true;
  }

protected:

  unsigned int debug_lvl_;

  psm_scene<APM> &scene_;
  psm_aux_scene_base_sptr aux_scene_ptr_;
  bool use_aux_;
  vpgl_camera<double> const* cam_;
  unsigned int img_ni_;
  unsigned int img_nj_;

  bool full_vis_needed_;
  bool reverse_traversal_;

};

#endif
