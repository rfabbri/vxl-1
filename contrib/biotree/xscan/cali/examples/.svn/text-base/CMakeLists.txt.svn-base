# library for storing conic information

SET(CMAKE_CXX_FLAGS -g)

INCLUDE_DIRECTORIES( ${VXL_CORE_INCLUDE_DIR})
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR})
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg)
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic)
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg)

ADD_EXECUTABLE(cali_new_conics_write_bin_example    cali_new_conics_write_bin.cxx )
TARGET_LINK_LIBRARIES(cali_new_conics_write_bin_example cali dbdet sdet vsol vgl imgr_file_format)

ADD_EXECUTABLE(cali_conics_write_bin_example    cali_conics_write_bin.cxx )
TARGET_LINK_LIBRARIES(cali_conics_write_bin_example cali imgr_file_format)

ADD_EXECUTABLE(cali_gen_scan_imgs_example    cali_gen_scan_imgs.cxx )
TARGET_LINK_LIBRARIES(cali_gen_scan_imgs_example cali xscan imgr_file_format)

ADD_EXECUTABLE(cali_gen_overlay_imgs_example    cali_gen_overlay_imgs.cxx )
TARGET_LINK_LIBRARIES(cali_gen_overlay_imgs_example cali xscan imgr_file_format)

ADD_EXECUTABLE(cali_error_summary    cali_error_summary.cxx )
TARGET_LINK_LIBRARIES(cali_error_summary cali xscan imgr_file_format)

ADD_EXECUTABLE(log_to_scan    log_to_scan.cxx )
TARGET_LINK_LIBRARIES(log_to_scan  xscan imgr_file_format)

ADD_EXECUTABLE(cali_calibrate_example    cali_calibrate.cxx )
TARGET_LINK_LIBRARIES(cali_calibrate_example vgui cali xscan imgr_file_format vnl vnl_algo)







