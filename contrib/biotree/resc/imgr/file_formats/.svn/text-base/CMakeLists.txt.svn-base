# Detection for filters
# blem/contrib/biotree/resc/imgr/CMakeLists.txt
INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR}/bbas)

SET(CMAKE_CXX_FLAGS -g)
SET(imgr_file_format_src
  imgr_ff_algos.cxx                  imgr_ff_algos.h
  imgr_rsq_header.cxx                imgr_rsq_header.h
  imgr_rsq.cxx                       imgr_rsq.h

  imgr_isq_header.cxx                imgr_isq_header.h
  imgr_isq_file_format.cxx           imgr_isq_file_format.h

  imgr_skyscan_log_header.cxx        imgr_skyscan_log_header.h
  imgr_skyscan_log.cxx               imgr_skyscan_log.h

  imgr_skyscan_reconlog.cxx   imgr_skyscan_reconlog.h
)

AUX_SOURCE_DIRECTORY(Templates imgr_file_format_src)

ADD_LIBRARY(imgr_file_format ${imgr_file_format_src})

TARGET_LINK_LIBRARIES(imgr_file_format vcl imgr vnl vsl)

IF(BUILD_EXAMPLES)
  SUBDIRS(examples)
ENDIF(BUILD_EXAMPLES)
 
IF(VXL_VGUI_FOUND)
  SUBDIRS(vis)
ENDIF(VXL_VGUI_FOUND)

IF(BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)

SUBDIRS(converters)
