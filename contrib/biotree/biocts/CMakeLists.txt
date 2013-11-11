# CT simulation library
# algo/biocts/CMakeLists.txt

INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR} )

SET(biocts_sources

biocts_rvgs.h
biocts_rvgs.cxx
biocts_rngs.h
biocts_rngs.cxx
biocts_Volume3D.h
biocts_Volume3D.cxx
biocts_ctsim.h
biocts_ctsim.cxx

)

AUX_SOURCE_DIRECTORY(Templates biocts_sources)

ADD_LIBRARY(biocts  ${biocts_sources})

TARGET_LINK_LIBRARIES( biocts vgl vcl vnl vbl vil)

SUBDIRS(pro)

IF(BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)
