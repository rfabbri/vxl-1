# curvematch library
# algo/curvematch/CMakeLists.txt

INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR}/bbas)
INCLUDE_DIRECTORIES(${VXL_GEL_INCLUDE_DIR})

SET(curvematch_sources

CIMatch.h
curve_dpmatch_2d.h
curve_dpmatch_2d.cpp
curve_dpmatch_2d_sptr.h
closed_dpmatch_2d.h
closed_dpmatch_2d.cpp
closed_dpmatch_2d_sptr.h
WSearch.h     
CIMfile.cpp         
CIMatch.cpp     
WImage.cpp          
WSearch.cpp

)

AUX_SOURCE_DIRECTORY(Templates curvematch_sources)

ADD_LIBRARY(curvematch  ${curvematch_sources})

TARGET_LINK_LIBRARIES(curvematch vbl vsol bsol)

IF( BUILD_EXAMPLES )
  SUBDIRS( examples )
ENDIF( BUILD_EXAMPLES )


