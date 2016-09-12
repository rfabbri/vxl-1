include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc )
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc/vpgl )
include_directories( ${lems_SOURCE_DIR}/contrib/dec )

set(psm_mfa_sources
   psm_mfa_compute_factor_properties.h
   psm_mfa_optimizer.h  psm_mfa_optimizer.txx
 )

aux_source_directory(Templates psm_mfa_sources)

add_library( psm_mfa ${psm_mfa_sources})

target_link_libraries( psm_mfa psm psm_algo hsds vbl vul vgl vnl vnl_algo vpgl)

add_subdirectory(pro)

