# Brandon A. Mayer 10/7/2011
# dts Process Library

project( dts_pro )

#BPRB DEPENDS ON EXPAT
if( VXL_EXPAT_FOUND )

    set( dts_pro_sources
         dts_processes.h
         dts_register.h         dts_register.cxx
        )
        
    aux_source_directory( processes dts_pro_sources )
    aux_source_directory( Templates dts_pro_sources )
    
    add_library( dts_pro ${dts_pro_sources} )
    
    target_link_libraries( dts_pro dts dts_io bapl bprb brdb vidl vil )

endif( VXL_EXPAT_FOUND )