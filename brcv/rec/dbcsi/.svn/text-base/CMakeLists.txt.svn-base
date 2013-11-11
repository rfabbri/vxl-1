# Brown Curve-based Spatial Indexing Library  
# brcv/rec/dbcsi/CMakeLists.txt

INCLUDE_DIRECTORIES(${VXL_GEL_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR})
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/rec )

SET(dbcsi_sources
    dbcsi_shotton_ocm.h             dbcsi_shotton_ocm.cxx
    dbcsi_curve_distance.h          dbcsi_curve_distance.cxx
)

AUX_SOURCE_DIRECTORY(Templates dbcsi_sources)

ADD_LIBRARY(dbcsi  ${dbcsi_sources})

TARGET_LINK_LIBRARIES(dbcsi dbspi dbdet_algo )

# algorithms library
#SUBDIRS(algo)

# Processing Library
#SUBDIRS(pro)

# Visualization Library
#SUBDIRS(vis)

IF(BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)

# Executables
#SUBDIRS(bin)
