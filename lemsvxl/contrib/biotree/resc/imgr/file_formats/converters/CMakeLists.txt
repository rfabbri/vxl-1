# converters

set(CMAKE_CXX_FLAGS -g)

include_directories(${BIOTREE_SOURCE_DIR}/xscan )

add_executable(isq_to_tiff_converter isq_to_tiff_converter.cxx )
target_link_libraries(isq_to_tiff_converter vgui imgr_file_format)

add_executable(rsq_to_tiff_converter rsq_to_tiff_converter.cxx )
target_link_libraries(rsq_to_tiff_converter  vgui imgr_file_format)

add_executable(rsq_cali_images rsq_cali_images.cxx )
target_link_libraries(rsq_cali_images  imgr_file_format)

add_executable(imgr_log_file_modifier modify_log_file.cxx )
target_link_libraries(imgr_log_file_modifier vgui imgr_file_format)

add_executable(spe_to_tiff spe_to_tiff.cxx )
target_link_libraries(spe_to_tiff imgr_file_format)


