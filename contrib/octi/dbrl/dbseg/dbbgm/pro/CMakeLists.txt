# dbrl/dbseg/dbbgm/pro/CMakeLists.txt

include( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
if(EXPAT_FOUND)

include_directories( ${BRL_INCLUDE_DIR} )
include_directories( ${DBRL_INCLUDE_DIR}/bseg )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bpro )
include_directories( ${VXL_GEL_INCLUDE_DIR} )
set(dbbgmo_pro_sources
    bbgm_register.h                   bbgm_register.cxx
    bbgm_processes.h
)

aux_source_directory(Templates dbbgmo_pro_sources)
aux_source_directory(processes dbbgmo_pro_sources)

add_library(dbbgmo_pro ${dbbgmo_pro_sources})
if(LINK_WITH_DLIBS)
	target_link_libraries(dbbgmo_pro dbbgmo bprb brdb dbstao_algo dbstao brip vidl vil vil_pro vsl vgui bgui dbbgmo_vis)
else(LINK_WITH_DLIBS)
	target_link_libraries(dbbgmo_pro bbgm bprb brdb bsta_algo bsta brip vidl vil vil_pro vsl)
endif(LINK_WITH_DLIBS)
if(LINK_WITH_DLIBS)
		add_definitions( 
		-DDEVELINCLUDEPATH
		)

if (BUILD_OCTI_TESTING OR BUILD_TESTING)
	subdirs(tests)
endif(BUILD_OCTI_TESTING OR BUILD_TESTING)



endif(LINK_WITH_DLIBS)

endif(EXPAT_FOUND)
