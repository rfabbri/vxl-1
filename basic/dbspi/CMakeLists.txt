# This is basic/dbspi/CMakeLists.txt
# Brown Spatial Index Library


INCLUDE_DIRECTORIES(${VXL_GEL_INCLUDE_DIR} )

SET(dbspi_sources
    dbspi_chamfer.txx          dbspi_chamfer.h
    dbspi_curve_map.cxx        dbspi_curve_map.h
    dbspi_curve_distance.cxx   dbspi_curve_distance.h
)

AUX_SOURCE_DIRECTORY(Templates dbspi_sources)

ADD_LIBRARY(dbspi ${dbspi_sources})

TARGET_LINK_LIBRARIES(dbspi vsol vil)

# Processing Library
SUBDIRS(pro)

IF(BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)
