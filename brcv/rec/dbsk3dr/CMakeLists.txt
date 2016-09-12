# rec/dbsk3dr/CMakeLists.txt

# main package for shock3d algorithms.

include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_RPL_INCLUDE_DIR} )
include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${lems_SOURCE_DIR}/brcv/shp )
include_directories( ${lems_SOURCE_DIR}/brcv/rec )

set(dbsk3dr_sources

dbsk3dr_match.cxx              dbsk3dr_match.h
dbsk3dr_dpmatch.cxx            dbsk3dr_dpmatch.h
dbsk3dr_ms_curve.cxx        dbsk3dr_ms_curve.h

)

add_library(dbsk3dr  ${dbsk3dr_sources})

# add the required libraries into this list
target_link_libraries( dbsk3dr dbsk3d dbsk3d_algo dbmsh3dr dbasn )  

subdirs( vis )

subdirs( pro )

if(BUILD_TESTING)
subdirs(tests)
endif(BUILD_TESTING)

