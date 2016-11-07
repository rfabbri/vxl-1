include_directories(${lems_SOURCE_DIR}/contrib/biotree/proc/util/)

set(util_src
    proc_filter_stats.cxx        proc_filter_stats.h
    proc_det_filter_utils.cxx    proc_det_filter_utils.h
   )


add_library(bioproc_util ${util_src})

target_link_libraries(bioproc_util xmvg xmvg_algo vnl vcl)

if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif(BUILD_EXAMPLES)
  
if(BUILD_TESTING)
  add_subdirectory(tests)
endif(BUILD_TESTING)

