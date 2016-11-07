include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc )
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc/vpgl )
include_directories( ${lems_SOURCE_DIR}/contrib/dec )

set(psm_opt_exe_sources
  psm_opt.cxx
  )

set(psm_opt_bayes_exe_sources
  psm_opt_bayes.cxx
  )


add_executable( psm_opt_exe ${psm_opt_exe_sources})

add_executable( psm_opt_bayes_exe ${psm_opt_bayes_exe_sources})

target_link_libraries( psm_opt_exe psm_opt psm psm_algo hsds vbl vul vgl vnl vnl_algo vpgl)

target_link_libraries( psm_opt_bayes_exe psm_opt psm psm_algo hsds vbl vul vgl vnl vnl_algo vpgl)


