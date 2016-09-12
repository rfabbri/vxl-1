if (VXL_FOUND)
	set( dborl_contour_evaluation_sources
	  dborl_contour_evaluation_main.cxx 
	)

	add_executable (dborl_contour_evaluation ${dborl_contour_evaluation_sources} )
	target_link_libraries (dborl_contour_evaluation dborl_algo dbdet_pro)

	set( dborl_contour_evaluation_new_sources
	  dborl_contour_evaluation_new_main.cxx 
	)

	add_executable (dborl_contour_evaluation_new ${dborl_contour_evaluation_new_sources} )
	target_link_libraries (dborl_contour_evaluation_new dborl_algo dbdet_pro)
endif(VXL_FOUND)
