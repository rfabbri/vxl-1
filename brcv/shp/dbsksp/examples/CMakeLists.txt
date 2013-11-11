# This is dbsks/examples/CMakeLists.txt
# Examples of using dbsksp library

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/trk )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/rec )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )
INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${VXL_GEL_INCLUDE_DIR} )

ADD_EXECUTABLE( screenshot_xgraph screenshot_xgraph.cxx )
TARGET_LINK_LIBRARIES( screenshot_xgraph dbsksp_algo dbsksp_xio )

ADD_EXECUTABLE( esf2xgraph convert_esf_to_xgraph_main.cxx )
TARGET_LINK_LIBRARIES( esf2xgraph dbsksp_pro dbsk2d_pro )

ADD_EXECUTABLE( average_xgraph average_two_xgraphs_main.cxx )
TARGET_LINK_LIBRARIES( average_xgraph dbsksp_pro )


IF (VXL_VGUI_FOUND)

# save xgraph to pdf file
ADD_EXECUTABLE( xgraph2ps xgraph2ps.cxx )
TARGET_LINK_LIBRARIES( xgraph2ps bvis1 dbsksp_pro dbsk2d_pro dbsksp_vis dbsk2d_vis dbgui)

ENDIF (VXL_VGUI_FOUND)
