# contrib/dbrl/dbseg/dbbgm_batch/CMakeLists.txt
include( ${VXL_CMAKE_DIR}/NewCMake/FindPython.cmake )
if( PYTHON_FOUND )
  include( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
  if( EXPAT_FOUND )
    include(${MODULE_PATH}/FindTIFF.cmake)
    if(TIFF_FOUND)
      include( ${MODULE_PATH}/FindGEOTIFF.cmake)
      if(GEOTIFF_FOUND)
        include( ${MODULE_PATH}/FindZLIB.cmake )
        if(ZLIB_FOUND)
          include_directories( ${ZLIB_INCLUDE_DIR} )
          include_directories( ${BRL_INCLUDE_DIR}/b3p )
        endif(ZLIB_FOUND)
        include_directories( ${PYTHON_INCLUDE_DIRS} )
        include_directories( ${BRL_INCLUDE_DIR} )
        include_directories( ${BRL_INCLUDE_DIR}/bpro )
        include_directories( ${BRL_INCLUDE_DIR}/bpro/core )
        
        include_directories( ${VXLCORE_INCLUDE_DIR} )
        include_directories( ${GEL_INCLUDE_DIR}/mrc )
        include_directories( ${MUL_INCLUDE_DIR} )
        include_directories( ${DBRL_INCLUDE_DIR}/dbseg/dbbgm )
      
        include_directories( ${TIFF_INCLUDE_DIR} ) 
        include_directories( ${GEOTIFF_INCLUDE_DIR} ) 
    
        set(dbbgmo_batch_sources
          reg_bbgm.cxx          reg_bbgm.h
        )
     
        add_library(dbbgmo_batch MODULE ${dbbgmo_batch_sources})

        if(WIN32)
          #library has to have different name depending on debug or release version.
          set_target_properties(dbbgmo_batch PROPERTIES OUTPUT_NAME dbbgm_batch DEBUG_POSTFIX _d SUFFIX .pyd )
        endif(WIN32)
        set_target_properties(dbbgmo_batch PROPERTIES PREFIX "")
		if (LINK_WITH_DLIBS)
			target_link_libraries(dbbgmo_batch bprb brdb bpro_batch dbbgmo_pro vbl vidl_pro vil_io vil_pro) 
		else (LINK_WITH_DLIBS)
			target_link_libraries(dbbgmo_batch bprb brdb bpro_batch bbgm_pro vbl vidl_pro vil_io vil_pro)
		endif(LINK_WITH_DLIBS)	
		if(LINK_WITH_DLIBS)
		add_definitions( 
		-DDEVELINCLUDEPATH
		)
		endif(LINK_WITH_DLIBS)
		
        if (PYTHON_DEBUG_LIBRARIES)
          target_link_libraries( dbbgmo_batch debug ${PYTHON_DEBUG_LIBRARY})
        endif(PYTHON_DEBUG_LIBRARIES )

        if(PYTHON_LIBRARIES)
          target_link_libraries( dbbgmo_batch optimized ${PYTHON_LIBRARY})
        endif(PYTHON_LIBRARIES)
    endif(GEOTIFF_FOUND )
      endif(TIFF_FOUND ) 

  endif( EXPAT_FOUND ) 
endif( PYTHON_FOUND )
