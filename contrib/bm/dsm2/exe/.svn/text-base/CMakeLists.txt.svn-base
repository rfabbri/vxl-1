# This is /contrib/bm/dsm2/exe/CMakeLists.txt
# Executables utilizing dsm2 library

INCLUDE_DIRECTORIES( ${VXL_CORE_INCLUDE_DIR} )

INCLUDE_DIRECTORIES( ${bm_SOURCE_DIR} )

SET( dsm2_node_size_main_sources
	 dsm2_node_size_main.cxx )
	 
ADD_EXECUTABLE( dsm2_node_size_main ${dsm2_node_size_main_sources} )	 
TARGET_LINK_LIBRARIES( dsm2_node_size_main dsm2 bsta_io )