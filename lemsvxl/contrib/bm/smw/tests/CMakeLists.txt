#This is contrib/bm/smw/tests/CMakeLists.txt

include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${smw_SOURCE_DIR} )
#include_directories( ${vxlcore_SOURCE_DIR} )

set( smw_test_src
     test_driver.cxx
     #test_smw_voxel_world.cxx
     #test_smw_node.cxx
     #test_smw_graph.cxx
     #test_smw_world.cxx
     #test_smw_freq_matrix.cxx
)


add_executable( smw_test_all ${smw_test_src} )

target_link_libraries( smw_test_all testlib smw vul vnl)

add_test( smw_test_graph smw_test_all test_graph )

