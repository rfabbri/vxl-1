# 3D realisitc lung modeling library
# algo/biolung/CMakeLists.txt

INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR} )

SET(biolung_sources

biolung_3D_Tree.h
biolung_3d_Tree.cxx
biolung_lungmodel.h
biolung_lungmodel.cxx

)

AUX_SOURCE_DIRECTORY(Templates biolung_sources)

ADD_LIBRARY(biolung  ${biolung_sources})

TARGET_LINK_LIBRARIES( biolung vgl vcl vnl vbl vil)

SUBDIRS(pro)

IF(BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)

