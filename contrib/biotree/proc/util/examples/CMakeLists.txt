INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/biotree/proc/ )

ADD_EXECUTABLE(bioproc_util_filter_stats_example   proc_util_filter_stats_example.cxx )
TARGET_LINK_LIBRARIES(bioproc_util_filter_stats_example bioproc_util vgui)



