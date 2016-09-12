# this is /contrib/bm/dsm2/io/CMakeLists.txt
# io library for dsm2

include_directories( ${bm_SOURCE_DIR} )
include_directories( ${VXL_CORE_INCLUDE_DIR} )

set( dsm2_io_sources
	 
	 dsm2_io_node_base.h								dsm2_io_node_base.cxx
	 dsm2_io_node_gaussian.h							dsm2_io_node_gaussian.hxx
	 
	 dsm2_io_transition_table_base.h					dsm2_io_transition_table_base.cxx
	 dsm2_io_transition_map.h							dsm2_io_transition_map.hxx
	 
	 dsm2_io_state_machine_base.h						dsm2_io_state_machine_base.cxx
	 dsm2_io_state_machine.h							dsm2_io_state_machine.hxx
	 
	 dsm2_io_manager_base.h								dsm2_io_manager_base.cxx
	 dsm2_io_manager_pixels.h							dsm2_io_manager_pixels.hxx
	 )
	 
aux_source_directory( Templates dsm2_io_sources ) 

add_library( dsm2_io ${dsm2_io_sources} )
target_link_libraries( dsm2_io dsm2 vbl vsl vnl_io )