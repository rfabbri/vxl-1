# Brandon A. Mayer 4/13/2011
# dsm's Process Library

#bprb depends on expat and dsm2_pro depends of bprb primarily

IF( VXL_EXPAT_FOUND )

	SET( dsm2_pro_sources
		 dsm2_processes.h
		 dsm2_register.h			dsm2_register.cxx )
		 
    AUX_SOURCE_DIRECTORY( Templates dsm2_pro_sources )
	AUX_SOURCE_DIRECTORY( processes dsm2_pro_sources )
	
	ADD_LIBRARY( dsm2_pro ${dsm2_pro_sources})
	
	TARGET_LINK_LIBRARIES( dsm2_pro dsm2 bprb brdb vnl vil )

ENDIF( VXL_EXPAT_FOUND )