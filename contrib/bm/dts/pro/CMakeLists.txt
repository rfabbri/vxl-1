# Brandon A. Mayer 10/7/2011
# dts Process Library

PROJECT( dts_pro )

#BPRB DEPENDS ON EXPAT
IF( VXL_EXPAT_FOUND )

    SET( dts_pro_sources
         dts_processes.h
         dts_register.h         dts_register.cxx
        )
        
    AUX_SOURCE_DIRECTORY( processes dts_pro_sources )
    AUX_SOURCE_DIRECTORY( Templates dts_pro_sources )
    
    ADD_LIBRARY( dts_pro ${dts_pro_sources} )
    
    TARGET_LINK_LIBRARIES( dts_pro dts dts_io bapl bprb brdb vidl vil )

ENDIF( VXL_EXPAT_FOUND )