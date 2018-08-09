# This is rec/dbasn/tests/CMakeLists.txt

add_executable(dbasn_test_all 
      test_driver.cxx

      dbasn_test_artificial_def.cxx     dbasn_test_artificial_def.h

      dbasn_test_artificial_graph.cxx
      dbasn_test_random_graph.cxx
      dbasn_test_simple_graph.cxx

      dbasn_test_aug_graph.cxx

      dbasnh_test_artificial_hypg.cxx
      dbasnh_test_random_hypg.cxx
      dbasnh_test_simple_hypg.cxx

      dbasnh_test_aug_hypg.cxx
      dbasnh_test_str_hypg.cxx
      dbasnh_test_str_aug_hypg.cxx

)
target_link_libraries( dbasn_test_all dbasn testlib )

add_test( NAME dbasn_test_artificial_graph  COMMAND $<TARGET_FILE:dbasn_test_all>  dbasn_test_artificial_graph)
add_test( NAME dbasn_test_random_graph      COMMAND $<TARGET_FILE:dbasn_test_all>  dbasn_test_random_graph)
add_test( NAME dbasn_test_simple_graph      COMMAND $<TARGET_FILE:dbasn_test_all>  dbasn_test_simple_graph ${CMAKE_CURRENT_SOURCE_DIR}/data) 

add_test( NAME dbasn_test_aug_graph  COMMAND $<TARGET_FILE:dbasn_test_all>  dbasn_test_aug_graph)

add_test( NAME dbasnh_test_artificial_hypg  COMMAND $<TARGET_FILE:dbasn_test_all>  dbasnh_test_artificial_hypg)
add_test( NAME dbasnh_test_random_hypg      COMMAND $<TARGET_FILE:dbasn_test_all>  dbasnh_test_random_hypg)
add_test( NAME dbasnh_test_simple_hypg      COMMAND $<TARGET_FILE:dbasn_test_all>  dbasnh_test_simple_hypg ${CMAKE_CURRENT_SOURCE_DIR}/data) 

add_test( NAME dbasnh_test_aug_hypg  COMMAND $<TARGET_FILE:dbasn_test_all>  dbasnh_test_aug_hypg)
add_test( NAME dbasnh_test_str_hypg  COMMAND $<TARGET_FILE:dbasn_test_all>  dbasnh_test_str_hypg)
add_test( NAME dbasnh_test_str_aug_hypg  COMMAND $<TARGET_FILE:dbasn_test_all>  dbasnh_test_str_aug_hypg)

add_executable( dbasn_test_include dbasn_test_include.cxx )
target_link_libraries( dbasn_test_include dbasn )
