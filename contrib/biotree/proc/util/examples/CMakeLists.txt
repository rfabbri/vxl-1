include_directories( ${lems_SOURCE_DIR}/contrib/biotree/proc/ )

add_executable(bioproc_util_filter_stats_example   proc_util_filter_stats_example.cxx )
target_link_libraries(bioproc_util_filter_stats_example bioproc_util vgui)



