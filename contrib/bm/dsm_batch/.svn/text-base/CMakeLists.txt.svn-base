# /contrib/bm/dsm_batch/CmakeLists.txt
#Brandon A. Mayer dsm_batch library
#1/13/11

PROJECT(dsm_batch)

INCLUDE( ${VXL_CMAKE_DIR}/NewCMake/FindPython.cmake )

IF( PYTHON_FOUND )
  INCLUDE_DIRECTORIES( ${PYTHON_INCLUDE_DIRS} )
  INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR} )
  INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bpro )
  INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bpro/core )
  INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas ) #for bapl
  INCLUDE_DIRECTORIES( ${bm_SOURCE_DIR})
  
  SET( dsm_batch_sources 
       reg_dsm.h            reg_dsm.cxx 
      )
      
ADD_LIBRARY( dsm_batch SHARED ${dsm_batch_sources})
      
  #library has to have different name depending on debug or release version.
  IF(WIN32)
    SET_TARGET_PROPERTIES( dsm_batch PROPERTIES OUTPUT_NAME 
      dsm_batch DEBUG_POSTFIX _d SUFFIX .pyd )
  ENDIF(WIN32)
    
  IF(APPLE)
    SET_TARGET_PROPERTIES( dsm_batch PROPERTIES OUTPUT_NAME dsm_batch 
      SUFFIX .so )
  ENDIF(APPLE)

  IF(NOT WIN32)
    SET_TARGET_PROPERTIES( dsm_batch PROPERTIES COMPILE_FLAGS "-fPIC" )
  ENDIF(NOT WIN32)

  SET_TARGET_PROPERTIES(dsm_batch PROPERTIES PREFIX "")
  
  TARGET_LINK_LIBRARIES( dsm_batch dsm_pro dsm vbl vsl bpro_batch 
    vil vil_pro vil_io vgl_io bsta_pro bapl_pro bprb vidl )
  IF( PYTHON_DEBUG_LIBRARIES )
    TARGET_LINK_LIBRARIES( dsm_batch debug ${PYTHON_DEBUG_LIBRARY} )
    #MESSAGE( "PY DEBUG = " ${PYTHON_DEBUG_LIBRARY})
  ENDIF( PYTHON_DEBUG_LIBRARIES )
  
  IF( PYTHON_LIBRARIES )
    TARGET_LINK_LIBRARIES( dsm_batch optimized ${PYTHON_LIBRARY} )
  ENDIF( PYTHON_LIBRARIES )
  
ENDIF( PYTHON_FOUND )



