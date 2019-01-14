# contrib/fine/algo/CMakeLists.txt

if (VXL_VGUI_FOUND)

include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_MUL_INCLUDE_DIR} )
include_directories( ${VXL_OXL_INCLUDE_DIR} )
include_directories( ${LEMS_SOURCE_DIR}/breye1 )
include_directories( ${LEMS_SOURCE_DIR}/basic )
include_directories( ${LEMS_SOURCE_DIR}/brcv/trk )
include_directories( ${LEMS_SOURCE_DIR}/brcv/seg )
include_directories( ${LEMS_SOURCE_DIR}/brcv/rec )
include_directories( ${LEMS_SOURCE_DIR}/brcv/shp )
include_directories( ${LEMS_SOURCE_DIR}/brcv/trk )


set(dborl_archaeology_algo_sources
    #dborl_archaeology_algo.cxx                dborl_archaeology_algo.h
    dborl_archaeology_algo_main.cxx
                                dborl_archaeology_algo_params.h
    vbl_smart_ptr+dborl_archaeology_algo_params-.cxx    dborl_archaeology_algo_params_sptr.h
    
)
add_executable(dborl_archaeology_algo ${dborl_archaeology_algo_sources})
target_link_libraries(dborl_archaeology_algo dborl_algo vidpro1 bpro1 bvis1 dbseg_pro dbseg_structure)

endif (VXL_VGUI_FOUND) 


