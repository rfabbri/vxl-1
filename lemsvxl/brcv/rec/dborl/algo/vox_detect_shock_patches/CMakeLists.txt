include_directories( ${LEMS_SOURCE_DIR}/db3p )

set( vox_detect_shock_patches_sources
  vox_detect_shock_patches_params.h 
  vox_detect_shock_patches_params_sptr.h
  vox_detect_shock_patches_params.cxx
  vox_detect_shock_patches_main.cxx
  vbl_smart_ptr+vox_detect_shock_patches_params-.cxx
)

add_executable (vox_detect_shock_patches ${vox_detect_shock_patches_sources} )
target_link_libraries (vox_detect_shock_patches dborl_algo dbskr_algo dbskr_pro vidpro1 dbxml)



