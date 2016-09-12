# This is /contrib/bm/dsm2/exe/CMakeLists.txt
# Executables utilizing dsm2 library

include_directories( ${VXL_CORE_INCLUDE_DIR} )

include_directories( ${bm_SOURCE_DIR} )

set( dsm2_node_size_main_sources
	 dsm2_node_size_main.cxx )
	 
add_executable( dsm2_node_size_main ${dsm2_node_size_main_sources} )	 
target_link_libraries( dsm2_node_size_main dsm2 bsta_io )