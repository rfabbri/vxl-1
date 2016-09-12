# Detection for filters
# blem/contrib/biotree/resc/imgr/CMakeLists.txt
include_directories(${VXL_BRL_INCLUDE_DIR}/bbas)

set(CMAKE_CXX_FLAGS -g)
set(imgr_file_format_src
  imgr_ff_algos.cxx                  imgr_ff_algos.h
  imgr_rsq_header.cxx                imgr_rsq_header.h
  imgr_rsq.cxx                       imgr_rsq.h

  imgr_isq_header.cxx                imgr_isq_header.h
  imgr_isq_file_format.cxx           imgr_isq_file_format.h

  imgr_skyscan_log_header.cxx        imgr_skyscan_log_header.h
  imgr_skyscan_log.cxx               imgr_skyscan_log.h

  imgr_skyscan_reconlog.cxx   imgr_skyscan_reconlog.h
)

aux_source_directory(Templates imgr_file_format_src)

add_library(imgr_file_format ${imgr_file_format_src})

target_link_libraries(imgr_file_format vcl imgr vnl vsl)

if(BUILD_EXAMPLES)
  add_subdirectory(examples)
endif(BUILD_EXAMPLES)
 
if(VXL_VGUI_FOUND)
  add_subdirectory(vis)
endif(VXL_VGUI_FOUND)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif(BUILD_TESTING)

add_subdirectory(converters)
