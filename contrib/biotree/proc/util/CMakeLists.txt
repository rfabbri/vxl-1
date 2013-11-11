INCLUDE_DIRECTORIES(${lems_SOURCE_DIR}/contrib/biotree/proc/util/)

SET(util_src
    proc_filter_stats.cxx        proc_filter_stats.h
    proc_det_filter_utils.cxx    proc_det_filter_utils.h
   )


ADD_LIBRARY(bioproc_util ${util_src})

TARGET_LINK_LIBRARIES(bioproc_util xmvg xmvg_algo vnl vcl)

IF(BUILD_EXAMPLES)
    SUBDIRS(examples)
ENDIF(BUILD_EXAMPLES)
  
IF(BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)

