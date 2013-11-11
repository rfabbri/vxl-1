# library for storing / loading 3d image 

IF (VXL_VGUI_FOUND)
  ADD_EXECUTABLE(rsq_file_format_example    imgr_rsq_example.cxx )
  TARGET_LINK_LIBRARIES(rsq_file_format_example vgui imgr_file_format)

  ADD_EXECUTABLE(isq_file_format_example    imgr_isq_example.cxx)
  TARGET_LINK_LIBRARIES(isq_file_format_example vgui imgr_file_format)
  
  ADD_EXECUTABLE(skyscan_log_example    imgr_skyscan_log_example.cxx)
  TARGET_LINK_LIBRARIES(skyscan_log_example vgui imgr_file_format)
ENDIF (VXL_VGUI_FOUND)

ADD_EXECUTABLE(babe_rsq_file_generator    imgr_babe_rsq_generator.cxx)
TARGET_LINK_LIBRARIES(babe_rsq_file_generator imgr_file_format)


ADD_EXECUTABLE(skyscan_reconlog_example    imgr_skyscan_reconlog_header_example.cxx)
TARGET_LINK_LIBRARIES(skyscan_reconlog_example imgr_file_format)

ADD_EXECUTABLE(skyscan_rics_to_bsc_example imgr_skyscan_recon_to_bsc_example.cxx)
TARGET_LINK_LIBRARIES(skyscan_rics_to_bsc_example imgr_file_format)

INCLUDE_DIRECTORIES(${VXL_GEL_INCLUDE_DIR})
ADD_EXECUTABLE(skyscan_bsc_2_rics_example imgr_bsc_to_fbpc_example.cxx)
TARGET_LINK_LIBRARIES(skyscan_bsc_2_rics_example vcl vgl vsol imgr_file_format )

ADD_EXECUTABLE(skyscan_bscpoint_2_recpoint imgr_bsc_to_fbpc_onepoint.cxx)
TARGET_LINK_LIBRARIES(skyscan_bscpoint_2_recpoint vcl vgl vsol imgr_file_format )

ADD_EXECUTABLE(skyscan_rec_2_bsc imgr_fbpc_to_bsc_onepoint.cxx)
TARGET_LINK_LIBRARIES(skyscan_rec_2_bsc vcl vgl vsol imgr_file_format )
