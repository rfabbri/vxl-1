#ifndef psm_cell_enumerator_h_
#define psm_cell_enumerator_h_

#include <vcl_map.h>
#include <vcl_vector.h>
#include <vcl_algorithm.h>
#include <vcl_utility.h>
#include <vgl/vgl_point_3d.h>

#include <hsds/hsds_fd_tree_node_index.h>
#include <hsds/hsds_fd_tree.h>

#include "psm_scene_base.h"
#include "psm_scene.h"




//: helper class whose job is to enumerate each cell in a scene.
class psm_cell_enumerator {

  typedef vcl_vector<hsds_fd_tree_node_index<3> > cell_list_type;
  typedef vcl_map<vgl_point_3d<int>, cell_list_type, vgl_point_3d_cmp<int> > block_map_type;
  typedef vcl_map<vgl_point_3d<int>, unsigned int, vgl_point_3d_cmp<int> > block_offset_map_type;

public:

  template<psm_apm_type APM>
  psm_cell_enumerator(psm_scene<APM> &scene)
  {
    typename psm_scene<APM>::block_index_iterator block_it = scene.block_index_begin();
    for (; block_it != scene.block_index_end(); ++block_it) {
      hsds_fd_tree<psm_sample<APM>,3> const& block = scene.get_block(*block_it);
      vcl_vector<hsds_fd_tree_node_index<3> > block_cell_list;
      typename hsds_fd_tree<psm_sample<APM>,3>::const_iterator cell_it = block.begin();
      for (; cell_it != block.end(); ++cell_it) {
        block_cell_list.push_back(cell_it->first);
      }
      // sort cell list
      vcl_sort(block_cell_list.begin(), block_cell_list.end());
      // insert into map
      enum_map_.insert(vcl_make_pair<vgl_point_3d<int>, cell_list_type >(*block_it, block_cell_list));
    } 
  }

  bool index_of(vgl_point_3d<int> block_idx, hsds_fd_tree_node_index<3> cell_idx, unsigned int& cell_index) const
  {
    unsigned int offset = 0;

    block_map_type::const_iterator block_it = enum_map_.begin();
    for (; block_it != enum_map_.end(); ++block_it) {
      if (block_it->first == block_idx) {
        cell_list_type::const_iterator cell_it = vcl_lower_bound(block_it->second.begin(), block_it->second.end(), cell_idx);
        if (cell_it == block_it->second.end()) {
          return false;
        }
        cell_index = offset + (cell_it - block_it->second.begin());
        return true;
      }
      offset += block_it->second.size();
    }
    return false;
  }

  bool cell_at(unsigned int index, vgl_point_3d<int> &block_idx, hsds_fd_tree_node_index<3> &cell_idx) const
  {
    unsigned int offset = 0;
    block_map_type::const_iterator block_it = enum_map_.begin();
    for (; block_it != enum_map_.end(); ++block_it) {
      unsigned int ncells = block_it->second.size();
      if (index < offset + ncells) {
        block_idx = block_it->first;
        cell_idx = block_it->second[index - offset];
        return true;
      }
      offset += ncells;
    }
    return false;
  }

  unsigned int size()
  {
    unsigned int count = 0;
    block_map_type::const_iterator block_it = enum_map_.begin();
    for (; block_it != enum_map_.end(); ++block_it) {
      count += block_it->second.size();
    }
    return count;
  }

private:

  //: map containing index maps for each block
  block_map_type enum_map_;
  
};



#endif

