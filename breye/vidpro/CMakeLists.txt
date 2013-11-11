# brl/vidpro/CMakeLists.txt


INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_OXL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas/brdb )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic/ )

SET(vidpro_sources
   vidpro_process_manager.cxx             vidpro_process_manager.h             vidpro_process_manager_sptr.h
   vidpro_repository.cxx                  vidpro_repository.h                  vidpro_repository_sptr.h
  # vidpro_vidl_gui_param_dialog.cxx      vidpro_vidl_gui_param_dialog.h  
 vidpro_object_stream.cxx           vidpro_object_stream.h        vidpro_object_stream_sptr.h       

# Video processes
   
  
   process/vidpro_motion_process.cxx               process/vidpro_motion_process.h
   process/vidpro_horn_schunck_process.cxx         process/vidpro_horn_schunck_process.h
   process/vidpro_frame_diff_process.cxx           process/vidpro_frame_diff_process.h
   process/vidpro_frame_average_process.cxx        process/vidpro_frame_average_process.h
   process/vidpro_equalize_images_process.cxx      process/vidpro_equalize_images_process.h

   process/vidpro_brightness_contrast_process.cxx  process/vidpro_brightness_contrast_process.h
   process/vidpro_gaussian_blur_process.cxx        process/vidpro_gaussian_blur_process.h
   process/vidpro_resample_process.cxx             process/vidpro_resample_process.h
   process/vidpro_grey_image_process.cxx           process/vidpro_grey_image_process.h
   process/vidpro_RGB_to_IHS_process.cxx           process/vidpro_RGB_to_IHS_process.h
   process/vidpro_RGB_to_CIELab_process.cxx        process/vidpro_RGB_to_CIELab_process.h
   process/vidpro_split_into_RGB_process.cxx       process/vidpro_split_into_RGB_process.h
   process/vidpro_image_gradient_process.cxx       process/vidpro_image_gradient_process.h

   process/vidpro_VD_edge_process.cxx              process/vidpro_VD_edge_process.h
   process/vidpro_vsol_scale_process.cxx           process/vidpro_vsol_scale_process.h
   process/vidpro_harris_corners_process.cxx       process/vidpro_harris_corners_process.h
   process/vidpro_global_harris_corners_process.cxx       
   process/vidpro_global_harris_corners_process.h

   process/vidpro_load_image_process.cxx           process/vidpro_load_image_process.h
   process/vidpro_save_image_process.cxx           process/vidpro_save_image_process.h
   process/vidpro_open_istream_process.cxx         process/vidpro_open_istream_process.h
   process/vidpro_open_live_istream_process.cxx    process/vidpro_open_live_istream_process.h
   process/vidpro_open_dc1394_istream_process.cxx  process/vidpro_open_dc1394_istream_process.h
   process/vidpro_open_ostream_process.cxx         process/vidpro_open_ostream_process.h
   process/vidpro_frame_from_istream_process.cxx   process/vidpro_frame_from_istream_process.h
   process/vidpro_load_video_process.cxx           process/vidpro_load_video_process.h
   process/vidpro_save_video_process.cxx           process/vidpro_save_video_process.h
   process/vidpro_load_con_process.cxx             process/vidpro_load_con_process.h
   process/vidpro_save_con_process.cxx             process/vidpro_save_con_process.h
   process/vidpro_load_cem_process.cxx             process/vidpro_load_cem_process.h
   process/vidpro_save_cem_process.cxx             process/vidpro_save_cem_process.h
   process/vidpro_load_edg_process.cxx             process/vidpro_load_edg_process.h
   process/vidpro_save_ps_process.cxx              process/vidpro_save_ps_process.h      
   process/vidpro_edgeprune_process.cxx            process/vidpro_edgeprune_process.h   
   process/vidpro_smoothcem_process.cxx            process/vidpro_smoothcem_process.h 
   
   process/vidpro_kl_process.cxx                   process/vidpro_kl_process.h
   process/vidpro_background_diff_process.cxx      process/vidpro_background_diff_process.h 

  # process/vidpro_load_video_and_polys_process.cxx process/vidpro_load_video_and_polys_process.h

# Storage classes
   storage/vidpro_istream_storage.cxx      storage/vidpro_istream_storage.h      storage/vidpro_istream_storage_sptr.h
   storage/vidpro_ostream_storage.cxx      storage/vidpro_ostream_storage.h      storage/vidpro_ostream_storage_sptr.h
   storage/vidpro_obj_stream_storage.cxx   storage/vidpro_obj_stream_storage.h   storage/vidpro_obj_stream_storage_sptr.h
 #  storage/vidpro_object_stream.cxx        storage/vidpro_object_stream.h        storage/vidpro_object_stream_sptr.h
   storage/vidpro_image_storage.cxx        storage/vidpro_image_storage.h        storage/vidpro_image_storage_sptr.h
   storage/vidpro_vsol2D_storage.cxx       storage/vidpro_vsol2D_storage.h       storage/vidpro_vsol2D_storage_sptr.h
   storage/vidpro_vsol3D_storage.cxx       storage/vidpro_vsol3D_storage.h       storage/vidpro_vsol3D_storage_sptr.h
   storage/vidpro_vtol_storage.cxx         storage/vidpro_vtol_storage.h         storage/vidpro_vtol_storage_sptr.h
   storage/vidpro_fmatrix_storage.cxx      storage/vidpro_fmatrix_storage.h      storage/vidpro_fmatrix_storage_sptr.h
)


AUX_SOURCE_DIRECTORY(Templates vidpro_sources)

ADD_LIBRARY(vidpro ${vidpro_sources})

TARGET_LINK_LIBRARIES(vidpro bpro sdet vgel vil_io vbl_io mvl vimt vidl brdb)

# vgui add on library
#SUBDIRS( gui )

# Add one of these libraries if you have linker errors
#  vsol vdgl vtol vil vil_algo vnl vnl_io vnl_algo vbl vidl1 vul


