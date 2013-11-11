# lemsvxlsrc/contrib/dbrec/dbrec_batch/CMakeLists.txt
INCLUDE( ${VXL_CMAKE_DIR}/NewCMake/FindPython.cmake )
IF( PYTHON_FOUND )
  #INCLUDE( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
  #IF( EXPAT_FOUND )
    INCLUDE_DIRECTORIES( ${PYTHON_INCLUDE_PATH} )
    INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR} )
    INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
    INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bpro )
    INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bpro/core )
        
    INCLUDE_DIRECTORIES( ${VXLCORE_INCLUDE_DIR} )
    INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc )
    INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc/vpgl )
    INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR} )
    INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg/brec )
    INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg/bvxm )

    INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/dbrec_lib )
    INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )
    INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/rec )
      
    SET(dbrec_batch_sources
      reg_dbrec.cxx          reg_dbrec.h
    )
     
    ADD_LIBRARY(dbrec_batch SHARED ${dbrec_batch_sources}) 

    IF(WIN32)
      #library has to have different name depending on debug or release version.
      SET_TARGET_PROPERTIES(dbrec_batch PROPERTIES OUTPUT_NAME dbrec_batch DEBUG_POSTFIX _d SUFFIX .pyd)
    ENDIF(WIN32)
    SET_TARGET_PROPERTIES(dbrec_batch PROPERTIES PREFIX "")

    TARGET_LINK_LIBRARIES(dbrec_batch bprb brdb bpro_batch bmdl_pro bvxm_pro bvxm_io bvxm_grid_io brec_pro dbrec_pro dbrec_mic_pro dbfs_pro dborl_pro bsvg_pro bbgm_pro bvgl_pro brip_pro vbl vpgl_io vidl_pro vpgl_pro vil_io vil_pro) 
      
    IF (PYTHON_DEBUG_LIBRARIES)
      TARGET_LINK_LIBRARIES( dbrec_batch debug ${PYTHON_DEBUG_LIBRARY})
    ENDIF(PYTHON_DEBUG_LIBRARIES )

    IF(PYTHON_LIBRARIES)
      TARGET_LINK_LIBRARIES( dbrec_batch optimized ${PYTHON_LIBRARY})
    ENDIF(PYTHON_LIBRARIES)
  #ENDIF( EXPAT_FOUND ) 
ENDIF( PYTHON_FOUND )

#INCLUDE( ${MODULE_PATH}/NewCMake/FindDirectShow.cmake )
#IF(DIRECTSHOW_FOUND)
#  ADD_DEFINITIONS(-DHAS_DIRECTSHOW)
#ENDIF(DIRECTSHOW_FOUND)
