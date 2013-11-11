# brcv/rec/dbskr/tests/CMakeLists.txt

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp )

ADD_EXECUTABLE( dbskr_test_all
  test_driver.cxx
  test_tree.cxx
  test_tree_edit.cxx
  test_scurve.cxx
  test_scvmatch.cxx
 # test_shock_patch.cxx
)
TARGET_LINK_LIBRARIES( dbskr_test_all dbskr testlib)

ADD_TEST( dbskr_tree_test            dbskr_test_all test_tree ${CMAKE_CURRENT_SOURCE_DIR})
ADD_TEST( dbskr_tree_edit_test       dbskr_test_all test_tree_edit ${CMAKE_CURRENT_SOURCE_DIR})
#ADD_TEST( dbskr_shock_patch_test       dbskr_test_all test_shock_patch ${CMAKE_CURRENT_SOURCE_DIR})
#ADD_TEST( dbskr_scurve_test          dbskr_test_all test_scurve ${CMAKE_CURRENT_SOURCE_DIR})
#ADD_TEST( dbskr_scvmatch_test        dbskr_test_all test_scvmatch ${CMAKE_CURRENT_SOURCE_DIR})

ADD_EXECUTABLE( dbskr_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( dbskr_test_include dbskr ) #dbsk2d_algo
