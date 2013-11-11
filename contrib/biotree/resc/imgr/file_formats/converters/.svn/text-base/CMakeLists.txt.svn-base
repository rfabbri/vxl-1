# converters

SET(CMAKE_CXX_FLAGS -g)

INCLUDE_DIRECTORIES(${BIOTREE_SOURCE_DIR}/xscan )

ADD_EXECUTABLE(isq_to_tiff_converter isq_to_tiff_converter.cxx )
TARGET_LINK_LIBRARIES(isq_to_tiff_converter vgui imgr_file_format)

ADD_EXECUTABLE(rsq_to_tiff_converter rsq_to_tiff_converter.cxx )
TARGET_LINK_LIBRARIES(rsq_to_tiff_converter  vgui imgr_file_format)

ADD_EXECUTABLE(rsq_cali_images rsq_cali_images.cxx )
TARGET_LINK_LIBRARIES(rsq_cali_images  imgr_file_format)

ADD_EXECUTABLE(imgr_log_file_modifier modify_log_file.cxx )
TARGET_LINK_LIBRARIES(imgr_log_file_modifier vgui imgr_file_format)

ADD_EXECUTABLE(spe_to_tiff spe_to_tiff.cxx )
TARGET_LINK_LIBRARIES(spe_to_tiff imgr_file_format)


