#ifndef psm_refine_scene_h_
#define psm_refine_scene_h_

#include <vcl_vector.h>
#include <vcl_set.h>

#include <vbl/vbl_bounding_box.h>

#include <vgl/vgl_point_3d.h>

#include <psm/psm_scene.h>
#include <psm/psm_sample.h>
#include <psm/psm_apm_traits.h>

#include <hsds/hsds_fd_tree.h>

//: refine the specified scene block
template<psm_apm_type APM>
void psm_refine_scene(psm_scene<APM> &scene, float occlusion_prob_thresh, vgl_point_3d<int> block_idx, bool reset_appearance = true)
{   
  //vcl_vector<hsds_fd_tree_node_index<3> > to_split;
  vcl_set<hsds_fd_tree_node_index<3> > to_split;
  //vcl_vector<psm_sample<APM> > new_vals;

  float max_alpha_int = (float)-vcl_log(1.0 - occlusion_prob_thresh);

  hsds_fd_tree<psm_sample<APM>,3> &block = scene.get_block(block_idx);
  vcl_cout << "max levels = " << block.max_level() << vcl_endl;

  typename hsds_fd_tree<psm_sample<APM>,3>::iterator block_it = block.begin();
  for (; block_it != block.end(); ++block_it) {
    vbl_bounding_box<double,3> cell_bb = block.cell_bounding_box(block_it->first);
    //float alpha_volume = block_it->second.alpha * (float)(cell_bb.volume());
    //float alpha_volume = block_it->second.alpha;
    float side_len = (float)(cell_bb.zmax() - cell_bb.zmin());
    float alpha_int = block_it->second.alpha * side_len;
    if ( (alpha_int > max_alpha_int) && (block_it->first.lvl < block.max_level()) ) {
      to_split.insert(block_it->first);
      // make sure neighbor cells are not already coarser than this cell
      for (unsigned int dim =0; dim < 3; ++dim) {
        hsds_fd_tree_node_index<3> neighbor_idx;
        // neighbor in negative direction
        if (block.neighbor_cell(block_it->first, dim, false, neighbor_idx)) {
          if (neighbor_idx.lvl < block_it->first.lvl) {
            to_split.insert(neighbor_idx);
          }
        }
        // neighbor in positive direction
        if (block.neighbor_cell(block_it->first, dim, true, neighbor_idx)) {
          if (neighbor_idx.lvl < block_it->first.lvl) {
            to_split.insert(neighbor_idx);
          }
        }
      }
    }
  }
  vcl_cout << "splitting " << to_split.size() << " cells. " << vcl_endl;
  vcl_set<hsds_fd_tree_node_index<3> >::iterator split_it = to_split.begin();
  for (; split_it != to_split.end(); ++split_it) {
    psm_sample<APM> old_sample = block[*split_it];
    vbl_bounding_box<double,3> cell_bb = block.cell_bounding_box(*split_it);
    float side_len = (float)(cell_bb.zmax() - cell_bb.zmin());
    float new_alpha = max_alpha_int / side_len; 
    psm_sample<APM> new_sample(new_alpha, typename psm_sample<APM>::apm_datatype());
    // if reset_appearance is set, just leave the appearance created by the default constructor
    if (!reset_appearance) {
      // if it is not set, transfer the old appearance to the new sample
      new_sample.appearance = old_sample.appearance;
    }
    block.split(*split_it,new_sample);
  }
}

//: refine all valid blocks in the scene
template<psm_apm_type APM>
void psm_refine_scene(psm_scene<APM> &scene, float occlusion_prob_thresh, bool reset_appearance = true)
{
  vcl_set<vgl_point_3d<int>, vgl_point_3d_cmp<int> > valid_blocks = scene.valid_blocks();
  vcl_set<vgl_point_3d<int>, vgl_point_3d_cmp<int> >::iterator vbit = valid_blocks.begin();
  for (; vbit != valid_blocks.end(); ++vbit) {
    psm_refine_scene(scene, occlusion_prob_thresh,*vbit,reset_appearance);
  }
}

#endif

