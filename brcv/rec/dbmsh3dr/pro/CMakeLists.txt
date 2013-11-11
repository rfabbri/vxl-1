# rec/dbmsh3dr/pro/CMakeLists.txt

# processing packaged for dbmsh3dr


#INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
#INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
#INCLUDE_DIRECTORIES( ${VXL_RPL_INCLUDE_DIR} )
#INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp)
#INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/rec)

SET(dbmsh3dr_pro_sources

dbmsh3dr_cmdpara.cxx        dbmsh3dr_cmdpara.h
dbmsh3dr_process.cxx        dbmsh3dr_process.h


)

ADD_LIBRARY(dbmsh3dr_pro  ${dbmsh3dr_pro_sources})

# add the required libraries into this list
TARGET_LINK_LIBRARIES( dbmsh3dr_pro dbmsh3dr dbgl rgrl dbasn ) 


