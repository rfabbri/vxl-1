# lemsvxlsrc/contrib/dbrec/dbrec_batch/CMakeLists.txt
include( ${VXL_CMAKE_DIR}/NewCMake/FindPython.cmake )
if( PYTHON_FOUND )
  #include( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
  #if( EXPAT_FOUND )
    include_directories( ${PYTHON_INCLUDE_PATH} )
    include_directories( ${VXL_BRL_INCLUDE_DIR} )
    include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
    include_directories( ${VXL_BRL_INCLUDE_DIR}/bpro )
    include_directories( ${VXL_BRL_INCLUDE_DIR}/bpro/core )
        
    include_directories( ${VXLCORE_INCLUDE_DIR} )
    include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc )
    include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc/vpgl )
    include_directories( ${VXL_MUL_INCLUDE_DIR} )
    include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg/brec )
    include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg/bvxm )

    include_directories( ${lems_SOURCE_DIR}/contrib/dbrec_lib )
    include_directories( ${lems_SOURCE_DIR}/basic )
    include_directories( ${lems_SOURCE_DIR}/brcv/rec )
      
    set(dbrec_batch_sources
      reg_dbrec.cxx          reg_dbrec.h
    )
     
    add_library(dbrec_batch SHARED ${dbrec_batch_sources}) 

    if(WIN32)
      #library has to have different name depending on debug or release version.
      set_target_properties(dbrec_batch PROPERTIES OUTPUT_NAME dbrec_batch DEBUG_POSTFIX _d SUFFIX .pyd)
    endif(WIN32)
    set_target_properties(dbrec_batch PROPERTIES PREFIX "")

    target_link_libraries(dbrec_batch bprb brdb bpro_batch bmdl_pro bvxm_pro bvxm_io bvxm_grid_io brec_pro dbrec_pro dbrec_mic_pro dbfs_pro dborl_pro bsvg_pro bbgm_pro bvgl_pro brip_pro vbl vpgl_io vidl_pro vpgl_pro vil_io vil_pro) 
      
    if (PYTHON_DEBUG_LIBRARIES)
      target_link_libraries( dbrec_batch debug ${PYTHON_DEBUG_LIBRARY})
    endif(PYTHON_DEBUG_LIBRARIES )

    if(PYTHON_LIBRARIES)
      target_link_libraries( dbrec_batch optimized ${PYTHON_LIBRARY})
    endif(PYTHON_LIBRARIES)
  #endif( EXPAT_FOUND ) 
endif( PYTHON_FOUND )

#include( ${MODULE_PATH}/NewCMake/FindDirectShow.cmake )
#if(DIRECTSHOW_FOUND)
#  add_definitions(-DHAS_DIRECTSHOW)
#endif(DIRECTSHOW_FOUND)
