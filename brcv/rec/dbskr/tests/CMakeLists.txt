# brcv/rec/dbskr/tests/CMakeLists.txt

include_directories( ${lems_SOURCE_DIR}/brcv/shp )

add_executable( dbskr_test_all
  test_driver.cxx
  test_tree.cxx
  test_tree_edit.cxx
  test_scurve.cxx
  test_scvmatch.cxx
 # test_shock_patch.cxx
)
target_link_libraries( dbskr_test_all dbskr testlib)

add_test( dbskr_tree_test            dbskr_test_all test_tree ${CMAKE_CURRENT_SOURCE_DIR})
add_test( dbskr_tree_edit_test       dbskr_test_all test_tree_edit ${CMAKE_CURRENT_SOURCE_DIR})
#add_test( dbskr_shock_patch_test       dbskr_test_all test_shock_patch ${CMAKE_CURRENT_SOURCE_DIR})
#add_test( dbskr_scurve_test          dbskr_test_all test_scurve ${CMAKE_CURRENT_SOURCE_DIR})
#add_test( dbskr_scvmatch_test        dbskr_test_all test_scvmatch ${CMAKE_CURRENT_SOURCE_DIR})

add_executable( dbskr_test_include test_include.cxx )
target_link_libraries( dbskr_test_include dbskr ) #dbsk2d_algo
