#include <testlib/testlib_register.h>

DECLARE( test_rays_to_voxels );
DECLARE( test_voxel );
DECLARE( test_supervoxel );
DECLARE( test_ray );

void register_tests()
{
  REGISTER( test_rays_to_voxels );
  REGISTER( test_voxel );
  REGISTER( test_supervoxel );
  REGISTER( test_ray );
}

DEFINE_MAIN;
