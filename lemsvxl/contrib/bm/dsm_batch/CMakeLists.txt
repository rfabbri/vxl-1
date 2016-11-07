# /contrib/bm/dsm_batch/CmakeLists.txt
#Brandon A. Mayer dsm_batch library
#1/13/11

project(dsm_batch)

include( ${VXL_CMAKE_DIR}/NewCMake/FindPython.cmake )

if( PYTHON_FOUND )
  include_directories( ${PYTHON_INCLUDE_DIRS} )
  include_directories( ${VXL_BRL_INCLUDE_DIR} )
  include_directories( ${VXL_BRL_INCLUDE_DIR}/bpro )
  include_directories( ${VXL_BRL_INCLUDE_DIR}/bpro/core )
  include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas ) #for bapl
  include_directories( ${bm_SOURCE_DIR})
  
  set( dsm_batch_sources 
       reg_dsm.h            reg_dsm.cxx 
      )
      
add_library( dsm_batch SHARED ${dsm_batch_sources})
      
  #library has to have different name depending on debug or release version.
  if(WIN32)
    set_target_properties( dsm_batch PROPERTIES OUTPUT_NAME 
      dsm_batch DEBUG_POSTFIX _d SUFFIX .pyd )
  endif(WIN32)
    
  if(APPLE)
    set_target_properties( dsm_batch PROPERTIES OUTPUT_NAME dsm_batch 
      SUFFIX .so )
  endif(APPLE)

  if(NOT WIN32)
    set_target_properties( dsm_batch PROPERTIES COMPILE_FLAGS "-fPIC" )
  endif(NOT WIN32)

  set_target_properties(dsm_batch PROPERTIES PREFIX "")
  
  target_link_libraries( dsm_batch dsm_pro dsm vbl vsl bpro_batch 
    vil vil_pro vil_io vgl_io bsta_pro bapl_pro bprb vidl )
  if( PYTHON_DEBUG_LIBRARIES )
    target_link_libraries( dsm_batch debug ${PYTHON_DEBUG_LIBRARY} )
    #message( "PY DEBUG = " ${PYTHON_DEBUG_LIBRARY})
  endif( PYTHON_DEBUG_LIBRARIES )
  
  if( PYTHON_LIBRARIES )
    target_link_libraries( dsm_batch optimized ${PYTHON_LIBRARY} )
  endif( PYTHON_LIBRARIES )
  
endif( PYTHON_FOUND )



