#ifndef psm_cell_visibility_iterator_txx_
#define psm_cell_visibility_iterator_txx_

#include <vcl_vector.h>

#include <vbl/vbl_bounding_box.h>
#include <vgl/vgl_point_3d.h>
#include <vpgl/vpgl_perspective_camera.h>

#include <hsds/hsds_fd_tree.h>

#include "psm_raytrace_operations.h"
#include "psm_cell_visibility_iterator.h"

template<psm_apm_type APM>
psm_cell_visibility_iterator<APM>::psm_cell_visibility_iterator(hsds_fd_tree<psm_sample<APM>,3> const& block, vpgl_perspective_camera<double> const& cam, unsigned int img_ni, unsigned int img_nj, bool full_vis_needed, bool reverse_iterator)
: block_(block), cam_(cam), img_ni_(img_ni), img_nj_(img_nj), vis_graph_(block_.twin_tree(psm_cell_vis_graph_node())), reverse_it_(reverse_iterator), full_vis_needed_(full_vis_needed)
{
  cell_count_ = 0;
  vgl_point_3d<double> cam_center(cam_.camera_center());

  // add visibility edges to each node. add unoccluded cells to curr_cells_
  hsds_fd_tree<psm_sample<APM> ,3>::const_iterator nit = block_.begin();
  hsds_fd_tree<psm_cell_vis_graph_node,3>::iterator vit = vis_graph_.begin();
  for (; nit != block_.end(); ++nit, ++vit) {

    vbl_bounding_box<double,3> cell_bb = block.cell_bounding_box(nit->first);

    // find visible faces of cell
    psm_cube_face_list vis_faces = visible_faces(cell_bb,cam_center);

    // add edge to other neighboring cell for each visible face
    if (vis_faces & psm_cube_face::X_HIGH) {
      vcl_vector<hsds_fd_tree_node_index<3> > neighbors;
      // find neighbors on X_HIGH face
      bool search_high = !reverse_it_;
      block_.neighbor_cells(nit->first,0,search_high,neighbors);
      if (neighbors.size() == 0) {
        vit->second.incoming_count++;
        curr_cells_.push_back(vit);
      } else {
        vcl_vector<hsds_fd_tree_node_index<3> >::iterator neighbor_it = neighbors.begin();
        for (; neighbor_it != neighbors.end(); ++neighbor_it) {
          vit->second.incoming_count++;
          vis_graph_[*neighbor_it].outgoing_links.push_back(vit);
        }
      }
    } else if (vis_faces & psm_cube_face::X_LOW) {
      vcl_vector<hsds_fd_tree_node_index<3> > neighbors;
      // find neighors on X_LOW face
      bool search_high = reverse_it_;
      block_.neighbor_cells(nit->first,0,search_high,neighbors);
      if (neighbors.size() == 0) {
        vit->second.incoming_count++;
        curr_cells_.push_back(vit);
      } else {
        vcl_vector<hsds_fd_tree_node_index<3> >::iterator neighbor_it = neighbors.begin();
        for (; neighbor_it != neighbors.end(); ++neighbor_it) {
          vit->second.incoming_count++;
          vis_graph_[*neighbor_it].outgoing_links.push_back(vit);
        }
      }
    }
    if (vis_faces & psm_cube_face::Y_HIGH) {
      vcl_vector<hsds_fd_tree_node_index<3> > neighbors;
      // find neighbors on Y_HIGH face
      bool search_high = !reverse_it_;
      block_.neighbor_cells(nit->first,1,search_high,neighbors);
      if (neighbors.size() == 0) {
        vit->second.incoming_count++;
        curr_cells_.push_back(vit);
      } else {
        vcl_vector<hsds_fd_tree_node_index<3> >::iterator neighbor_it = neighbors.begin();
        for (; neighbor_it != neighbors.end(); ++neighbor_it) {
          vit->second.incoming_count++;
          vis_graph_[*neighbor_it].outgoing_links.push_back(vit);
        }
      }
    } else if (vis_faces & psm_cube_face::Y_LOW) {
      vcl_vector<hsds_fd_tree_node_index<3> > neighbors;
      // find neighors on Y_LOW face
      bool search_high = reverse_it_;
      block_.neighbor_cells(nit->first,1,search_high,neighbors);
      if (neighbors.size() == 0) {
        vit->second.incoming_count++;
        curr_cells_.push_back(vit);
      } else {
        vcl_vector<hsds_fd_tree_node_index<3> >::iterator neighbor_it = neighbors.begin();
        for (; neighbor_it != neighbors.end(); ++neighbor_it) {
          vit->second.incoming_count++;
          vis_graph_[*neighbor_it].outgoing_links.push_back(vit);
        }
      }
    }

    if (vis_faces & psm_cube_face::Z_HIGH) {
      vcl_vector<hsds_fd_tree_node_index<3> > neighbors;
      // find neighbors on Z_HIGH face
      bool search_high = !reverse_it_;
      block_.neighbor_cells(nit->first,2,search_high,neighbors);
      if (neighbors.size() == 0) {
        vit->second.incoming_count++;
        curr_cells_.push_back(vit);
      } else {
        vcl_vector<hsds_fd_tree_node_index<3> >::iterator neighbor_it = neighbors.begin();
        for (; neighbor_it != neighbors.end(); ++neighbor_it) {
          vit->second.incoming_count++;
          vis_graph_[*neighbor_it].outgoing_links.push_back(vit);
        }
      }
    } else if (vis_faces & psm_cube_face::Z_LOW) {
      vcl_vector<hsds_fd_tree_node_index<3> > neighbors;
      // find neighors on Z_LOW face
      bool search_high = reverse_it_;
      block_.neighbor_cells(nit->first,2,reverse_it_,neighbors);
      if (neighbors.size() == 0) {
        vit->second.incoming_count++;
        curr_cells_.push_back(vit);
      } else {
        vcl_vector<hsds_fd_tree_node_index<3> >::iterator neighbor_it = neighbors.begin();
        for (; neighbor_it != neighbors.end(); ++neighbor_it) {
          vit->second.incoming_count++;
          vis_graph_[*neighbor_it].outgoing_links.push_back(vit);
        }
      }
    }

  }
}


template<psm_apm_type APM>
bool psm_cell_visibility_iterator<APM>::next()
{
  vcl_vector<hsds_fd_tree<psm_cell_vis_graph_node,3>::iterator > to_process;

  vcl_vector<hsds_fd_tree<psm_cell_vis_graph_node,3>::iterator >::iterator cell_it = curr_cells_.begin();
  for (; cell_it != curr_cells_.end(); ++cell_it) {
    // decrement each cell's count in list. if count == 0, add to list of nodes to process
    if (--((*cell_it)->second.incoming_count) == 0) {
      to_process.push_back(*cell_it);
    }
  }
  if (to_process.size() == 0) {
    return false;
  }
  to_process_indices_.clear();

  for (cell_it = to_process.begin(); cell_it != to_process.end(); ++cell_it) {
    // make sure cell projects to inside of image
    vbl_bounding_box<double,3> cell_bb = vis_graph_.cell_bounding_box((*cell_it)->first);
    if (full_vis_needed_) {
      if (full_cube_visible(cell_bb, cam_, img_ni_, img_nj_, true)) {
        to_process_indices_.push_back((*cell_it)->first);
      }
    } else {
      if (cube_visible(cell_bb, cam_, img_ni_, img_nj_, true)) {
        // add to list
        to_process_indices_.push_back((*cell_it)->first);
      }
    }
  }

  // add linked cells to list for next iteration
  curr_cells_.clear();
  for (cell_it = to_process.begin(); cell_it != to_process.end(); ++cell_it) {
    vcl_vector<hsds_fd_tree<psm_cell_vis_graph_node,3>::iterator > &links = (*cell_it)->second.outgoing_links;
    vcl_vector<hsds_fd_tree<psm_cell_vis_graph_node,3>::iterator >::iterator neighbor_it = links.begin();
    for (; neighbor_it != links.end(); ++neighbor_it) {
      curr_cells_.push_back(*neighbor_it);
    }
  }
  return true;
}

template<psm_apm_type APM>
void psm_cell_visibility_iterator<APM>::current_cells(vcl_vector<hsds_fd_tree_node_index<3> > &cells)
{
  cells = to_process_indices_;
}



#define PSM_CELL_VISIBILITY_ITERATOR_INSTANTIATE(T) \
  template class psm_cell_visibility_iterator<T >


#endif

