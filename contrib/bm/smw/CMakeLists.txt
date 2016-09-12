# This is contrib/bm/smw/CMakeLists.txt
# State Machine Scene Modeling

project( smw )

#message( "smw_SOURCE_DIR = " ${smw_SOURCE_DIR} )
include_directories( ${VXL_CORE_INCLUDE_DIR} )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg ) #bsta is next step down
include_directories( ${lems_SOURCE_DIR}/contrib/bm )
include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc )

set( smw_sources 
     #smw_node_gaussian_frames.h     smw_node_gaussian_frames.cxx
     #smw_graph_gaussian_frames.h    smw_graph_gaussian_frames.cxx
     smw_frame_clock.h              smw_frame_clock.cxx
     #smw_world_frames.h             smw_world_frames.cxx
     smw_node.h                     smw_node.cxx
     smw_graph.h                    smw_graph.cxx 
     smw_world.h                    smw_world.cxx
)

aux_source_directory( Templates smw_sources )

add_library( smw ${smw_sources} )

target_link_libraries( smw vil vnl bsta )

#add_test(smw_bvxm_voxel_world ${EXECUTABLE_OUTPUT_PATH}/smw_test_all test_smw_bvxm_voxel_world)
#add_test(smw_node ${EXECUTABLE_OUTPUT_PATH}/smw_test_all test_smw_node)

add_subdirectory(tests)

add_subdirectory(executables)

