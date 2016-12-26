
set( vox_visualize_patch_matches_sources
     vox_visualize_patch_matches_params.h 
     vox_visualize_patch_matches_params_sptr.h
     vox_visualize_patch_matches.h

     vox_visualize_patch_matches.cxx
     vox_visualize_patch_matches_params.cxx
     vox_visualize_patch_matches_main.cxx
     vbl_smart_ptr+vox_visualize_patch_matches_params-.cxx
)

add_executable (vox_visualize_patch_matches 
  ${vox_visualize_patch_matches_sources} )
target_link_libraries (vox_visualize_patch_matches dborl_algo dbskr_pro)



