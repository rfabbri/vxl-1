# rec/dbsk3dr/CMakeLists.txt

# main package for shock3d algorithms.

INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${VXL_RPL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/rec )

SET(dbsk3dr_sources

dbsk3dr_match.cxx              dbsk3dr_match.h
dbsk3dr_dpmatch.cxx            dbsk3dr_dpmatch.h
dbsk3dr_ms_curve.cxx        dbsk3dr_ms_curve.h

)

ADD_LIBRARY(dbsk3dr  ${dbsk3dr_sources})

# add the required libraries into this list
TARGET_LINK_LIBRARIES( dbsk3dr dbsk3d dbsk3d_algo dbmsh3dr dbasn )  

SUBDIRS( vis )

SUBDIRS( pro )

IF(BUILD_TESTING)
SUBDIRS(tests)
ENDIF(BUILD_TESTING)

