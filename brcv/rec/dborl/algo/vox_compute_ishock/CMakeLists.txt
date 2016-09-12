include_directories( ${lems_SOURCE_DIR}/db3p )
# include_directories( ${lems_SOURCE_DIR}/db3p/expatpp/expat/lib )

#set( dborl_compute_ishock_sources
#  dborl_compute_ishock_params.h 
#  dborl_compute_ishock_params_sptr.h
#  dborl_compute_ishock_params.cxx
#  dborl_compute_ishock_main.cxx
#  vbl_smart_ptr+dborl_compute_ishock_params-.cxx
#
#)
#
#add_executable (dborl_compute_ishock ${dborl_compute_ishock_sources})
#target_link_libraries (dborl_compute_ishock dborl_algo dbsk2d_pro vidpro1)
##


set( vox_compute_ishock_sources
  vox_compute_ishock_params.h 
  vox_compute_ishock_params_sptr.h
  vox_compute_ishock_params.cxx
  vox_compute_ishock_main.cxx
  vbl_smart_ptr+vox_compute_ishock_params-.cxx
)

add_executable (vox_compute_ishock ${vox_compute_ishock_sources} )
target_link_libraries (vox_compute_ishock dborl_algo dbsk2d_pro dbdet_pro vidpro1)



