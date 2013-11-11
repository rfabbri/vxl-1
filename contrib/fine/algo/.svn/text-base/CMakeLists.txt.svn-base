# contrib/fine/algo/CMakeLists.txt

IF (VXL_VGUI_FOUND)

INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_OXL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/trk )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/rec )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/trk )


SET(dborl_archaeology_algo_sources
    #dborl_archaeology_algo.cxx                dborl_archaeology_algo.h
    dborl_archaeology_algo_main.cxx
                                dborl_archaeology_algo_params.h
    vbl_smart_ptr+dborl_archaeology_algo_params-.cxx    dborl_archaeology_algo_params_sptr.h
    
)
ADD_EXECUTABLE(dborl_archaeology_algo ${dborl_archaeology_algo_sources})
TARGET_LINK_LIBRARIES(dborl_archaeology_algo dborl_algo vidpro1 bpro1 bvis1 dbseg_pro dbseg_structure)

ENDIF (VXL_VGUI_FOUND) 


