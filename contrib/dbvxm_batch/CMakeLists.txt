INCLUDE_DIRECTORIES( ${VXL_PYTHON_INCLUDE_PATH} )
IF (WIN32)
  INCLUDE_DIRECTORIES( ${VXL_PYTHON_PC_INCLUDE_PATH} )
ENDIF(WIN32)
  INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR} )
  INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bpro )
  INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bpro/core )

  INCLUDE_DIRECTORIES( ${VXL_CORE_INCLUDE_DIR} )
  INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc )
  INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
  INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc/vpgl )
  INCLUDE_DIRECTORIES(${VXL_MUL_INCLUDE_DIR} )
  INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib )

  SET(dbvxm_batch_sources      
          batch_dbvxm.h batch_dbvxm.cxx
      reg_dbvxm.cxx
   )
 

  ADD_LIBRARY(dbvxm_batch SHARED ${dbvxm_batch_sources})

  #library has to have different name depending on debug or release version.

  SET_TARGET_PROPERTIES(dbvxm_batch PROPERTIES OUTPUT_NAME dbvxm_batch DEBUG_POSTFIX _d SUFFIX .pyd)

  TARGET_LINK_LIBRARIES(dbvxm_batch dbvxm_pro dbvxm_rec_pro bprb brdb vbl bvxm bvxm_pro bvxm_io vpgl_io vpgl_pro bvgl_pro vil_io vil_pro debug ${VXL_PYTHON_DEBUG_LIBRARY} optimized ${VXL_PYTHON_LIBRARY})

  IF (PYTHON_DEBUG_LIBRARIES)
    TARGET_LINK_LIBRARIES( dbvxm_batch debug ${PYTHON_DEBUG_LIBRARY})
  ENDIF(PYTHON_DEBUG_LIBRARIES )

  IF(PYTHON_LIBRARIES)
    TARGET_LINK_LIBRARIES( dbvxm_batch optimized ${PYTHON_LIBRARY})
  ENDIF(PYTHON_LIBRARIES)

#old cmakelist file
# dbvxm_batch/CMakeLists.txt
#INCLUDE( ${VXL_CMAKE_DIR}/NewCMake/FindPython.cmake )
#IF( PYTHON_FOUND )
#  #INCLUDE( ${VXL_BRL_INCLUDE_DIR}/bmods/FindEXPAT.cmake )
#  #IF( EXPAT_FOUND )
#    INCLUDE_DIRECTORIES( ${PYTHON_INCLUDE_PATH} )
#    INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR} )
#    INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bpro )
#    INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bpro/core )
#
#    INCLUDE_DIRECTORIES( ${VXL_CORE_INCLUDE_DIR} )
#    INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc )
#    INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc/vpgl )
#    INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR} )
#
#    INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR})
#
#    INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/ )
#    INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/dbvxm )
#
#    SET(dbvxm_batch_sources
#      batch_dbvxm.h batch_dbvxm.cxx
#      reg_dbvxm.cxx
#    )
# 
#
#    ADD_LIBRARY(dbvxm_batch SHARED ${dbvxm_batch_sources})
#
#    #library has to have different name depending on debug or release version.
#
#    SET_TARGET_PROPERTIES(dbvxm_batch PROPERTIES OUTPUT_NAME dbvxm_batch DEBUG_POSTFIX _d SUFFIX .pyd)
#
#    TARGET_LINK_LIBRARIES(dbvxm_batch bprb brdb vbl bvxm_pro dbvxm_pro bvxm_io vpgl_io vpgl_pro vil_io vil_pro debug ${PYTHON_DEBUG_LIBRARY} optimized ${PYTHON_LIBRARY})
#  
#    IF (PYTHON_DEBUG_LIBRARIES)
#      TARGET_LINK_LIBRARIES( dbvxm_batch debug ${PYTHON_DEBUG_LIBRARY})
#    ENDIF(PYTHON_DEBUG_LIBRARIES )
#
#    IF(PYTHON_LIBRARIES)
#      TARGET_LINK_LIBRARIES( dbvxm_batch optimized ${PYTHON_LIBRARY})
#    ENDIF(PYTHON_LIBRARIES)
#
#  #ENDIF( EXPAT_FOUND )
#ENDIF( PYTHON_FOUND )
