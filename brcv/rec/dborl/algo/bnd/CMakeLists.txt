INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/db3p )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/db3p/expatpp/expat/lib )

SET( dborl_bnd_generic_linker_sources
  dborl_bnd_generic_params.h 
  dborl_bnd_generic_params_sptr.h
  dborl_bnd_generic_main.cxx
  vbl_smart_ptr+dborl_bnd_generic_params-.cxx
)

ADD_EXECUTABLE (dborl_bnd_generic ${dborl_bnd_generic_linker_sources})
TARGET_LINK_LIBRARIES (dborl_bnd_generic dborl_algo dbdet_pro)



