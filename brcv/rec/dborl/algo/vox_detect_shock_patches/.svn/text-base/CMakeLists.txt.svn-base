INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/db3p )

SET( vox_detect_shock_patches_sources
  vox_detect_shock_patches_params.h 
  vox_detect_shock_patches_params_sptr.h
  vox_detect_shock_patches_params.cxx
  vox_detect_shock_patches_main.cxx
  vbl_smart_ptr+vox_detect_shock_patches_params-.cxx
)

ADD_EXECUTABLE (vox_detect_shock_patches ${vox_detect_shock_patches_sources} )
TARGET_LINK_LIBRARIES (vox_detect_shock_patches dborl_algo dbskr_algo dbskr_pro vidpro1 dbxml)



