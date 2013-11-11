# This is brcv/mvg/dbdif/CMakeLists.txt
# Brown Multiview Differential Geometry Library

INCLUDE_DIRECTORIES(${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${VXL_GEL_INCLUDE_DIR}/mrc )


SET(dbdif_sources
dbdif_analytic.cxx dbdif_analytic.h
dbdif_camera.cxx dbdif_camera.h dbdif_frenet.h 
dbdif_rig.cxx dbdif_rig.h
dbdif_util.cxx dbdif_util.h
)

AUX_SOURCE_DIRECTORY(Templates dbdif_sources)

ADD_LIBRARY(dbdif ${dbdif_sources})

TARGET_LINK_LIBRARIES(dbdif vpgl vsol)

# Algorithms Library
SUBDIRS(algo)

# Processing Library
SUBDIRS(pro)

# Visualization Library
#SUBDIRS(vis)

IF(BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)
