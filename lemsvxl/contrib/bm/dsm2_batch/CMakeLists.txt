# this is /contrib/bm/dsm2_batch/CMakeLists.txt
# Brandon A. Mayer dsm2_batch library
# 9/25/11

project(dsm2_batch)

include( ${VXL_CMAKE_DIR}/NewCMake/FindPython.cmake )

if( PYTHON_FOUND )
  include_directories( ${PYTHON_INCLUDE_DIRS} )
  include_directories( ${VXL_BRL_INCLUDE_DIR} )
  include_directories( ${VXL_BRL_INCLUDE_DIR}/bpro )
  include_directories( ${VXL_BRL_INCLUDE_DIR}/bpro/core )
  include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas ) #for bapl
  include_directories( ${bm_SOURCE_DIR})
  
  set( dsm2_batch_sources
       reg_dsm2.h               reg_dsm2.cxx
     )
  add_library( dsm2_batch SHARED ${dsm2_batch_sources} )

  #library has to have different name depending on debug or release version.
  if(WIN32)
    set_target_properties( dsm2_batch PROPERTIES OUTPUT_NAME 
      dsm2_batch DEBUG_POSTFIX _d SUFFIX .pyd )
  endif(WIN32)

  if(APPLE)
    set_target_properties( dsm2_batch PROPERTIES OUTPUT_NAME dsm2_batch
      SUFFIX .so )
  endif(APPLE)
  
  if( NOT WIN32 )
    set_target_properties( dsm_batch PROPERTIES COMPILE_FLAGS "-fPIC" )
  endif(NOT WIN32)  
  
  set_target_properties(dsm2_batch PROPERTIES PREFIX "")

  target_link_libraries( dsm2_batch dsm2 dsm2_algo dsm2_io dsm2_pro dts dts_algo dts_pro dts_io vbl vsl bpro_batch
    vil vil_pro vil_io vgl_io bsta_pro bapl_pro bprb vidl )

  if( PYTHON_DEBUG_LIBRARIES )
    target_link_libraries( dsm2_batch debug ${PYTHON_DEBUG_LIBRARY} )
  endif( PYTHON_DEBUG_LIBRARIES )

  if( PYTHON_LIBRARIES )
    target_link_libraries( dsm2_batch optimized ${PYTHON_LIBRARY} )
  endif( PYTHON_LIBRARIES )

 endif( PYTHON_FOUND )