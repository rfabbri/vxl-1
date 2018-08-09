# brcv/shp/dbsk3d/tests/CMakeLists.txt


add_executable( dbsk3d_test_all
      test_driver.cxx
      test_shock_detect.cxx
)

target_link_libraries( dbsk3d_test_all dbsk3d dbsk3d_pro dbmsh3d dbmsh3d_pro testlib )
add_test( NAME  dbsk3d_test_shock_detect COMMAND $<TARGET_FILE:dbsk3d_test_all> dbsk3d_test_shock_detect ${CMAKE_CURRENT_SOURCE_DIR}/data ) 


add_executable( dbsk3d_test_include test_include.cxx )
target_link_libraries( dbsk3d_test_include dbsk3d dbsk3d_algo)


