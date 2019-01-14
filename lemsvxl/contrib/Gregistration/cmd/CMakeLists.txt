# contrib/Gregistration/cmd/CMakeLists.txt

include_directories(${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories(${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories(${VXL_GEL_INCLUDE_DIR} )
include_directories(${VXL_OXL_INCLUDE_DIR} )
include_directories(${VXL_MUL_INCLUDE_DIR} )
#include_directories(${VXL_SOURCE_DIR} )

#include_directories( ${LEMS_SOURCE_DIR}/contrib/vehicleseg/pro )
#include_directories( ${LEMS_SOURCE_DIR}/contrib/vehicleseg/vis)

include_directories( ${LEMS_SOURCE_DIR}/contrib/Gregistration )


include_directories( ${LEMS_SOURCE_DIR}/brcv/seg )
include_directories( ${LEMS_SOURCE_DIR}/brcv/mvg )
include_directories( ${LEMS_SOURCE_DIR}/brcv/rec )
include_directories( ${LEMS_SOURCE_DIR}/brcv/trk )
include_directories( ${LEMS_SOURCE_DIR}/basic )

#include_directories( ${LEMS_SOURCE_DIR}/contrib )

set(georegister_cmd_sources 
	georegister_cmd_main.cxx georegister_cmd_params_sptr.h
	georegister_cmd_params.h georegister_cmd_params.cxx
	vbl_smart_ptr+georegister_cmd_params-.cxx
)

#aux_source_directory(Templates georegister_gui_sources)

add_executable( georegister_cmd ${georegister_cmd_sources})


#target_link_libraries( georegister_gui  bpro1  vidpro1  bvis1 georegistration_pro dbgl_algo brip  georegistration_vis dbvidl_pro  dbvrl_pro dbgl_algo  vsl dbil_pro dbbgm_vis dbbgm_pro dbdet_pro dbinfo_pro dbinfo_vis dbacm_pro dbsol_pro dbsta_algo dbdet_vis dbru_pro)

target_link_libraries( georegister_cmd  bpro1  vidpro1 georegistration_pro dbgl_algo brip dbvidl_pro dbvrl_pro dbgl_algo vsl dbil_pro dbbgm_vis dbbgm_pro dbdet_pro dbinfo_pro dbacm_pro dbsol_pro dbsta_algo dbru_pro dborl_algo)
