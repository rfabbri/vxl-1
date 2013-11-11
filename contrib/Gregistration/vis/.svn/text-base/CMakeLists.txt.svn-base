

INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/Gregistration )

SET( georegistration_vis_sources
dbrl_match_set_tableau.h        dbrl_match_set_tableau.cxx              dbrl_match_set_tableau_sptr.h
dbrl_match_set_displayer.h              dbrl_match_set_displayer.cxx
dbrl_id_point_2d_displayer.h    dbrl_id_point_2d_displayer.cxx
dbrl_rpm_tps_process_tool.h     dbrl_rpm_tps_process_tool.cxx
dbrl_id_point_2d_crop_tool.h    dbrl_id_point_2d_crop_tool.cxx
bvis_correspond_line_tool.h             bvis_correspond_line_tool.cxx
)


AUX_SOURCE_DIRECTORY(Templates georegistration_vis_sources)
ADD_LIBRARY( georegistration_vis  ${georegistration_vis_sources})
TARGET_LINK_LIBRARIES( georegistration_vis georegister bpro1 bvis1 vgui )

