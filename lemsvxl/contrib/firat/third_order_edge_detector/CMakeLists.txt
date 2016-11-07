include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_MUL_INCLUDE_DIR} )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories(${lems_SOURCE_DIR}/brcv/seg)

set(third_order_edge_detector_sources
       third_order_edge_detector.cxx
    )
add_executable(third_order_edge_detector  ${third_order_edge_detector_sources} )
target_link_libraries (third_order_edge_detector dbdet_pro)


#mex version
if(UNIX)
	find_package(Matlab)
	include_directories(${MATLAB_INCLUDE_DIR})
	add_definitions(-DMATLAB_MEX_FILE)
	add_library(mex_third_order_edge_detector MODULE mex_third_order_edge_detector.cxx)
	set_target_properties(mex_third_order_edge_detector PROPERTIES
	LIBRARY_OUTPUT_DIRECTORY /home/mn/lemsvxl/bin/mex PREFIX "" SUFFIX ".mexa64")
	target_link_libraries(mex_third_order_edge_detector vxl_mex_utils dbdet_pro ${MATLAB_LIBRARIES})
	
	
	add_library(mex_third_order_color_edge_detector MODULE mex_third_order_color_edge_detector.cxx)
	set_target_properties(mex_third_order_color_edge_detector PROPERTIES
	LIBRARY_OUTPUT_DIRECTORY /home/firat/lemsvxl/bin/mex PREFIX "" SUFFIX ".mexa64")
	target_link_libraries(mex_third_order_color_edge_detector vxl_mex_utils dbdet_pro ${MATLAB_LIBRARIES})
	
endif(UNIX)