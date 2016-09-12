# This is brcv/seg/dbacm/tests/CMakeLists.txt


add_executable( dbacm_test_all
  test_driver.cxx
  test_image_filters.cxx
)
target_link_libraries( dbacm_test_all dbacm testlib )

# add_test( )


add_executable( dbacm_test_include test_include.cxx )
target_link_libraries( dbacm_test_include dbacm )
