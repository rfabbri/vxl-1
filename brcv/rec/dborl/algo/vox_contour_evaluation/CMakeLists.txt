IF (VXL_FOUND)
	SET( dborl_contour_evaluation_sources
	  dborl_contour_evaluation_main.cxx 
	)

	ADD_EXECUTABLE (dborl_contour_evaluation ${dborl_contour_evaluation_sources} )
	TARGET_LINK_LIBRARIES (dborl_contour_evaluation dborl_algo dbdet_pro)

	SET( dborl_contour_evaluation_new_sources
	  dborl_contour_evaluation_new_main.cxx 
	)

	ADD_EXECUTABLE (dborl_contour_evaluation_new ${dborl_contour_evaluation_new_sources} )
	TARGET_LINK_LIBRARIES (dborl_contour_evaluation_new dborl_algo dbdet_pro)
ENDIF(VXL_FOUND)
