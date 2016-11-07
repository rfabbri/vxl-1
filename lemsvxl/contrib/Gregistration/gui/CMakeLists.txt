# contrib/vehicleseg/gui/CMakeLists.txt

if (VXL_VGUI_FOUND)

include_directories(${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories(${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories(${VXL_GEL_INCLUDE_DIR} )
include_directories(${VXL_OXL_INCLUDE_DIR} )
include_directories(${VXL_MUL_INCLUDE_DIR} )
include_directories(${VXL_SOURCE_DIR} )

include_directories( ${lems_SOURCE_DIR}/contrib/vehicleseg/pro )
include_directories( ${lems_SOURCE_DIR}/contrib/vehicleseg/vis)

include_directories( ${lems_SOURCE_DIR}/contrib/Gregistration )


include_directories( ${lems_SOURCE_DIR}/brcv/seg )
include_directories( ${lems_SOURCE_DIR}/brcv/mvg )
include_directories( ${lems_SOURCE_DIR}/brcv/rec )
include_directories( ${lems_SOURCE_DIR}/brcv/trk )
include_directories( ${lems_SOURCE_DIR}/basic )

include_directories( ${lems_SOURCE_DIR}/contrib )

set(georegister_gui_sources
   georegister_gui_menu.cxx       georegister_gui_menu.h
   georegister_gui_main.cxx
)

aux_source_directory(Templates georegister_gui_sources)

add_executable( georegister_gui ${georegister_gui_sources})


target_link_libraries( georegister_gui  bpro1  vidpro1  bvis1 georegistration_pro dbgl_algo brip  georegistration_vis dbvidl_pro  dbvrl_pro dbgl_algo  vsl dbil_pro dbbgm_vis dbbgm_pro dbdet_pro dbinfo_pro dbinfo_vis dbacm_pro dbsol_pro dbsta_algo dbdet_vis dbru_pro)


endif (VXL_VGUI_FOUND)

