# brcv/shp/dbsk3d/tests/CMakeLists.txt


ADD_EXECUTABLE( dbsk3d_test_all
      test_driver.cxx
      test_shock_detect.cxx
)

TARGET_LINK_LIBRARIES( dbsk3d_test_all dbsk3d dbsk3d_pro dbmsh3d dbmsh3d_pro testlib )
ADD_TEST( dbsk3d_test_shock_detect dbsk3d_test_all dbsk3d_test_shock_detect ${CMAKE_CURRENT_SOURCE_DIR}/data ) 


ADD_EXECUTABLE( dbsk3d_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( dbsk3d_test_include dbsk3d dbsk3d_algo)


