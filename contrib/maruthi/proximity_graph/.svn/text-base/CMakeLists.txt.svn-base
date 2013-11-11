# contrib/maruthi/CMakeLists.txt
# This file is used to build the proximity graph stuff


INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/rec)
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/maruthi )
INCLUDE_DIRECTORIES( /usr/local/boost_1_42_0 )

LINK_DIRECTORIES(/usr/local/boost_1_42_0/stage/lib )

#turn off warnings in boost graph library
SET( CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS}-Wno-deprecated)

SET(gc_sources

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

SET(find_nn_sources

  #performs wave propagation
  dborl_graph_wave_propagation.h         dborl_graph_wave_propagation.cxx

  #vox wrapper around find nn algorithm
  dborl_find_nn_params.h      dborl_find_nn_params.cxx
  dborl_find_nn_params_sptr.h vbl_smart_ptr+dborl_find_nn_params-.cxx
)

ADD_LIBRARY(graph_building ${gc_sources})
ADD_LIBRARY(finding_nn ${find_nn_sources})

#ADD_EXECUTABLE(example test_things.cxx )
ADD_EXECUTABLE(graph_categorization dborl_graph_categorization_main.cxx )
ADD_EXECUTABLE(graph_constructor dborl_graph_construction_main.cxx )
ADD_EXECUTABLE(search_nn dborl_find_nn_main.cxx )

TARGET_LINK_LIBRARIES(graph_constructor vnl graph_building dborl_algo boost_graph )
TARGET_LINK_LIBRARIES(search_nn vnl dborl_algo finding_nn boost_graph)
TARGET_LINK_LIBRARIES(graph_categorization vnl graph_building dborl_algo boost_graph)
#TARGET_LINK_LIBRARIES(example vnl graph_building dborl_algo boost_graph)
