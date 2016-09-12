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

add_test( bvam_test_voxel_storage_mem    ${EXECUTABLE_OUTPUT_PATH}/bvam_test_all   test_voxel_storage_mem )
add_test( bvam_test_voxel_storage_disk   ${EXECUTABLE_OUTPUT_PATH}/bvam_test_all   test_voxel_storage_disk )
add_test( bvam_test_voxel_grid           ${EXECUTABLE_OUTPUT_PATH}/bvam_test_all   test_voxel_grid )
add_test( bvam_test_apm_processors   ${EXECUTABLE_OUTPUT_PATH}/bvam_test_all   test_apm_processors )
add_test( bvam_test_voxel_world      ${EXECUTABLE_OUTPUT_PATH}/bvam_test_all   test_voxel_world )
add_test( bvam_test_voxel_world_update      ${EXECUTABLE_OUTPUT_PATH}/bvam_test_all   test_voxel_world_update )
add_test( bvam_test_voxel_world_mog_image   ${EXECUTABLE_OUTPUT_PATH}/bvam_test_all   test_voxel_world_mog_image )
add_test( bvam_test_gen_synthetic_world     ${EXECUTABLE_OUTPUT_PATH}/bvam_test_all   test_gen_synthetic_world )



