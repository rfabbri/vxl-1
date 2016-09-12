# boxm_algo Process Library

# bprb depends on expat and this process library depends on bprb
include( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
if(EXPAT_FOUND)
  include_directories( ${BRL_INCLUDE_DIR}/bpro )
  include_directories( ${BRL_INCLUDE_DIR}/bpro/bprb )
  include_directories( ${BRL_INCLUDE_DIR}/bseg )

  set(boxm_algo_pro_sources
      boxm_algo_processes.h
      boxm_algo_register.h   boxm_algo_register.cxx
  )

  #aux_source_directory(Templates boxm_algo_pro_sources)
  aux_source_directory(processes boxm_algo_pro_sources)

  add_library(boxm_algo_pro ${boxm_algo_pro_sources} )

  target_link_libraries(boxm_algo_pro boxm_algo boxm_io bprb brdb)
endif(EXPAT_FOUND)
