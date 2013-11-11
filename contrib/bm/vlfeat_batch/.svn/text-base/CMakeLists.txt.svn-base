#
# this is /contrib/bm/vlfeat_batch/CMakeLists.txt
# Batch Library for VL Feature library processes
# Author: Brandon A. Mayer
# 2/28/2011
#

INCLUDE( ${VXL_CMAKE_DIR}/NewCMake/FindPython.cmake )

IF( PYTHON_FOUND )
  INCLUDE_DIRECTORIES( ${PYTHON_INCLUDE_DIRS} )
  INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR} )
  INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bpro )
  INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bpro/core )
  INCLUDE_DIRECTORIES( ${VL_FEAT_VXL_SRC_DIR} )
  
  SET( VLFEAT_BATCH_SOURCES
       reg_vlfeat.h         reg_vlfeat.cxx )
       
  ADD_LIBRARY( vlfeat_batch SHARED ${VL_BATCH_SOURCES} )
  
  #library has to have different name depending on debug or release version.
  IF(WIN32)
    SET_TARGET_PROPERTIES( vlfeat_batch PROPERTIES OUTPUT_NAME vl_batch DEBUG_POSTFIX _d SUFFIX .pyd )
  ENDIF(WIN32)
  
  IF(APPLE)
    SET_TARGET_PROPERTIES( vlfeat_batch PROPERTIES OUTPUT_NAME vl_batch SUFFIX .so )
  ENDIF(APPLE)
  
  TARGET_LINK_LIBRARIES( vlfeat_batch vlfeat_pro vlfeat )
  
ENDIF( PYTHON_FOUND )