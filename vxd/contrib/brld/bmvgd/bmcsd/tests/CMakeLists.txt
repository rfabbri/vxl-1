# This is /contrib/brld/bmvgd/bmcsd/tests

add_executable ( bmcsd_test_all
   # Driver source and utilities
   test_driver.cxx

   # The tests
   test_bmcsd_util.cxx
   test_bmcsd_discrete_corresp_io.cxx
   test_bmcsd_discrete_corresp_n_io.cxx
   test_bmcsd_curve_3d_sketch_io.cxx
)

target_link_libraries( bmcsd_test_all bmcsd testlib )

add_test( NAME  bmcsd_test_bmcsd_util COMMAND $<TARGET_FILE:bmcsd_test_all> test_bmcsd_util )
add_test( NAME  bmcsd_test_discrete_corresp_io COMMAND $<TARGET_FILE:bmcsd_test_all> test_discrete_corresp_io )
add_test( NAME  bmcsd_test_discrete_corresp_n_io COMMAND $<TARGET_FILE:bmcsd_test_all> test_discrete_corresp_n_io )
add_test( NAME  bmcsd_curve_3d_sketch_io COMMAND $<TARGET_FILE:bmcsd_test_all> test_bmcsd_curve_3d_sketch_io)

add_executable( bmcsd_test_include test_include.cxx )

target_link_libraries(bmcsd_test_include bmcsd)
