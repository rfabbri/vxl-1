# contrib/brl/bseg/bvxm_batch/CMakeLists.txt
INCLUDE( ${VXL_CMAKE_DIR}/NewCMake/FindPython.cmake )
IF( PYTHON_FOUND )
  IF( VXL_EXPAT_FOUND )
    INCLUDE_DIRECTORIES( ${PYTHON_INCLUDE_PATH} )
    INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR} )
    INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bpro )
    INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bpro/core )

    INCLUDE_DIRECTORIES( ${VXL_CORE_INCLUDE_DIR} )
    INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc )
    INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc/vpgl )
    INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/dec )
  
     SET(psm_batch_sources
       reg_psm.h reg_psm.cxx
     )
 

    ADD_LIBRARY(psm_batch SHARED ${psm_batch_sources})

    #library has to have different name depending on debug or release version.

    SET_TARGET_PROPERTIES(psm_batch PROPERTIES OUTPUT_NAME psm_batch DEBUG_POSTFIX _d SUFFIX .pyd)

    TARGET_LINK_LIBRARIES(psm_batch bprb brdb bpro_batch vbl psm psm_pro psm_algo_pro psm_opt_pro psm_camera_opt_pro psm_mfa_pro psm_mfa psm_io vpgl_io vpgl_pro vil_io vil_pro)
  
    IF (PYTHON_DEBUG_LIBRARIES)
      TARGET_LINK_LIBRARIES( psm_batch debug ${PYTHON_DEBUG_LIBRARY})
    ENDIF(PYTHON_DEBUG_LIBRARIES )

    IF(PYTHON_LIBRARIES)
      TARGET_LINK_LIBRARIES( psm_batch optimized ${PYTHON_LIBRARY})
    ENDIF(PYTHON_LIBRARIES)
  ENDIF( VXL_EXPAT_FOUND ) 
ENDIF( PYTHON_FOUND )
