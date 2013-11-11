#include <testlib/testlib_register.h>

DECLARE( test_bvam_roi_init_process );
DECLARE( test_bvam_illum_index_process );
DECLARE( test_bvam_create_voxel_world_process );
DECLARE( test_bvam_normalize_image_process );
DECLARE( test_bvam_generate_edge_map_process );
DECLARE( test_bvam_rpc_registration_process );

void register_tests()
{
  REGISTER( test_bvam_roi_init_process );
  REGISTER( test_bvam_illum_index_process );
  REGISTER( test_bvam_create_voxel_world_process );
  REGISTER( test_bvam_normalize_image_process );
  REGISTER( test_bvam_generate_edge_map_process );
  REGISTER( test_bvam_rpc_registration_process );
}

DEFINE_MAIN;
