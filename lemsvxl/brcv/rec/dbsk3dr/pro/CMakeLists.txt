# rec/dbskr/pro/CMakeLists.txt

# processing packaged for dbskr


#include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
#include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_RPL_INCLUDE_DIR} )
#include_directories( ${LEMS_SOURCE_DIR}/brcv/shp)

set(dbsk3dr_pro_sources

dbsk3dr_pro_cmdpara.cxx    dbsk3dr_pro_cmdpara.h
dbsk3dr_process.cxx        dbsk3dr_process.h


)

add_library(dbsk3dr_pro  ${dbsk3dr_pro_sources})

# add the required libraries into this list
target_link_libraries( dbsk3dr_pro dbsk3dr dbasn ) 


