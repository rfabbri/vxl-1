# rec/dbmsh3dr/pro/CMakeLists.txt

# processing packaged for dbmsh3dr


#include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
#include_directories( ${VXL_GEL_INCLUDE_DIR} )
#include_directories( ${VXL_RPL_INCLUDE_DIR} )
#include_directories( ${lems_SOURCE_DIR}/brcv/shp)
#include_directories( ${lems_SOURCE_DIR}/brcv/rec)

set(dbmsh3dr_pro_sources

dbmsh3dr_cmdpara.cxx        dbmsh3dr_cmdpara.h
dbmsh3dr_process.cxx        dbmsh3dr_process.h


)

add_library(dbmsh3dr_pro  ${dbmsh3dr_pro_sources})

# add the required libraries into this list
target_link_libraries( dbmsh3dr_pro dbmsh3dr dbmsh3d_pro bgld rgrl dbasn ) 


