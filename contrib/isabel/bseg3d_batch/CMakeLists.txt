# contrib/brl/bseg/bvxm_batch/CMakeLists.txt
INCLUDE( ${VXL_CMAKE_DIR}/NewCMake/FindPython.cmake )
IF( PYTHON_FOUND )
  IF( VXL_EXPAT_FOUND )
    INCLUDE_DIRECTORIES( ${PYTHON_INCLUDE_PATH} )
    INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR} )
    INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bpro )
    INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bpro/core )
    INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
    INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg/bvxm )

    INCLUDE_DIRECTORIES( ${VXL_CORE_INCLUDE_DIR} )
    INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc )
    INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc/vpgl )
    INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/isabel )
  
     SET(bseg3d_batch_sources
       reg_bseg3d.h reg_bseg3d.cxx
     )
 

    ADD_LIBRARY(bseg3d_batch SHARED ${bseg3d_batch_sources})

    #library has to have different name depending on debug or release version.
    IF(WIN32)
      SET_TARGET_PROPERTIES(bseg3d_batch PROPERTIES OUTPUT_NAME bseg3d_batch DEBUG_POSTFIX _d SUFFIX .pyd)
    ENDIF(WIN32)
    SET_TARGET_PROPERTIES(bseg3d_batch PROPERTIES PREFIX "")

    TARGET_LINK_LIBRARIES(bseg3d_batch bseg3d bseg3d_pro bseg3d_algo bseg3d_algo_pro bprb brdb bpro_batch bvxm_pro vil_pro vpgl_pro )
  
    IF (PYTHON_DEBUG_LIBRARIES)
      TARGET_LINK_LIBRARIES( bseg3d_batch debug ${PYTHON_DEBUG_LIBRARY})
    ENDIF(PYTHON_DEBUG_LIBRARIES )

    IF(PYTHON_LIBRARIES)
      TARGET_LINK_LIBRARIES( bseg3d_batch optimized ${PYTHON_LIBRARY})
    ENDIF(PYTHON_LIBRARIES)
  ENDIF( VXL_EXPAT_FOUND ) 
ENDIF( PYTHON_FOUND )
