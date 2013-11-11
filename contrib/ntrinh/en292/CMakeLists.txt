# contrib/ntrinh/en292/CMakeLists.txt
# Library for en292 final project (Spring 2005)

INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )

SET( en292_sources
  en292_insert_scene.cxx                 en292_insert_scene.h 
)

AUX_SOURCE_DIRECTORY(Templates en292_sources)
ADD_LIBRARY(en292 ${en292_sources})
TARGET_LINK_LIBRARIES( en292 vgl vsol )

SUBDIRS( pro )
SUBDIRS( vis )

#IF( BUILD_TESTING )
#  SUBDIRS(tests)
#ENDIF( BUILD_TESTING )

IF( BUILD_EXAMPLES )
#  SUBDIRS( examples )
ENDIF( BUILD_EXAMPLES )
