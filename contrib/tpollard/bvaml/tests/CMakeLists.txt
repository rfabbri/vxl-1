ADD_EXECUTABLE( bvaml_test_all
  test_driver.cxx
  test_rays_to_voxels.cxx
  test_voxel.cxx
  test_ray.cxx
  test_supervoxel.cxx
)

TARGET_LINK_LIBRARIES( bvaml_test_all bvaml baml vpgl vpl testlib )
ADD_TEST( bvaml_test_rays_to_voxels  bvaml_test_all   test_rays_to_voxels )
ADD_TEST( bvaml_test_voxel           bvaml_test_all   test_voxel )
ADD_TEST( bvaml_test_supervoxel      bvaml_test_all   test_supervoxel )
ADD_TEST( bvaml_test_ray             bvaml_test_all   test_ray )

