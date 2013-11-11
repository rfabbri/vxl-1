#Brandon A. Mayer 1/13/11
#dsm's Process Library

#bprb depends on expat and dsm_pro depends of bprb primarily

IF(VXL_EXPAT_FOUND)
  INCLUDE_DIRECTORIES( ${VXL_EXPAT_INCLUDE_DIR} )
  INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bpro )
  INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bpro/bprb )
  INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
  INCLUDE_DIRECTORIES( ${bm_SOURCE_DIR} )
  
  SET( dsm_pro_sources
       dsm_processes.h
       dsm_register.h       dsm_register.cxx )
       
  AUX_SOURCE_DIRECTORY( Templates dsm_pro_sources )       
  AUX_SOURCE_DIRECTORY( processes dsm_pro_sources )
  
  ADD_LIBRARY( dsm_pro ${dsm_pro_sources} )
  
  #TARGET_LINK_LIBRARIES( dsm_pro dsm_io dsm vgl bsta bsta_pro bprb brdb vnl vil vidl)
  TARGET_LINK_LIBRARIES( dsm_pro dsm dsm_io vgl bsta bsta_pro bprb brdb vnl vil vidl)
ENDIF(VXL_EXPAT_FOUND)
