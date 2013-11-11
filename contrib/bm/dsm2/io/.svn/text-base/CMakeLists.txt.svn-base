# this is /contrib/bm/dsm2/io/CMakeLists.txt
# io library for dsm2

INCLUDE_DIRECTORIES( ${bm_SOURCE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_CORE_INCLUDE_DIR} )

SET( dsm2_io_sources
	 
	 dsm2_io_node_base.h								dsm2_io_node_base.cxx
	 dsm2_io_node_gaussian.h							dsm2_io_node_gaussian.txx
	 
	 dsm2_io_transition_table_base.h					dsm2_io_transition_table_base.cxx
	 dsm2_io_transition_map.h							dsm2_io_transition_map.txx
	 
	 dsm2_io_state_machine_base.h						dsm2_io_state_machine_base.cxx
	 dsm2_io_state_machine.h							dsm2_io_state_machine.txx
	 
	 dsm2_io_manager_base.h								dsm2_io_manager_base.cxx
	 dsm2_io_manager_pixels.h							dsm2_io_manager_pixels.txx
	 )
	 
AUX_SOURCE_DIRECTORY( Templates dsm2_io_sources ) 

ADD_LIBRARY( dsm2_io ${dsm2_io_sources} )
TARGET_LINK_LIBRARIES( dsm2_io dsm2 vbl vsl vnl_io )