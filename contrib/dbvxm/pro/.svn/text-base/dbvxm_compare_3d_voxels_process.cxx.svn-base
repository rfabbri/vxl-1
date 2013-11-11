#include "dbvxm_compare_3d_voxels_process.h"

#include <brdb/brdb_value.h>
#include <bprb/bprb_parameters.h>

#include <vil/vil_image_view_base.h>
#include <vil/vil_convert.h>
#include <brip/brip_vil_float_ops.h>
#include <vpgl/vpgl_camera.h>
#include <vpgl/algo/vpgl_backproject.h>

#include <bvxm/bvxm_voxel_world.h>
#include <dbvxm_multi_scale_voxel_world.h>

#include <bvxm/bvxm_mog_grey_processor.h>
#include <bvxm/bvxm_image_metadata.h>
#include <bvxm/bvxm_voxel_traits.h>
#include <bvxm/bvxm_util.h>
#include <vil/vil_new.h>
#include <brip/brip_vil_float_ops.h>
#include <bsta/bsta_histogram.h>

dbvxm_compare_3d_voxels_process::dbvxm_compare_3d_voxels_process()
{
  //inputs
  input_data_.resize(3,brdb_value_sptr(0));
  input_types_.resize(3);
  input_types_[0] = "bvxm_voxel_world_sptr";   // world
  input_types_[1] = "dbvxm_multi_scale_voxel_world_sptr";
  input_types_[2] = "unsigned";

  //output
  output_data_.resize(1,brdb_value_sptr(0));
  output_types_.resize(1);
  output_types_[0]= "float";      // output an expected image of the object at the highest prob location overlayed 

}


bool dbvxm_compare_3d_voxels_process::execute()
{
  // Sanity check
  if (!this->verify_inputs())
    return false;

  //get the inputs
  brdb_value_t<bvxm_voxel_world_sptr>* input0 =
    static_cast<brdb_value_t<bvxm_voxel_world_sptr>* >(input_data_[0].ptr());
  bvxm_voxel_world_sptr main_world = input0->value();

  brdb_value_t<dbvxm_multi_scale_voxel_world_sptr>* input1 =
    static_cast<brdb_value_t<dbvxm_multi_scale_voxel_world_sptr>* >(input_data_[1].ptr());
  dbvxm_multi_scale_voxel_world_sptr multi_scale_world = input1->value();

   brdb_value_t<unsigned>* input2 =
    static_cast<brdb_value_t<unsigned>* >(input_data_[2].ptr());
  unsigned input_scale = input2->value();

  //: for the ground truth
  bgeo_lvcs_sptr lvcs=main_world->get_params()->lvcs();


  typedef bvxm_voxel_traits<OCCUPANCY>::voxel_datatype ocp_datatype;

  // get ocuppancy probability grids
  bvxm_voxel_grid_base_sptr lidar_ocp_grid_base = main_world->get_grid<OCCUPANCY>(0,0);
  bvxm_voxel_grid<ocp_datatype> *lidar_ocp_grid  = static_cast<bvxm_voxel_grid<ocp_datatype>*>(lidar_ocp_grid_base.ptr());
  //bvxm_voxel_grid<ocp_datatype>::const_iterator l_ocp_slab_it = lidar_ocp_grid->begin();

  bvxm_voxel_grid_base_sptr multi_ocp_grid_base = multi_scale_world->get_grid<OCCUPANCY>(0,input_scale);
  bvxm_voxel_grid<ocp_datatype> *multi_ocp_grid  = static_cast<bvxm_voxel_grid<ocp_datatype>*>(multi_ocp_grid_base.ptr());

  vgl_vector_3d<unsigned int> grid_size = multi_scale_world->get_params()->num_voxels(input_scale);
  bsta_histogram<double> hist(0.0, 1.0, 10, 0.0);
  bvxm_voxel_grid<ocp_datatype>::const_iterator m_ocp_slab_it = multi_ocp_grid->begin();

  int factor=vcl_pow((float)2,(float)input_scale);

  for (unsigned k_idx = 0; k_idx < (unsigned)grid_size.z(); ++k_idx, ++m_ocp_slab_it) {
      bvxm_voxel_slab<float> m_slab = *m_ocp_slab_it;
      bvxm_voxel_grid<ocp_datatype>::const_iterator l_ocp_slab_it = lidar_ocp_grid->slab_iterator(k_idx*factor);
      bvxm_voxel_slab<float> l_slab = *l_ocp_slab_it;

      for (unsigned i=0; i<grid_size.x(); i++) {
          for (unsigned j=0; j<grid_size.y(); j++) {
              hist.upcount(vcl_fabs(m_slab(i,j)-l_slab(i*factor,j*factor)), 1);
              
              //vcl_cout<<m_slab(i,j)<<" "<<l_slab(i*factor,j*factor)<< ":";
          }
      }
  }

  hist.print(vcl_cout);

  return true;
}

