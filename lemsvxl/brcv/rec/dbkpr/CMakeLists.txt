# brcv/rec/dbkpr/CMakeLists.txt
# Brown Keypoint Recognition Library

include_directories( ${lems_SOURCE_DIR}/brcv/seg)
include_directories( ${VXL_OXL_INCLUDE_DIR} )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )

set(dbkpr_sources
  dbkpr_view_span_tree.cxx      dbkpr_view_span_tree.h  dbkpr_view_span_tree_sptr.h
#  dbkpr_lowe_cluster.cxx        dbkpr_lowe_cluster.h
)

aux_source_directory(Templates dbkpr_sources)

add_library(dbkpr ${dbkpr_sources})

# add the required libraries into this list
target_link_libraries( dbkpr dbdet mvl bnld_algo )

# Processing Library
add_subdirectory(pro)

# Visualization Library
add_subdirectory(vis)

if(BUILD_TESTING)
#  add_subdirectory(tests)
endif(BUILD_TESTING)
  

