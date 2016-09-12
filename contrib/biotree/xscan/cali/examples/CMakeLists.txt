# library for storing conic information

set(CMAKE_CXX_FLAGS -g)

include_directories( ${VXL_CORE_INCLUDE_DIR})
include_directories( ${VXL_GEL_INCLUDE_DIR})
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg)
include_directories( ${lems_SOURCE_DIR}/basic)
include_directories( ${lems_SOURCE_DIR}/brcv/seg)

add_executable(cali_new_conics_write_bin_example    cali_new_conics_write_bin.cxx )
target_link_libraries(cali_new_conics_write_bin_example cali dbdet sdet vsol vgl imgr_file_format)

add_executable(cali_conics_write_bin_example    cali_conics_write_bin.cxx )
target_link_libraries(cali_conics_write_bin_example cali imgr_file_format)

add_executable(cali_gen_scan_imgs_example    cali_gen_scan_imgs.cxx )
target_link_libraries(cali_gen_scan_imgs_example cali xscan imgr_file_format)

add_executable(cali_gen_overlay_imgs_example    cali_gen_overlay_imgs.cxx )
target_link_libraries(cali_gen_overlay_imgs_example cali xscan imgr_file_format)

add_executable(cali_error_summary    cali_error_summary.cxx )
target_link_libraries(cali_error_summary cali xscan imgr_file_format)

add_executable(log_to_scan    log_to_scan.cxx )
target_link_libraries(log_to_scan  xscan imgr_file_format)

add_executable(cali_calibrate_example    cali_calibrate.cxx )
target_link_libraries(cali_calibrate_example vgui cali xscan imgr_file_format vnl vnl_algo)







