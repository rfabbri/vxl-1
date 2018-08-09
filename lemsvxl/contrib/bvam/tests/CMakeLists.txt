add_executable( bvam_test_all
  test_driver.cxx
  test_voxel_storage_mem.cxx
  test_voxel_storage_disk.cxx
  test_voxel_grid.cxx
  test_apm_processors.cxx
  test_voxel_world.cxx
  test_voxel_world_update.cxx
  test_voxel_world_mog_image.cxx
  test_gen_synthetic_world.cxx
)

target_link_libraries( bvam_test_all bvam testlib vcl vbl vpgl bsta bsta_algo)

add_test( NAME  bvam_test_voxel_storage_mem    COMMAND $<TARGET_FILE:bvam_test_all>   test_voxel_storage_mem )
add_test( NAME  bvam_test_voxel_storage_disk   COMMAND $<TARGET_FILE:bvam_test_all>   test_voxel_storage_disk )
add_test( NAME  bvam_test_voxel_grid           COMMAND $<TARGET_FILE:bvam_test_all>   test_voxel_grid )
add_test( NAME  bvam_test_apm_processors   COMMAND $<TARGET_FILE:bvam_test_all>   test_apm_processors )
add_test( NAME  bvam_test_voxel_world      COMMAND $<TARGET_FILE:bvam_test_all>   test_voxel_world )
add_test( NAME  bvam_test_voxel_world_update      COMMAND $<TARGET_FILE:bvam_test_all>   test_voxel_world_update )
add_test( NAME  bvam_test_voxel_world_mog_image   COMMAND $<TARGET_FILE:bvam_test_all>   test_voxel_world_mog_image )
add_test( NAME  bvam_test_gen_synthetic_world     COMMAND $<TARGET_FILE:bvam_test_all>   test_gen_synthetic_world )



