# contrib/maruthi/CMakeLists.txt
# This file is used to build the proximity graph stuff


include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${LEMS_SOURCE_DIR}/brcv/rec)
include_directories( ${LEMS_SOURCE_DIR}/breye1 )
include_directories( ${LEMS_SOURCE_DIR}/contrib/maruthi )
include_directories( /usr/local/boost_1_42_0 )

link_directories(/usr/local/boost_1_42_0/stage/lib )

#turn off warnings in boost graph library
set( CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS}-Wno-deprecated)

set(gc_sources

  # Creates graphs
  dborl_proximity_graph.h                dborl_proximity_graph.cxx
  dborl_knn_graph.h                      dborl_knn_graph.cxx
  dborl_graph_categorization.h           dborl_graph_categorization.cxx

  # Vox wrapper
  dborl_graph_construction_params.h      dborl_graph_construction_params.cxx
  dborl_graph_construction_params_sptr.h vbl_smart_ptr+dborl_graph_construction_params-.cxx

  # Vox wrapper graph embedding
  dborl_graph_categorization_params.h      dborl_graph_categorization_params.cxx
  dborl_graph_categorization_params_sptr.h vbl_smart_ptr+dborl_graph_categorization_params-.cxx
 

)

set(find_nn_sources

  #performs wave propagation
  dborl_graph_wave_propagation.h         dborl_graph_wave_propagation.cxx

  #vox wrapper around find nn algorithm
  dborl_find_nn_params.h      dborl_find_nn_params.cxx
  dborl_find_nn_params_sptr.h vbl_smart_ptr+dborl_find_nn_params-.cxx
)

add_library(graph_building ${gc_sources})
add_library(finding_nn ${find_nn_sources})

#add_executable(example test_things.cxx )
add_executable(graph_categorization dborl_graph_categorization_main.cxx )
add_executable(graph_constructor dborl_graph_construction_main.cxx )
add_executable(search_nn dborl_find_nn_main.cxx )

target_link_libraries(graph_constructor vnl graph_building dborl_algo boost_graph )
target_link_libraries(search_nn vnl dborl_algo finding_nn boost_graph)
target_link_libraries(graph_categorization vnl graph_building dborl_algo boost_graph)
#target_link_libraries(example vnl graph_building dborl_algo boost_graph)
