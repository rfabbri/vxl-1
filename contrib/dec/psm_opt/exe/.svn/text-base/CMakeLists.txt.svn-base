INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc/vpgl )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/dec )

SET(psm_opt_exe_sources
  psm_opt.cxx
  )

SET(psm_opt_bayes_exe_sources
  psm_opt_bayes.cxx
  )


ADD_EXECUTABLE( psm_opt_exe ${psm_opt_exe_sources})

ADD_EXECUTABLE( psm_opt_bayes_exe ${psm_opt_bayes_exe_sources})

TARGET_LINK_LIBRARIES( psm_opt_exe psm_opt psm psm_algo hsds vbl vul vgl vnl vnl_algo vpgl)

TARGET_LINK_LIBRARIES( psm_opt_bayes_exe psm_opt psm psm_algo hsds vbl vul vgl vnl vnl_algo vpgl)


