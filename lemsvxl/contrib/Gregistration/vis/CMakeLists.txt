

include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${LEMS_SOURCE_DIR}/breye1 )
include_directories( ${LEMS_SOURCE_DIR}/basic )

include_directories( ${LEMS_SOURCE_DIR}/contrib/Gregistration )

set( georegistration_vis_sources
dbrl_match_set_tableau.h        dbrl_match_set_tableau.cxx              dbrl_match_set_tableau_sptr.h
dbrl_match_set_displayer.h              dbrl_match_set_displayer.cxx
dbrl_id_point_2d_displayer.h    dbrl_id_point_2d_displayer.cxx
dbrl_rpm_tps_process_tool.h     dbrl_rpm_tps_process_tool.cxx
dbrl_id_point_2d_crop_tool.h    dbrl_id_point_2d_crop_tool.cxx
bvis_correspond_line_tool.h             bvis_correspond_line_tool.cxx
)


aux_source_directory(Templates georegistration_vis_sources)
add_library( georegistration_vis  ${georegistration_vis_sources})
target_link_libraries( georegistration_vis georegister bpro1 bvis1 vgui )

