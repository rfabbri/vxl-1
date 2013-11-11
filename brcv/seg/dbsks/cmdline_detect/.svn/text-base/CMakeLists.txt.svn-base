# dbsks/cmdline_detect/CMakeLists.txt
# Command line program for detecting objects in an image


IF (VXL_VGUI_FOUND)

INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/rec )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )


SET(dbsks_cmdline_detect_sources
   dbsks_cmdline_detect_main.cxx
   dbsks_cmdline_detect_utils.cxx    dbsks_cmdline_detect_utils.h
)

ADD_EXECUTABLE(dbsks_cmdline_detect ${dbsks_cmdline_detect_sources})

TARGET_LINK_LIBRARIES(dbsks_cmdline_detect dbsks_algo dbsksp_xio dbsksp_pro dbsks_pro dbsksp_vis vgui dbgui dbul)


ENDIF (VXL_VGUI_FOUND)


