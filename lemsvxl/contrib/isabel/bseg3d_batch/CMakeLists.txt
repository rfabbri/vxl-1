# contrib/brl/bseg/bvxm_batch/CMakeLists.txt
include( ${VXL_CMAKE_DIR}/NewCMake/FindPython.cmake )
if( PYTHON_FOUND )
  if( VXL_EXPAT_FOUND )
    include_directories( ${PYTHON_INCLUDE_PATH} )
    include_directories( ${VXL_BRL_INCLUDE_DIR} )
    include_directories( ${VXL_BRL_INCLUDE_DIR}/bpro )
    include_directories( ${VXL_BRL_INCLUDE_DIR}/bpro/core )
    include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
    include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg/bvxm )

    include_directories( ${VXL_CORE_INCLUDE_DIR} )
    include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc )
    include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc/vpgl )
    include_directories( ${LEMS_SOURCE_DIR}/contrib/isabel )
  
     set(bseg3d_batch_sources
       reg_bseg3d.h reg_bseg3d.cxx
     )
 

    add_library(bseg3d_batch SHARED ${bseg3d_batch_sources})

    #library has to have different name depending on debug or release version.
    if(WIN32)
      set_target_properties(bseg3d_batch PROPERTIES OUTPUT_NAME bseg3d_batch DEBUG_POSTFIX _d SUFFIX .pyd)
    endif(WIN32)
    set_target_properties(bseg3d_batch PROPERTIES PREFIX "")

    target_link_libraries(bseg3d_batch bseg3d bseg3d_pro bseg3d_algo bseg3d_algo_pro bprb brdb bpro_batch bvxm_pro vil_pro vpgl_pro )
  
    if (PYTHON_DEBUG_LIBRARIES)
      target_link_libraries( bseg3d_batch debug ${PYTHON_DEBUG_LIBRARY})
    endif(PYTHON_DEBUG_LIBRARIES )

    if(PYTHON_LIBRARIES)
      target_link_libraries( bseg3d_batch optimized ${PYTHON_LIBRARY})
    endif(PYTHON_LIBRARIES)
  endif( VXL_EXPAT_FOUND ) 
endif( PYTHON_FOUND )
