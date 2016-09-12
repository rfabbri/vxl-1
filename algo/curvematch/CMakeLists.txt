# curvematch library
# algo/curvematch/CMakeLists.txt

include_directories(${VXL_BRL_INCLUDE_DIR})
include_directories(${VXL_BRL_INCLUDE_DIR}/bbas)
include_directories(${VXL_GEL_INCLUDE_DIR})

set(curvematch_sources

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

aux_source_directory(Templates curvematch_sources)

add_library(curvematch  ${curvematch_sources})

target_link_libraries(curvematch vbl vsol bsol)

if( BUILD_EXAMPLES )
  subdirs( examples )
endif( BUILD_EXAMPLES )


