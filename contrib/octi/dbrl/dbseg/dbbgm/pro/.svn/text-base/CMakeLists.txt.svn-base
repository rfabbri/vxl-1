# dbrl/dbseg/dbbgm/pro/CMakeLists.txt

INCLUDE( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
IF(EXPAT_FOUND)

INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${DBRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bpro )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
SET(dbbgmo_pro_sources
    bbgm_register.h                   bbgm_register.cxx
    bbgm_processes.h
)

AUX_SOURCE_DIRECTORY(Templates dbbgmo_pro_sources)
AUX_SOURCE_DIRECTORY(processes dbbgmo_pro_sources)

ADD_LIBRARY(dbbgmo_pro ${dbbgmo_pro_sources})
IF(LINK_WITH_DLIBS)
	TARGET_LINK_LIBRARIES(dbbgmo_pro dbbgmo bprb brdb dbstao_algo dbstao brip vidl vil vil_pro vsl vgui bgui dbbgmo_vis)
ELSE(LINK_WITH_DLIBS)
	TARGET_LINK_LIBRARIES(dbbgmo_pro bbgm bprb brdb bsta_algo bsta brip vidl vil vil_pro vsl)
ENDIF(LINK_WITH_DLIBS)
IF(LINK_WITH_DLIBS)
		add_definitions( 
		-DDEVELINCLUDEPATH
		)

IF (BUILD_OCTI_TESTING OR BUILD_TESTING)
	SUBDIRS(tests)
ENDIF(BUILD_OCTI_TESTING OR BUILD_TESTING)



ENDIF(LINK_WITH_DLIBS)

ENDIF(EXPAT_FOUND)
