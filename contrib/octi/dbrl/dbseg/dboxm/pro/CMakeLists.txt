# boxm's Process Library

# bprb depends on expat and bvxm_prodepends on bprb primarily
include( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
if(EXPAT_FOUND)
  include_directories( ${BRL_INCLUDE_DIR}/bpro )
  include_directories( ${BRL_INCLUDE_DIR}/bpro/bprb )
  include_directories( ${BRL_INCLUDE_DIR}/bseg )
  include_directories( ${GEL_INCLUDE_DIR}/mrc/vpgl )
  include_directories( ${GEL_INCLUDE_DIR}/mrc )
 if(lems_SOURCE_DIR)
  include_directories( ${lems_SOURCE_DIR}/contrib/dec )
 endif(lems_SOURCE_DIR)

  set(boxm_pro_sources
      boxm_processes.h
      boxm_register.h   boxm_register.cxx
  )

  aux_source_directory(Templates boxm_pro_sources)
  aux_source_directory(processes boxm_pro_sources)

  add_library(boxm_pro ${boxm_pro_sources})

  target_link_libraries(boxm_pro boxm_io bprb brdb brip vsol)
endif(EXPAT_FOUND)
