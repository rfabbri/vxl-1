include_directories( ${lems_SOURCE_DIR}/db3p )

set( vox_extract_shock_patches_sources
  vox_extract_shock_patches_params.h 
  vox_extract_shock_patches_params_sptr.h
  vox_extract_shock_patches_params.cxx
  vox_extract_shock_patches_main.cxx
  vbl_smart_ptr+vox_extract_shock_patches_params-.cxx
)

add_executable (vox_extract_shock_patches ${vox_extract_shock_patches_sources} )
target_link_libraries (vox_extract_shock_patches dborl_algo dbskr_algo dbskr_pro bpro1)



