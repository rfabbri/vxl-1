INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc/vpgl )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/dec )

SET(psm_mfa_sources
   psm_mfa_compute_factor_properties.h
   psm_mfa_optimizer.h  psm_mfa_optimizer.txx
 )

AUX_SOURCE_DIRECTORY(Templates psm_mfa_sources)

ADD_LIBRARY( psm_mfa ${psm_mfa_sources})

TARGET_LINK_LIBRARIES( psm_mfa psm psm_algo hsds vbl vul vgl vnl vnl_algo vpgl)

SUBDIRS(pro)

