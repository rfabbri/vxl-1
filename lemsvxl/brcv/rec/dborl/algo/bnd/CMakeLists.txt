include_directories( ${LEMS_SOURCE_DIR}/db3p )
include_directories( ${LEMS_SOURCE_DIR}/db3p/expatpp/expat/lib )

set( dborl_bnd_generic_linker_sources
  dborl_bnd_generic_params.h 
  dborl_bnd_generic_params_sptr.h
  dborl_bnd_generic_main.cxx
  vbl_smart_ptr+dborl_bnd_generic_params-.cxx
)

add_executable (dborl_bnd_generic ${dborl_bnd_generic_linker_sources})
target_link_libraries (dborl_bnd_generic dborl_algo dbdet_pro)



