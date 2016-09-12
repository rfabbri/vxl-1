# library for storing / loading 3d image 

if (VXL_VGUI_FOUND)
  add_executable(rsq_file_format_example    imgr_rsq_example.cxx )
  target_link_libraries(rsq_file_format_example vgui imgr_file_format)

  add_executable(isq_file_format_example    imgr_isq_example.cxx)
  target_link_libraries(isq_file_format_example vgui imgr_file_format)
  
  add_executable(skyscan_log_example    imgr_skyscan_log_example.cxx)
  target_link_libraries(skyscan_log_example vgui imgr_file_format)
endif (VXL_VGUI_FOUND)

add_executable(babe_rsq_file_generator    imgr_babe_rsq_generator.cxx)
target_link_libraries(babe_rsq_file_generator imgr_file_format)


add_executable(skyscan_reconlog_example    imgr_skyscan_reconlog_header_example.cxx)
target_link_libraries(skyscan_reconlog_example imgr_file_format)

add_executable(skyscan_rics_to_bsc_example imgr_skyscan_recon_to_bsc_example.cxx)
target_link_libraries(skyscan_rics_to_bsc_example imgr_file_format)

include_directories(${VXL_GEL_INCLUDE_DIR})
add_executable(skyscan_bsc_2_rics_example imgr_bsc_to_fbpc_example.cxx)
target_link_libraries(skyscan_bsc_2_rics_example vcl vgl vsol imgr_file_format )

add_executable(skyscan_bscpoint_2_recpoint imgr_bsc_to_fbpc_onepoint.cxx)
target_link_libraries(skyscan_bscpoint_2_recpoint vcl vgl vsol imgr_file_format )

add_executable(skyscan_rec_2_bsc imgr_fbpc_to_bsc_onepoint.cxx)
target_link_libraries(skyscan_rec_2_bsc vcl vgl vsol imgr_file_format )
