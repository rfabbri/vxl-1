# /contrib/world_model/tests/CMakeLists.txt
include_directories(${lems_SOURCE_DIR}/contrib )

add_executable( world_model_test_all   
                test_driver.cxx  
                test_utm_lat_lon.cxx
              )

target_link_libraries(world_model_test_all world_model testlib)

add_test( NAME test_utm_lat_lon COMMAND $<TARGET_FILE:world_model_test_all>)


