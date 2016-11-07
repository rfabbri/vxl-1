#if 0
#include <testlib/testlib_test.h>
#include <vbl/vbl_bounding_box.h>
#include <vcl_iostream.h>
#include <vcl_string.h>

#include <hsds/hsds_fd_tree.h>

#include <bvxm/bvxm_voxel_world.h>
#include <bvxm/bvxm_world_params.h>
#include <bvxm/bvxm_voxel_traits.h>
#include <bvxm/bvxm_voxel_grid.h>

#include <bgeo/bgeo_lvcs.h>
#include <vgl/vgl_point_3d.h>


#include "../psm_scene.h"
#include "../psm_sample.h"
#include "../psm_apm_traits.h"
#include "../psm_util.h"

static void test_psm_bvxm_import()
{
  START("bvxm_import test");

  vgl_point_3d<float> corner(-180.0, -160.0, 25.0);
  vcl_string storage_dir = "c:/research/psm/models/capitol_high_bvxm";
  vgl_point_3d<double> scene_origin(corner.x(),corner.y(),corner.z());
  double block_length = 512*0.75;
  psm_scene<PSM_APM_MOG_GREY> scene(scene_origin,block_length,storage_dir);
  vbl_bounding_box<double,3> bbox;
  bbox.update(corner.x(),corner.y(),corner.z());
  bbox.update(corner.x() + block_length, corner.y() + block_length, corner.z() + block_length);

  // create a bvxm voxel world
  vcl_string vox_dir = "c:/research/registration/output/capitol_high_train";
  vgl_vector_3d<unsigned int> voxel_dims(475, 475, 185);
  float vox_len = 0.75f;
  bgeo_lvcs_sptr lvcs = new bgeo_lvcs();
  float min_ocp_prob = 0.001f;
  float max_ocp_prob = 0.99f;
  unsigned int max_scale = 0;

  bvxm_world_params_sptr params = new bvxm_world_params();
  params->set_params(vox_dir, corner, voxel_dims, vox_len, lvcs, min_ocp_prob, max_ocp_prob, max_scale);

  bvxm_voxel_world_sptr vox_world = new bvxm_voxel_world;
  vox_world->set_params(params);

  bvxm_voxel_grid_base_sptr ocp_grid_base = vox_world->get_grid<OCCUPANCY>(0,0);
  bvxm_voxel_grid<float> *ocp_grid = static_cast<bvxm_voxel_grid<float>*>(ocp_grid_base.ptr());


//#define CREATE_SCENE
#ifdef CREATE_SCENE
  
  typedef bvxm_voxel_traits<APM_MOG_GREY>::voxel_datatype apm_datatype;
  bvxm_voxel_grid_base_sptr apm_grid_base = vox_world->get_grid<APM_MOG_GREY>(0,0);
  bvxm_voxel_grid<apm_datatype> *apm_grid = static_cast<bvxm_voxel_grid<apm_datatype>*>(apm_grid_base.ptr());

  vcl_vector<float> max_p_vec;
  vcl_vector<unsigned int> nnodes_vec;

  //for (float max_p = 0.002f; max_p <= 0.01f; max_p += 0.002f) {
  //float max_p = 0.0085f; {
  float max_alpha_sum = 0.5f; {

    //vcl_cout << vcl_endl << "max p = " << max_p << vcl_endl;
    vcl_cout << vcl_endl << "max alpha sum = " << max_alpha_sum << vcl_endl;
    // create octree, start with 2 levels of subdivision
    hsds_fd_tree<psm_sample<PSM_APM_MOG_GREY>,3> octree(bbox,2,psm_sample<PSM_APM_MOG_GREY>());

    bool did_split = true;

    while (did_split) {
      did_split = false;
      unsigned nsplits = 0;

      // keep track of min and max values at each node in tree
      vcl_map<hsds_fd_tree_node_index<3>, float > max_vals;
      vcl_map<hsds_fd_tree_node_index<3>, float > alpha_sum;
      vcl_map<hsds_fd_tree_node_index<3>,unsigned int> vox_per_cell;

      bvxm_voxel_grid<float>::const_iterator grid_it = ocp_grid->begin();
      bvxm_voxel_grid<apm_datatype>::const_iterator apm_it = apm_grid->begin();
      vcl_cout << "finding max value in each cell: ";
      for (unsigned int k=0; grid_it != ocp_grid->end(); ++grid_it, ++apm_it, ++k) {
        vcl_cout << ".";
        bvxm_voxel_slab<float>::const_iterator slab_it = grid_it->begin();
        bvxm_voxel_slab<apm_datatype>::const_iterator apm_slab_it = apm_it->begin();
        for (unsigned int j=0; j<ocp_grid->grid_size().y(); ++j) {
          for (unsigned int i=0; i<ocp_grid->grid_size().x(); ++i, ++slab_it, ++apm_slab_it) {
            // get the point associated with the center of the voxel
            vgl_point_3d<float> pt = vox_world->voxel_index_to_xyz(i,j,k);
            // find the corresponding octree node
            hsds_fd_tree_node_index<3> pt_idx = octree.index_at(vnl_vector_fixed<double,3>(pt.x(),pt.y(),pt.z()));
            // convert P(X) to occlusion density
            float alpha = -vcl_log(vcl_pow(1 - *slab_it, 1/vox_len));
            if ( alpha_sum.find(pt_idx) == alpha_sum.end() ) {
              alpha_sum[pt_idx] = alpha;
            }else {
              alpha_sum[pt_idx] += alpha;
            }
            if ( (max_vals.find(pt_idx) == max_vals.end()) || (max_vals[pt_idx] < *slab_it) ) {
              max_vals[pt_idx] = *slab_it;
              //octree[pt_idx].alpha = alpha;
              octree[pt_idx].appearance = *apm_slab_it;
            }
            if (vox_per_cell.find(pt_idx) == vox_per_cell.end()) {
              vox_per_cell[pt_idx] = 1;
            } else {
              ++vox_per_cell[pt_idx];
            }
          }
        }
      }
      vcl_cout << vcl_endl;
      unsigned int prev_size = octree.size();
      // now split all nodes with sufficient spread between min and max values

      hsds_fd_tree<psm_sample<PSM_APM_MOG_GREY>,3>::iterator oct_it = octree.begin(); 
      vcl_vector<hsds_fd_tree_node_index<3> > to_split;
      for (; oct_it != octree.end(); ++oct_it) {
        vcl_map<hsds_fd_tree_node_index<3>,float>::iterator sum_it = alpha_sum.find(oct_it->first);
        if ( sum_it == alpha_sum.end() ) {
          // no voxels located in this cell.
          oct_it->second.alpha = 0;
          oct_it->second.appearance = psm_apm_traits<PSM_APM_MOG_GREY>::apm_datatype();
          continue;
        }
        unsigned int nvox = vox_per_cell[oct_it->first];
        oct_it->second.alpha = sum_it->second / nvox;
        if ( (sum_it->second > max_alpha_sum) && ( nvox > 1) ) {
          did_split = true;
          ++nsplits;
          to_split.push_back(oct_it->first);
        }
      }
      for (unsigned s=0; s<to_split.size(); ++s) {
        // split and give same value to all children
        octree.split(to_split[s],octree[to_split[s]]);
      }

      vcl_cout << "split " << nsplits << " out of " << prev_size << " nodes. Octree now has " << octree.size() << " nodes. " << vcl_endl;
    }

    vcl_cout << "final octree has " << octree.size() << "nodes. " << vcl_endl;
    max_p_vec.push_back(max_alpha_sum);
    nnodes_vec.push_back(octree.size());
    scene.set_block(vgl_point_3d<int>(0,0,0),octree);
  }

  for (unsigned i=0; i<max_p_vec.size(); ++i) {
    vcl_cout << "max_p = " << max_p_vec[i] << "   nnodes = " << nnodes_vec[i] << vcl_endl;
  }
#endif

    vgl_point_3d<double> slice_pt(corner.x() + voxel_dims.x()*vox_len/2.0f, corner.y() + voxel_dims.y()*vox_len/2.0f, corner.z() + voxel_dims.z()*vox_len/2.0f);

#define SAVE_BVXM_SLICE
#ifdef SAVE_BVXM_SLICE
  {
    // create a slice image of the bvxm voxels for comparision
    vil_image_view<float> bvxm_xz_slice(voxel_dims.x(),voxel_dims.z(),1);
    unsigned int j_idx = voxel_dims.y() / 2;
    vcl_cout << "slicing voxels at " << vox_world->voxel_index_to_xyz(0,j_idx,0) << vcl_endl;
    vcl_cout << "slice_pt = " << slice_pt << vcl_endl;
    bvxm_voxel_grid<float>::const_iterator ocp_it = ocp_grid->begin();
    vcl_cout << "saving bvxm slice" << vcl_endl;
    for (unsigned int k=0; ocp_it != ocp_grid->end(); ++ocp_it, ++k) {
      vcl_cout << ".";
      const float* ocp_ptr = &((*ocp_it)(0,j_idx));
      for (unsigned i=0; i< voxel_dims.x(); ++i) {
        bvxm_xz_slice(i,k) = *ocp_ptr++;
      }
    }
    vcl_cout << "done " << vcl_endl;
    vil_save(bvxm_xz_slice,"c:/research/psm/output/bvxm_xz_slice.tiff");
  }
#endif
  
   hsds_fd_tree<psm_sample<PSM_APM_MOG_GREY>,2> quadtree_xz = psm_util<PSM_APM_MOG_GREY>::xz_slice(slice_pt,scene);
  vcl_ofstream os("c:/research/psm/models/capitol_high_bvxm_xz.fd2",vcl_ios::binary);
  quadtree_xz.b_write(os);
  os.close();

  return;
}



TESTMAIN( test_psm_bvxm_import );
#endif

