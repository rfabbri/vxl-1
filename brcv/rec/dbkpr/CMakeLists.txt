# brcv/rec/dbkpr/CMakeLists.txt
# Brown Keypoint Recognition Library

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg)
INCLUDE_DIRECTORIES( ${VXL_OXL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )

SET(dbkpr_sources
  dbkpr_view_span_tree.cxx      dbkpr_view_span_tree.h  dbkpr_view_span_tree_sptr.h
#  dbkpr_lowe_cluster.cxx        dbkpr_lowe_cluster.h
)

AUX_SOURCE_DIRECTORY(Templates dbkpr_sources)

ADD_LIBRARY(dbkpr ${dbkpr_sources})

# add the required libraries into this list
TARGET_LINK_LIBRARIES( dbkpr dbdet mvl dbnl_algo )

# Processing Library
SUBDIRS(pro)

# Visualization Library
SUBDIRS(vis)

IF(BUILD_TESTING)
#  SUBDIRS(tests)
ENDIF(BUILD_TESTING)
  

