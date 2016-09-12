add_executable( bvaml_test_all
  test_driver.cxx
  test_rays_to_voxels.cxx
  test_voxel.cxx
  test_ray.cxx
  test_supervoxel.cxx
)

target_link_libraries( bvaml_test_all bvaml baml vpgl vpl testlib )
add_test( bvaml_test_rays_to_voxels  bvaml_test_all   test_rays_to_voxels )
add_test( bvaml_test_voxel           bvaml_test_all   test_voxel )
add_test( bvaml_test_supervoxel      bvaml_test_all   test_supervoxel )
add_test( bvaml_test_ray             bvaml_test_all   test_ray )

