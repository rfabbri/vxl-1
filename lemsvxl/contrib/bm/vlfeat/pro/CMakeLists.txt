# 
# This is a processes library which calls methods from the vl feature library
# Author: Brandon A. Mayer 2/28/2011
#

if( VL_FEAT_FOUND )

  include_directories( ${VXL_BRL_INCLUDE_DIR}/bpro )
  include_directories( ${VXL_BRL_INCLUDE_DIR}/bpro/bprb )
  include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )

  set( VLFEAT_PRO_SOURCES
       vlfeat_processes.h
       vlfeat_register.h            vlfeat_register.cxx )
       
  aux_source_directory( processes VLFEAT_PRO_SOURCES )
  add_library( vlfeat_pro ${VLFEAT_PRO_SOURCES} )
  
endif( VL_FEAT_FOUND )