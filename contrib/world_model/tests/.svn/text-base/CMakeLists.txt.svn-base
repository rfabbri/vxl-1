# /contrib/world_model/tests/CMakeLists.txt
INCLUDE_DIRECTORIES(${lems_SOURCE_DIR}/contrib )

ADD_EXECUTABLE( world_model_test_all   
                test_driver.cxx  
                test_utm_lat_lon.cxx
              )

TARGET_LINK_LIBRARIES(world_model_test_all world_model testlib)

ADD_TEST(test_utm_lat_lon world_model_test_all)


