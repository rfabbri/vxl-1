# contrib/brl/bseg/bvxm_batch/CMakeLists.txt
include( ${VXL_CMAKE_DIR}/NewCMake/FindPython.cmake )
if( PYTHON_FOUND )
  if( VXL_EXPAT_FOUND )
    include_directories( ${PYTHON_INCLUDE_PATH} )
    include_directories( ${VXL_BRL_INCLUDE_DIR} )
    include_directories( ${VXL_BRL_INCLUDE_DIR}/bpro )
    include_directories( ${VXL_BRL_INCLUDE_DIR}/bpro/core )

    include_directories( ${VXL_CORE_INCLUDE_DIR} )
    include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc )
    include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc/vpgl )
    include_directories( ${lems_SOURCE_DIR}/contrib/dec )
  
     set(psm_batch_sources
       reg_psm.h reg_psm.cxx
     )
 

    add_library(psm_batch SHARED ${psm_batch_sources})

    #library has to have different name depending on debug or release version.

    set_target_properties(psm_batch PROPERTIES OUTPUT_NAME psm_batch DEBUG_POSTFIX _d SUFFIX .pyd)

    target_link_libraries(psm_batch bprb brdb bpro_batch vbl psm psm_pro psm_algo_pro psm_opt_pro psm_camera_opt_pro psm_mfa_pro psm_mfa psm_io vpgl_io vpgl_pro vil_io vil_pro)
  
    if (PYTHON_DEBUG_LIBRARIES)
      target_link_libraries( psm_batch debug ${PYTHON_DEBUG_LIBRARY})
    endif(PYTHON_DEBUG_LIBRARIES )

    if(PYTHON_LIBRARIES)
      target_link_libraries( psm_batch optimized ${PYTHON_LIBRARY})
    endif(PYTHON_LIBRARIES)
  endif( VXL_EXPAT_FOUND ) 
endif( PYTHON_FOUND )
