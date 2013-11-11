# rec/dbskr/pro/CMakeLists.txt

# processing packaged for dbskr


#INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
#INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_RPL_INCLUDE_DIR} )
#INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp)

SET(dbsk3dr_pro_sources

dbsk3dr_pro_cmdpara.cxx    dbsk3dr_pro_cmdpara.h
dbsk3dr_process.cxx        dbsk3dr_process.h


)

ADD_LIBRARY(dbsk3dr_pro  ${dbsk3dr_pro_sources})

# add the required libraries into this list
TARGET_LINK_LIBRARIES( dbsk3dr_pro dbsk3dr dbasn ) 


