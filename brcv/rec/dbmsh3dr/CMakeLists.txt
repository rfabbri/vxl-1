# rec/dbmshdr/CMakeLists.txt

# main package for mesh registration algorithms.

include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_RPL_INCLUDE_DIR} )
include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${lems_SOURCE_DIR}/brcv/shp)
include_directories( ${lems_SOURCE_DIR}/brcv/rec)

set(dbmsh3dr_sources

dbmsh3dr_match.cxx          dbmsh3dr_match.h
dbmsh3dr_icp.cxx            
dbmsh3dr_icp_rgrl.cxx            
dbmsh3dr_error_estim.cxx
dbmsh3dr_scan_fuse.cxx

)

add_library(dbmsh3dr  ${dbmsh3dr_sources})

# add the required libraries into this list
target_link_libraries( dbmsh3dr dbmsh3d dbmsh3d_algo dbgl rgrl )  

subdirs( vis )

subdirs( pro )

if(BUILD_TESTING)
#subdirs(tests)
endif(BUILD_TESTING)



