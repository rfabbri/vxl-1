#ifndef psm_block_visibility_iterator_txx_
#define psm_block_visibility_iterator_txx_

#include <vcl_vector.h>
#include <vcl_set.h>
#include <vcl_utility.h>

#include <vbl/vbl_bounding_box.h>
#include <vgl/vgl_point_3d.h>
#include <vpgl/vpgl_perspective_camera.h>

#include <psm/psm_scene.h>
#include "psm_raytrace_operations.h"
#include "psm_block_visibility_iterator.h"

template<psm_apm_type APM>
psm_block_visibility_iterator<APM>::psm_block_visibility_iterator(psm_scene<APM> &scene, vpgl_camera<double> const* cam, unsigned int i0, unsigned int j0, unsigned int img_ni, unsigned int img_nj, bool reverse_iterator)
: scene_(scene), cam_(cam), img_ni_(img_ni), img_nj_(img_nj), reverse_it_(reverse_iterator)
{
  //vgl_point_3d<double> cam_center(cam_.camera_center());

  vcl_set<vgl_point_3d<int>, vgl_point_3d_cmp<int> > valid_blocks = scene_.valid_blocks();

  // initalize visibility graph
  vcl_set<vgl_point_3d<int>, vgl_point_3d_cmp<int> >::const_iterator bit = valid_blocks.begin();
  for (; bit != valid_blocks.end(); ++bit) {
    if (cube_visible(scene.block_bounding_box(*bit),cam, i0, j0, img_ni,img_nj,true)) {
      vis_graph_.insert(vcl_make_pair<vgl_point_3d<int>,psm_block_vis_graph_node>(*bit,psm_block_vis_graph_node()));
    }
  }

  // add visibility edges to each node. add unoccluded blocks to curr_blocks_
  psm_block_vis_graph_type::iterator vit = vis_graph_.begin();
  for (; vit != vis_graph_.end(); ++vit) {
    // find visible faces of block
    vbl_bounding_box<double,3> block_bb = scene_.block_bounding_box(vit->first);
    //psm_cube_face_list vis_faces = visible_faces(block_bb,cam_center);
    psm_cube_face_list vis_faces = visible_faces(block_bb, cam);

    // add edge to other neighboring block for each visible face
    if (vis_faces & psm_cube_face::X_HIGH) {
      // check for neighbor on X_HIGH face
      vgl_point_3d<int> neighbor_idx = vit->first;
      if (reverse_it_) {
        neighbor_idx += vgl_vector_3d<int>(-1,0,0);
      }else {
        neighbor_idx += vgl_vector_3d<int>(1,0,0);
      }
      psm_block_vis_graph_type::iterator nit = vis_graph_.find(neighbor_idx);
      if (nit == vis_graph_.end()) {
        vit->second.incoming_count++;
        curr_blocks_.push_back(vit);
      } else {
        vit->second.incoming_count++;
        nit->second.outgoing_links.push_back(vit);
      }
    } else if (vis_faces & psm_cube_face::X_LOW) {
      // check for neighbor on X_LOW face
      vgl_point_3d<int> neighbor_idx = vit->first;
      if (reverse_it_) {
        neighbor_idx += vgl_vector_3d<int>(1,0,0);
      } else {
        neighbor_idx += vgl_vector_3d<int>(-1,0,0);
      }
      psm_block_vis_graph_type::iterator nit = vis_graph_.find(neighbor_idx);
      if (nit == vis_graph_.end()) {
        vit->second.incoming_count++;
        curr_blocks_.push_back(vit);
      } else {
        vit->second.incoming_count++;
        nit->second.outgoing_links.push_back(vit);
      }
    }
    if (vis_faces & psm_cube_face::Y_HIGH) {
      // check for neighbor on Y_HIGH face
      vgl_point_3d<int> neighbor_idx = vit->first;
      if (reverse_it_) {
        neighbor_idx += vgl_vector_3d<int>(0,-1,0);
      }
      else {
        neighbor_idx += vgl_vector_3d<int>(0,1,0);
      }
      psm_block_vis_graph_type::iterator nit = vis_graph_.find(neighbor_idx);
      if (nit == vis_graph_.end()) {
        vit->second.incoming_count++;
        curr_blocks_.push_back(vit);
      } else {
        vit->second.incoming_count++;
        nit->second.outgoing_links.push_back(vit);
      }

    }else if (vis_faces & psm_cube_face::Y_LOW) {
      // check for neighbor on Y_LOW face
      vgl_point_3d<int> neighbor_idx = vit->first;
      if (reverse_it_) {
        neighbor_idx += vgl_vector_3d<int>(0,1,0);
      }else {
        neighbor_idx += vgl_vector_3d<int>(0,-1,0);
      }
      psm_block_vis_graph_type::iterator nit = vis_graph_.find(neighbor_idx);
      if (nit == vis_graph_.end()) {
        vit->second.incoming_count++;
        curr_blocks_.push_back(vit);
      } else {
        vit->second.incoming_count++;
        nit->second.outgoing_links.push_back(vit);
      }

    }
    if (vis_faces & psm_cube_face::Z_HIGH) {
      // check for neighbor on Z_HIGH face
      vgl_point_3d<int> neighbor_idx = vit->first;
      if (reverse_it_) {
        neighbor_idx += vgl_vector_3d<int>(0,0,-1);
      }else {
        neighbor_idx += vgl_vector_3d<int>(0,0,1);
      }
      psm_block_vis_graph_type::iterator nit = vis_graph_.find(neighbor_idx);
      if (nit == vis_graph_.end()) {
        vit->second.incoming_count++;
        curr_blocks_.push_back(vit);
      } else {
        vit->second.incoming_count++;
        nit->second.outgoing_links.push_back(vit);
      }

    }else if (vis_faces & psm_cube_face::Z_LOW) {
      // check for neighbor on Z_LOW face
      vgl_point_3d<int> neighbor_idx = vit->first;
      if (reverse_it_) {
        neighbor_idx += vgl_vector_3d<int>(0,0,1);
      }else {
        neighbor_idx += vgl_vector_3d<int>(0,0,-1);
      }
      psm_block_vis_graph_type::iterator nit = vis_graph_.find(neighbor_idx);
      if (nit == vis_graph_.end()) {
        vit->second.incoming_count++;
        curr_blocks_.push_back(vit);
      } else {
        vit->second.incoming_count++;
        nit->second.outgoing_links.push_back(vit);
      }
    }
  }
}


template<psm_apm_type APM>
bool psm_block_visibility_iterator<APM>::next()
{
  vcl_vector<psm_block_vis_graph_type::iterator> to_process;

  vcl_vector<psm_block_vis_graph_type::iterator>::iterator block_it = curr_blocks_.begin();

  for (; block_it != curr_blocks_.end(); ++block_it) {
    // decrement each block's count in list. if count == 0, add to list of nodes to process
    if (--((*block_it)->second.incoming_count) == 0) {
      // add to list
      to_process.push_back(*block_it);
    }
  }
  if (to_process.size() == 0) {
    to_process_indices_.resize(0);
    return false;
  }
  to_process_indices_.resize(to_process.size());

  vcl_vector<vgl_point_3d<int> >::iterator to_proc_it = to_process_indices_.begin();
  for (block_it = to_process.begin(); block_it != to_process.end(); ++block_it, ++to_proc_it) {
    *to_proc_it = (*block_it)->first;
  }

  // add linked blocks to list for next iteration
  curr_blocks_.clear();
  for (block_it = to_process.begin(); block_it != to_process.end(); ++block_it) {
    vcl_vector<psm_block_vis_graph_type::iterator > &links = (*block_it)->second.outgoing_links;
    vcl_vector<psm_block_vis_graph_type::iterator >::iterator neighbor_it = links.begin();
    for (; neighbor_it != links.end(); ++neighbor_it) {
      curr_blocks_.push_back(*neighbor_it);
    }
  }
  return true;
}

template<psm_apm_type APM>
void psm_block_visibility_iterator<APM>::current_blocks(vcl_vector<vgl_point_3d<int> > &blocks)
{
  blocks = to_process_indices_;
}



#define PSM_BLOCK_VISIBILITY_ITERATOR_INSTANTIATE(T) \
  template class psm_block_visibility_iterator<T >


#endif

