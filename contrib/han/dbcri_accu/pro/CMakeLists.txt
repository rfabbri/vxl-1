# This is 
# Brown xxxxxxx Processing library
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_OXL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/mvg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR} )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye )

SET(dbcri_accu_pro_sources
   dbcri_accu_process.cxx   dbcri_accu_process.h
   
)

#AUX_SOURCE_DIRECTORY(Templates dbecl_pro_sources)

ADD_LIBRARY(dbcri_accu_pro ${dbcri_accu_pro_sources})

TARGET_LINK_LIBRARIES(dbcri_accu_pro  vidpro dbecl bmrf vgl vgl_algo brip dtube)


IF(BUILD_TESTING)
#  SUBDIRS(tests)
ENDIF(BUILD_TESTING)

