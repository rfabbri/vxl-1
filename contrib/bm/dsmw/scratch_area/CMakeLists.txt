#This is /contrib/bm/dsmw/scratch_area/CMakeLists.txt
#This directory will be used to test ideas for the dsmw library


add_executable( transition_table transition_table.cxx )

target_link_libraries( transition_table bsta )


