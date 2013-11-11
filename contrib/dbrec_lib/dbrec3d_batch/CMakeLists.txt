# lemsvxlsrc/contrib/dbrec3d/dbrec3d_batch/CMakeLists.txt
PROJECT(dbrec3d_batch)

INCLUDE( ${VXL_CMAKE_DIR}/NewCMake/FindPython.cmake )
IF( PYTHON_FOUND )
  #INCLUDE( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
  #IF( EXPAT_FOUND )
    INCLUDE_DIRECTORIES( ${PYTHON_INCLUDE_PATH} )
    INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR} )   
    INCLUDE_DIRECTORIES( ${VXLCORE_INCLUDE_DIR} )
    INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/dbrec_lib)
      
    SET(dbrec3d_batch_sources
      reg_dbrec3d.cxx          reg_dbrec3d.h
    )
     
    ADD_LIBRARY(dbrec3d_batch MODULE ${dbrec3d_batch_sources}) 

    IF(WIN32)
      #library has to have different name depending on debug or release version.
      SET_TARGET_PROPERTIES(dbrec3d_batch PROPERTIES OUTPUT_NAME dbrec3d_batch DEBUG_POSTFIX _d SUFFIX .pyd)
    ENDIF(WIN32)
    SET_TARGET_PROPERTIES(dbrec3d_batch PROPERTIES PREFIX "")

    TARGET_LINK_LIBRARIES(dbrec3d_batch bprb brdb bpro_batch dbrec3d_pcl_pro bof_pro dbrec3d_pcl bvpl_octree_pro bvpl_kernels_pro boxm boxm_pro boxm_algo_pro bvxm_grid_pro vgl_io  ) 
    #TARGET_LINK_LIBRARIES(dbrec3d_batch bprb brdb bpro_batch dbrec3d_pro bsvg_pro bvpl_octree_pro bvpl_kernels_pro boxm boxm_pro boxm_algo_pro bvxm_grid_pro bvxm_grid_io bvxm_algo_pro vgl_io ) 
  
    IF (PYTHON_DEBUG_LIBRARIES)
      TARGET_LINK_LIBRARIES( dbrec3d_batch debug ${PYTHON_DEBUG_LIBRARY})
    ENDIF(PYTHON_DEBUG_LIBRARIES )

    IF(PYTHON_LIBRARIES)
      TARGET_LINK_LIBRARIES( dbrec3d_batch optimized ${PYTHON_LIBRARY})
    ENDIF(PYTHON_LIBRARIES)
  #ENDIF( EXPAT_FOUND ) 
ENDIF( PYTHON_FOUND )

