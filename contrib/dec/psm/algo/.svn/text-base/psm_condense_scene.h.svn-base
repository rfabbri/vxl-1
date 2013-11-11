#ifndef psm_condense_scene_h_
#define psm_condense_scene_h_

#include <vcl_vector.h>
#include <vcl_set.h>
#include <vcl_map.h>

#include <vbl/vbl_bounding_box.h>

#include <vgl/vgl_point_3d.h>

#include <psm/psm_scene.h>
#include <psm/psm_sample.h>
#include <psm/psm_apm_traits.h>

#include <hsds/hsds_fd_tree_node_index.h>
#include <hsds/hsds_fd_tree.h>

//: condense the specified scene block
template<psm_apm_type APM>
void psm_condense_scene(psm_scene<APM> &scene, float occlusion_prob_thresh, vgl_point_3d<int> block_idx, unsigned int min_level)
{   
  //vcl_vector<hsds_fd_tree_node_index<3> > to_split;
  vcl_set<hsds_fd_tree_node_index<3> > to_split;
  //vcl_vector<psm_sample<APM> > new_vals;

  float max_alpha_int = (float)-vcl_log(1.0 - occlusion_prob_thresh);

  hsds_fd_tree<psm_sample<APM>,3> &block = scene.get_block(block_idx);

  // start at max_levels - 1
  unsigned int max_level = block.max_level();
  if (max_level <= min_level) {
    return;
  }
  for (int level = max_level - 1; level >= (int)min_level; --level) {
    vcl_cout << "level = " << level << vcl_endl;
    unsigned int nmerged = 0;

    unsigned int max_level_index = 2 << level;
    for (unsigned int x=0; x < max_level_index; ++x) {
      for (unsigned int y=0; y < max_level_index; ++y) {
        for (unsigned int z=0; z < max_level_index; ++z) {
          hsds_fd_tree_node_index<3> index(0,level);
          index.set_node_coordinate(0,x);
          index.set_node_coordinate(1,y);
          index.set_node_coordinate(2,z);
          typename hsds_fd_tree<psm_sample<APM>,3>::const_iterator node_it = block.lower_bound(index);
          vbl_bounding_box<double,3> parent_bb = block.cell_bounding_box(index);
          float child_side_len = float(parent_bb.xmax() - parent_bb.xmin()) / 2.0f;
          if ((int)(node_it->first.lvl) <= level) {
            // node is already higher up in the tree - nothing to do
            continue;
          }
          bool do_merge = true;
          // check all 8 children
          float mean_alpha = 0.0f;
          hsds_fd_tree_node_index<3> child_index0(index.idx,level+1);
          for (unsigned int cx=0; cx<=1; ++cx) {
            for (unsigned int cy=0; cy<=1; ++cy) {
              for (unsigned int cz=0; cz<=1; ++cz) {
                hsds_fd_tree_node_index<3> child_index(child_index0);
                child_index.set_node_coordinate(0,child_index0.get_node_coordinate(0)+cx);
                child_index.set_node_coordinate(1,child_index0.get_node_coordinate(1)+cy);
                child_index.set_node_coordinate(2,child_index0.get_node_coordinate(2)+cz);
                typename hsds_fd_tree<psm_sample<APM>,3>::const_iterator child_it = block.find(child_index);
                if ( (child_it == block.end()) || (child_it->first.lvl != child_index.lvl) ) {
                  // no direct child - cannot do merge
                  do_merge = false;
                  break;
                }
                if (child_it->second.alpha * child_side_len > max_alpha_int) {
                  do_merge = false;
                  break;
                }
                mean_alpha += child_it->second.alpha * 0.125f;
              }
            }
          }
          // all 8 children are below threshold, merge all 
          if (do_merge) {
            nmerged += 8;
            psm_sample<APM> new_cell(mean_alpha);
            block.merge(index,new_cell);
          }
        }
      }
    }
    vcl_cout << "  merged " << nmerged << " cells." << vcl_endl;
  }
}

//: condense all valid blocks in the scene
template<psm_apm_type APM>
void psm_condense_scene(psm_scene<APM> &scene, float occlusion_prob_thresh, unsigned int min_level)
{
  vcl_set<vgl_point_3d<int>, vgl_point_3d_cmp<int> > valid_blocks = scene.valid_blocks();
  vcl_set<vgl_point_3d<int>, vgl_point_3d_cmp<int> >::iterator vbit = valid_blocks.begin();
  for (; vbit != valid_blocks.end(); ++vbit) {
    psm_condense_scene(scene, occlusion_prob_thresh,*vbit, min_level);
  }
}

#endif

