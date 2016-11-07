#ifndef psm_util_h_
#define psm_util_h_

#include <hsds/hsds_fd_tree.h>
#include <vbl/vbl_bounding_box.h>
#include <vgl/vgl_point_3d.h>

#include "psm_apm_traits.h"

template <psm_apm_type sample_type>
class psm_util
{
public:
  static hsds_fd_tree<psm_sample<sample_type>,2> xz_slice(vgl_point_3d<double> pt, psm_scene<sample_type>& scene)
  {
    vgl_point_3d<int> block_idx = scene.block_index(pt);
    hsds_fd_tree<psm_sample<sample_type>,3> &block = scene.get_block(block_idx);
    vbl_bounding_box<double,2> bbox_xz;
    bbox_xz.update(block.bounding_box().xmin(),block.bounding_box().zmin());
    bbox_xz.update(block.bounding_box().xmax(),block.bounding_box().zmax());
    // create the quadtree nodes
    vcl_map<hsds_fd_tree_node_index<2>, psm_sample<sample_type> > alpha_xz;

    // get index of point
    hsds_fd_tree_node_index<3> pt_index = block.full_index_at(vnl_vector_fixed<double,3>(pt.x(),pt.y(),pt.z()));
    //vcl_cout << "pt_index = " << pt_index << vcl_endl;
    // create xz mask
    unsigned int xz_mask = 0;
    unsigned int xz_mask_3bit = 0x02; // just include y bit in mask
    for (unsigned int i=1; i <= block.max_level(); ++i) {
      xz_mask += (xz_mask_3bit << hsds_fd_tree_node_index<3>::level_shift(i));
    }
    //vcl_cout << "xz_mask = 0x" << vcl_hex << xz_mask << vcl_endl;

    typename hsds_fd_tree<psm_sample<sample_type> ,3>::const_iterator block_it = block.begin();
    for (; block_it != block.end(); ++block_it) {
      // compare with pt_index to see if node belongs in slice
      unsigned int level_shift = hsds_fd_tree_node_index<3>::level_shift(block_it->first.lvl);
      //vcl_cout << "  node index = " << block_it->first << vcl_endl;
      if ((pt_index.idx & xz_mask) >> level_shift == (block_it->first.idx & xz_mask) >> level_shift) {
        //vcl_cout << "********* node is a match ***********" << vcl_endl;
        //vcl_cout << block_it->first << vcl_endl;
        // create 2d version of cell
        unsigned int index_2d = 0;
        unsigned int b2d = sizeof(unsigned int)*8 - 1;
        for (int b=sizeof(unsigned int)*8 - 1; b >= 0; --b) {
          if ( (~xz_mask) & (1 << b) ) {
            unsigned int bval = (block_it->first.idx & (1 << b)) >> b;
            index_2d |= bval << b2d;
            --b2d;
          }
        }
        // insert into quadtree
        hsds_fd_tree_node_index<2> quad_index(index_2d, block_it->first.lvl);
        alpha_xz.insert(vcl_make_pair<hsds_fd_tree_node_index<2>, psm_sample<sample_type>  >(quad_index,block_it->second));
      }
    }
    hsds_fd_tree<psm_sample<sample_type> ,2> slice_xz(bbox_xz, alpha_xz);
    return slice_xz;
  }


};

#endif
