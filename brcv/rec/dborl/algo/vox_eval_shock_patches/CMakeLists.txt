
set( vox_eval_shock_patches_sources
 
  vox_eval_shock_patches_params.h 
  vox_eval_shock_patches_params_sptr.h
  vox_eval_shock_patches.h
 
  vox_eval_shock_patches.cxx
  vox_eval_shock_patches_params.cxx
  vox_eval_shock_patches_main.cxx
  vbl_smart_ptr+vox_eval_shock_patches_params-.cxx
)

add_executable (vox_eval_shock_patches ${vox_eval_shock_patches_sources} )
target_link_libraries (vox_eval_shock_patches dborl_algo dbxml)



