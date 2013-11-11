# rec/dbmshdr/CMakeLists.txt

# main package for mesh registration algorithms.

INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${VXL_RPL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp)
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/rec)

SET(dbmsh3dr_sources

dbmsh3dr_match.cxx          dbmsh3dr_match.h
dbmsh3dr_icp.cxx            
dbmsh3dr_icp_rgrl.cxx            
dbmsh3dr_error_estim.cxx
dbmsh3dr_scan_fuse.cxx

)

ADD_LIBRARY(dbmsh3dr  ${dbmsh3dr_sources})

# add the required libraries into this list
TARGET_LINK_LIBRARIES( dbmsh3dr dbmsh3d dbmsh3d_algo dbgl rgrl )  

SUBDIRS( vis )

SUBDIRS( pro )

IF(BUILD_TESTING)
#SUBDIRS(tests)
ENDIF(BUILD_TESTING)



