
set(dboxm_opt_opencl_test_sources 
  test_driver.cxx
  octree_test_driver.h octree_test_driver.hxx
  open_cl_test_data.h  open_cl_test_data.cxx
  test_octree.cxx
  test_backproject_ray.cxx
  test_expected_image.cxx
)


aux_source_directory(Templates dboxm_opt_opencl_test_sources)
add_executable(dboxm_opt_opencl_test_all ${dboxm_opt_opencl_test_sources})
target_link_libraries( dboxm_opt_opencl_test_all dboxm_opt_opencl dboxm_opt dboxm dbocl boct testlib vnl vgl_xio vil vpl vul vpgl)

add_test( NAME  test_octree          COMMAND $<TARGET_FILE:dboxm_opt_opencl_test_all> test_octree         )
add_test( NAME  test_backproject_ray COMMAND $<TARGET_FILE:dboxm_opt_opencl_test_all> test_backproject_ray)
add_test( NAME  test_expected_image  COMMAND $<TARGET_FILE:dboxm_opt_opencl_test_all> test_expected_image )


add_executable( dopen_cl_test_include test_include.cxx )
target_link_libraries( dopen_cl_test_include dboxm_opt_opencl )
