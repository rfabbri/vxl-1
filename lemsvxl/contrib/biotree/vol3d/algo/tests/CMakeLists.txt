
set(vol3d_algo_test_src
  test_driver.cxx 
  test_vol3d_algo_radius_detection.cxx
)

add_executable( biovol3d_algo_test_all ${vol3d_algo_test_src})

add_test( biovol3d_algo_test_radius_filter
          biovol3d_algo_test_all
          test_vol3d_algo_radius_detection
        )

target_link_libraries(biovol3d_algo_test_all testlib vcl vbl vol3d_algo)


