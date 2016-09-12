# This is dbsks/examples/CMakeLists.txt
# Examples of using dbsksp library

include_directories( ${lems_SOURCE_DIR}/breye1 )
include_directories( ${lems_SOURCE_DIR}/brcv/trk )
include_directories( ${lems_SOURCE_DIR}/brcv/shp )
include_directories( ${lems_SOURCE_DIR}/brcv/rec )
include_directories( ${lems_SOURCE_DIR}/brcv/seg )
include_directories( ${lems_SOURCE_DIR}/basic )
include_directories(${VXL_BRL_INCLUDE_DIR} )
include_directories(${VXL_GEL_INCLUDE_DIR} )

add_executable( screenshot_xgraph screenshot_xgraph.cxx )
target_link_libraries( screenshot_xgraph dbsksp_algo dbsksp_xio )

add_executable( esf2xgraph convert_esf_to_xgraph_main.cxx )
target_link_libraries( esf2xgraph dbsksp_pro dbsk2d_pro )

add_executable( average_xgraph average_two_xgraphs_main.cxx )
target_link_libraries( average_xgraph dbsksp_pro )


if (VXL_VGUI_FOUND)

# save xgraph to pdf file
add_executable( xgraph2ps xgraph2ps.cxx )
target_link_libraries( xgraph2ps bvis1 dbsksp_pro dbsk2d_pro dbsksp_vis dbsk2d_vis dbgui)

endif (VXL_VGUI_FOUND)
