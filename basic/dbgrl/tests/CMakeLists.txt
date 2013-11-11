# This basic/dbgrl/tests/CMakeLists.txt

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )

SET( dbgrl_test_sources
  test_driver.cxx
  
  test_node.h    test_node_sptr.h
  test_edge.h    test_edge_sptr.h
  test_graph.h   test_graph_sptr.h
  
  graph_test.cxx
)

AUX_SOURCE_DIRECTORY(Templates dbgrl_test_sources)

ADD_EXECUTABLE ( dbgrl_test_all ${dbgrl_test_sources})

TARGET_LINK_LIBRARIES( dbgrl_test_all dbgrl testlib )

ADD_TEST( dbgrl_test_graph   dbgrl_test_all graph_test )

#this is for test_includes
ADD_EXECUTABLE( dbgrl_test_include test_include.cxx)
TARGET_LINK_LIBRARIES( dbgrl_test_include dbgrl)
