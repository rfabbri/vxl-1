# This is contrib/brl/bseg/bmrf/tests/CMakeLists.txt

include_directories(${BRL_INCLUDE_DIR} )

add_executable ( bmrf_test_all
  test_driver.cxx
  test_epi_point.cxx
  test_epi_seg.cxx
  test_node.cxx
  test_network.cxx
  test_network_builder.cxx
)
TARGET_link_libraries( bmrf_test_all bmrf vgl vil vbl_io vbl vpl testlib )

add_test( NAME  bmrf_test_epi_point       COMMAND $<TARGET_FILE:bmrf_test_all> test_epi_point)
add_test( NAME  bmrf_test_epi_seg         COMMAND $<TARGET_FILE:bmrf_test_all> test_epi_seg)
add_test( NAME  bmrf_test_node            COMMAND $<TARGET_FILE:bmrf_test_all> test_node)
add_test( NAME  bmrf_test_network         COMMAND $<TARGET_FILE:bmrf_test_all> test_network)
add_test( NAME  bmrf_test_network_builder COMMAND $<TARGET_FILE:bmrf_test_all> test_network_builder)

add_executable( bmrf_test_include test_include.cxx)
TARGET_link_libraries( bmrf_test_include bmrf)
