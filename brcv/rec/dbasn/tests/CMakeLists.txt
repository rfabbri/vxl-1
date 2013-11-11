# This is rec/dbasn/tests/CMakeLists.txt

ADD_EXECUTABLE(dbasn_test_all 
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
TARGET_LINK_LIBRARIES( dbasn_test_all dbasn testlib )

ADD_TEST(dbasn_test_artificial_graph  dbasn_test_all  dbasn_test_artificial_graph)
ADD_TEST(dbasn_test_random_graph      dbasn_test_all  dbasn_test_random_graph)
ADD_TEST(dbasn_test_simple_graph      dbasn_test_all  dbasn_test_simple_graph ${CMAKE_CURRENT_SOURCE_DIR}/data) 

ADD_TEST(dbasn_test_aug_graph  dbasn_test_all  dbasn_test_aug_graph)

ADD_TEST(dbasnh_test_artificial_hypg  dbasn_test_all  dbasnh_test_artificial_hypg)
ADD_TEST(dbasnh_test_random_hypg      dbasn_test_all  dbasnh_test_random_hypg)
ADD_TEST(dbasnh_test_simple_hypg      dbasn_test_all  dbasnh_test_simple_hypg ${CMAKE_CURRENT_SOURCE_DIR}/data) 

ADD_TEST(dbasnh_test_aug_hypg  dbasn_test_all  dbasnh_test_aug_hypg)
ADD_TEST(dbasnh_test_str_hypg  dbasn_test_all  dbasnh_test_str_hypg)
ADD_TEST(dbasnh_test_str_aug_hypg  dbasn_test_all  dbasnh_test_str_aug_hypg)

ADD_EXECUTABLE( dbasn_test_include dbasn_test_include.cxx )
TARGET_LINK_LIBRARIES( dbasn_test_include dbasn )
