# 
# This is a processes library which calls methods from the vl feature library
# Author: Brandon A. Mayer 2/28/2011
#

IF( VL_FEAT_FOUND )

  INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bpro )
  INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bpro/bprb )
  INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR} )

  SET( VLFEAT_PRO_SOURCES
       vlfeat_processes.h
       vlfeat_register.h            vlfeat_register.cxx )
       
  AUX_SOURCE_DIRECTORY( processes VLFEAT_PRO_SOURCES )
  ADD_LIBRARY( vlfeat_pro ${VLFEAT_PRO_SOURCES} )
  
ENDIF( VL_FEAT_FOUND )