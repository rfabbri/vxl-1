#Brandon A. Mayer 1/13/11
#dsm's Process Library

#bprb depends on expat and dsm_pro depends of bprb primarily

if(VXL_EXPAT_FOUND)
  include_directories( ${VXL_EXPAT_INCLUDE_DIR} )
  include_directories( ${VXL_BRL_INCLUDE_DIR}/bpro )
  include_directories( ${VXL_BRL_INCLUDE_DIR}/bpro/bprb )
  include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
  include_directories( ${bm_SOURCE_DIR} )
  
  set( dsm_pro_sources
       dsm_processes.h
       dsm_register.h       dsm_register.cxx )
       
  aux_source_directory( Templates dsm_pro_sources )       
  aux_source_directory( processes dsm_pro_sources )
  
  add_library( dsm_pro ${dsm_pro_sources} )
  
  #target_link_libraries( dsm_pro dsm_io dsm vgl bsta bsta_pro bprb brdb vnl vil vidl)
  target_link_libraries( dsm_pro dsm dsm_io vgl bsta bsta_pro bprb brdb vnl vil vidl)
endif(VXL_EXPAT_FOUND)
