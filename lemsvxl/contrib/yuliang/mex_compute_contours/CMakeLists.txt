
#mex version
IF(UNIX)
	FIND_PACKAGE(Matlab)
	INCLUDE_DIRECTORIES(${MATLAB_INCLUDE_DIR})
	ADD_DEFINITIONS(-DMATLAB_MEX_FILE)
	ADD_LIBRARY(mex_compute_curve_frags MODULE mex_edge_det_link_main.cxx)
	SET_TARGET_PROPERTIES(mex_compute_curve_frags PROPERTIES
	LIBRARY_OUTPUT_DIRECTORY /home/guoy/lemsvxl_bin/mex PREFIX "" SUFFIX ".mexa64")
	TARGET_LINK_LIBRARIES(mex_compute_curve_frags dbdet_pro ${MATLAB_LIBRARIES})
	
	
ENDIF(UNIX)
