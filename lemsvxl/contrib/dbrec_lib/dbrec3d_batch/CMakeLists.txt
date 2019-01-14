# lemsvxlsrc/contrib/dbrec3d/dbrec3d_batch/CMakeLists.txt
project(dbrec3d_batch)

include( ${VXL_CMAKE_DIR}/NewCMake/FindPython.cmake )
if( PYTHON_FOUND )
  #include( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
  #if( EXPAT_FOUND )
    include_directories( ${PYTHON_INCLUDE_PATH} )
    include_directories( ${VXL_BRL_INCLUDE_DIR} )   
    include_directories( ${VXLCORE_INCLUDE_DIR} )
    include_directories( ${LEMS_SOURCE_DIR}/contrib/dbrec_lib)
      
    set(dbrec3d_batch_sources
      reg_dbrec3d.cxx          reg_dbrec3d.h
    )
     
    add_library(dbrec3d_batch MODULE ${dbrec3d_batch_sources}) 

    if(WIN32)
      #library has to have different name depending on debug or release version.
      set_target_properties(dbrec3d_batch PROPERTIES OUTPUT_NAME dbrec3d_batch DEBUG_POSTFIX _d SUFFIX .pyd)
    endif(WIN32)
    set_target_properties(dbrec3d_batch PROPERTIES PREFIX "")

    target_link_libraries(dbrec3d_batch bprb brdb bpro_batch dbrec3d_pcl_pro bof_pro dbrec3d_pcl bvpl_octree_pro bvpl_kernels_pro boxm boxm_pro boxm_algo_pro bvxm_grid_pro vgl_io  ) 
    #target_link_libraries(dbrec3d_batch bprb brdb bpro_batch dbrec3d_pro bsvg_pro bvpl_octree_pro bvpl_kernels_pro boxm boxm_pro boxm_algo_pro bvxm_grid_pro bvxm_grid_io bvxm_algo_pro vgl_io ) 
  
    if (PYTHON_DEBUG_LIBRARIES)
      target_link_libraries( dbrec3d_batch debug ${PYTHON_DEBUG_LIBRARY})
    endif(PYTHON_DEBUG_LIBRARIES )

    if(PYTHON_LIBRARIES)
      target_link_libraries( dbrec3d_batch optimized ${PYTHON_LIBRARY})
    endif(PYTHON_LIBRARIES)
  #endif( EXPAT_FOUND ) 
endif( PYTHON_FOUND )

