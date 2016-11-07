# Brandon A. Mayer 4/13/2011
# dsm's Process Library

#bprb depends on expat and dsm2_pro depends of bprb primarily

if( VXL_EXPAT_FOUND )

	set( dsm2_pro_sources
		 dsm2_processes.h
		 dsm2_register.h			dsm2_register.cxx )
		 
    aux_source_directory( Templates dsm2_pro_sources )
	aux_source_directory( processes dsm2_pro_sources )
	
	add_library( dsm2_pro ${dsm2_pro_sources})
	
	target_link_libraries( dsm2_pro dsm2 bprb brdb vnl vil )

endif( VXL_EXPAT_FOUND )