# contrib/dbrl/dbseg/dbbgm_batch/CMakeLists.txt
INCLUDE( ${VXL_CMAKE_DIR}/NewCMake/FindPython.cmake )
IF( PYTHON_FOUND )
  INCLUDE( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
  IF( EXPAT_FOUND )
    INCLUDE(${MODULE_PATH}/FindTIFF.cmake)
    IF(TIFF_FOUND)
      INCLUDE( ${MODULE_PATH}/FindGEOTIFF.cmake)
      IF(GEOTIFF_FOUND)
        INCLUDE( ${MODULE_PATH}/FindZLIB.cmake )
        IF(ZLIB_FOUND)
          INCLUDE_DIRECTORIES( ${ZLIB_INCLUDE_DIR} )
          INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/b3p )
        ENDIF(ZLIB_FOUND)
        INCLUDE_DIRECTORIES( ${PYTHON_INCLUDE_DIRS} )
        INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR} )
        INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro )
        INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro/core )
        
        INCLUDE_DIRECTORIES( ${VXLCORE_INCLUDE_DIR} )
        INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc )
        INCLUDE_DIRECTORIES( ${MUL_INCLUDE_DIR} )
        INCLUDE_DIRECTORIES( ${DBRL_INCLUDE_DIR}/dbseg/dbbgm )
      
        INCLUDE_DIRECTORIES( ${TIFF_INCLUDE_DIR} ) 
        INCLUDE_DIRECTORIES( ${GEOTIFF_INCLUDE_DIR} ) 
    
        SET(dbbgmo_batch_sources
          reg_bbgm.cxx          reg_bbgm.h
        )
     
        ADD_LIBRARY(dbbgmo_batch MODULE ${dbbgmo_batch_sources})

        IF(WIN32)
          #library has to have different name depending on debug or release version.
          SET_TARGET_PROPERTIES(dbbgmo_batch PROPERTIES OUTPUT_NAME dbbgm_batch DEBUG_POSTFIX _d SUFFIX .pyd )
        ENDIF(WIN32)
        SET_TARGET_PROPERTIES(dbbgmo_batch PROPERTIES PREFIX "")
		IF (LINK_WITH_DLIBS)
			TARGET_LINK_LIBRARIES(dbbgmo_batch bprb brdb bpro_batch dbbgmo_pro vbl vidl_pro vil_io vil_pro) 
		ELSE (LINK_WITH_DLIBS)
			TARGET_LINK_LIBRARIES(dbbgmo_batch bprb brdb bpro_batch bbgm_pro vbl vidl_pro vil_io vil_pro)
		ENDIF(LINK_WITH_DLIBS)	
		IF(LINK_WITH_DLIBS)
		add_definitions( 
		-DDEVELINCLUDEPATH
		)
		ENDIF(LINK_WITH_DLIBS)
		
        IF (PYTHON_DEBUG_LIBRARIES)
          TARGET_LINK_LIBRARIES( dbbgmo_batch debug ${PYTHON_DEBUG_LIBRARY})
        ENDIF(PYTHON_DEBUG_LIBRARIES )

        IF(PYTHON_LIBRARIES)
          TARGET_LINK_LIBRARIES( dbbgmo_batch optimized ${PYTHON_LIBRARY})
        ENDIF(PYTHON_LIBRARIES)
    ENDIF(GEOTIFF_FOUND )
      ENDIF(TIFF_FOUND ) 

  ENDIF( EXPAT_FOUND ) 
ENDIF( PYTHON_FOUND )
