# This is brl/blem/brcv/mvg/dbirl/pro/CMakeLists.txt

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )
INCLUDE_DIRECTORIES(${VXL_GEL_INCLUDE_DIR} )

SET(dbirl_pro_sources
    dbirl_h_computation_process.cxx       dbirl_h_computation_process.h
)


ADD_LIBRARY(dbirl_pro ${dbirl_pro_sources})

TARGET_LINK_LIBRARIES(dbirl_pro dbirl dbinfo vidpro1)


IF(BUILD_TESTING)
#  SUBDIRS(tests)
ENDIF(BUILD_TESTING)

