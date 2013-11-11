INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES(${lems_SOURCE_DIR}/brcv/seg)

SET(third_order_edge_detector_sources
       third_order_edge_detector.cxx
    )
ADD_EXECUTABLE(third_order_edge_detector  ${third_order_edge_detector_sources} )
TARGET_LINK_LIBRARIES (third_order_edge_detector dbdet_pro)


#mex version
IF(UNIX)
	FIND_PACKAGE(Matlab)
	INCLUDE_DIRECTORIES(${MATLAB_INCLUDE_DIR})
	ADD_DEFINITIONS(-DMATLAB_MEX_FILE)
	ADD_LIBRARY(mex_third_order_edge_detector MODULE mex_third_order_edge_detector.cxx)
	SET_TARGET_PROPERTIES(mex_third_order_edge_detector PROPERTIES
	LIBRARY_OUTPUT_DIRECTORY /home/mn/lemsvxl/bin/mex PREFIX "" SUFFIX ".mexa64")
	TARGET_LINK_LIBRARIES(mex_third_order_edge_detector vxl_mex_utils dbdet_pro ${MATLAB_LIBRARIES})
	
	
	ADD_LIBRARY(mex_third_order_color_edge_detector MODULE mex_third_order_color_edge_detector.cxx)
	SET_TARGET_PROPERTIES(mex_third_order_color_edge_detector PROPERTIES
	LIBRARY_OUTPUT_DIRECTORY /home/firat/lemsvxl/bin/mex PREFIX "" SUFFIX ".mexa64")
	TARGET_LINK_LIBRARIES(mex_third_order_color_edge_detector vxl_mex_utils dbdet_pro ${MATLAB_LIBRARIES})
	
ENDIF(UNIX)