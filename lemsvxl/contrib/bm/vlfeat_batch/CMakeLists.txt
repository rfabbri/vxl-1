#
# this is /contrib/bm/vlfeat_batch/CMakeLists.txt
# Batch Library for VL Feature library processes
# Author: Brandon A. Mayer
# 2/28/2011
#

include( ${VXL_CMAKE_DIR}/NewCMake/FindPython.cmake )

if( PYTHON_FOUND )
  include_directories( ${PYTHON_INCLUDE_DIRS} )
  include_directories( ${VXL_BRL_INCLUDE_DIR} )
  include_directories( ${VXL_BRL_INCLUDE_DIR}/bpro )
  include_directories( ${VXL_BRL_INCLUDE_DIR}/bpro/core )
  include_directories( ${VL_FEAT_VXL_SRC_DIR} )
  
  set( VLFEAT_BATCH_SOURCES
       reg_vlfeat.h         reg_vlfeat.cxx )
       
  add_library( vlfeat_batch SHARED ${VL_BATCH_SOURCES} )
  
  #library has to have different name depending on debug or release version.
  if(WIN32)
    set_target_properties( vlfeat_batch PROPERTIES OUTPUT_NAME vl_batch DEBUG_POSTFIX _d SUFFIX .pyd )
  endif(WIN32)
  
  if(APPLE)
    set_target_properties( vlfeat_batch PROPERTIES OUTPUT_NAME vl_batch SUFFIX .so )
  endif(APPLE)
  
  target_link_libraries( vlfeat_batch vlfeat_pro vlfeat )
  
endif( PYTHON_FOUND )