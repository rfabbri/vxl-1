SET( vox_convert_edg_to_img_and_orient_sources
  vox_convert_edg_to_img_and_orient_params.h 
  vox_convert_edg_to_img_and_orient_params_sptr.h
  vox_convert_edg_to_img_and_orient_params.cxx
  vbl_smart_ptr+vox_convert_edg_to_img_and_orient_params-.cxx
  vox_convert_edg_to_img_and_orient_main.cxx 
)

ADD_EXECUTABLE (vox_convert_edg_to_img_and_orient ${vox_convert_edg_to_img_and_orient_sources} )
TARGET_LINK_LIBRARIES (vox_convert_edg_to_img_and_orient dborl_algo dbdet_algo)





