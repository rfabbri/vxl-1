# This is basic/dbspi/pro/CMakeLists.txt
# Brown Spatial Index Processing library

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )

SET(dbspi_pro_sources
#   dbspi_curvemap_process.cxx   dbspi_curvemap_process.h
   dbspi_curvemap_storage.cxx   dbspi_curvemap_storage.h    dbspi_curvemap_storage_sptr.h
)

#AUX_SOURCE_DIRECTORY(Templates dbspi_pro_sources)

ADD_LIBRARY(dbspi_pro ${dbspi_pro_sources})

TARGET_LINK_LIBRARIES(dbspi_pro dbspi vidpro1)


IF(BUILD_TESTING)
#  SUBDIRS(tests)
ENDIF(BUILD_TESTING)

