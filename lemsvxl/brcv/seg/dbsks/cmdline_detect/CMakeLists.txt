# dbsks/cmdline_detect/CMakeLists.txt
# Command line program for detecting objects in an image


if (VXL_VGUI_FOUND)

include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${VXL_GEL_INCLUDE_DIR} )

include_directories( ${LEMS_SOURCE_DIR}/brcv/shp )
include_directories( ${LEMS_SOURCE_DIR}/brcv/rec )
include_directories( ${LEMS_SOURCE_DIR}/brcv/seg )
include_directories( ${LEMS_SOURCE_DIR}/basic )
include_directories( ${LEMS_SOURCE_DIR}/breye1 )


set(dbsks_cmdline_detect_sources
   dbsks_cmdline_detect_main.cxx
   dbsks_cmdline_detect_utils.cxx    dbsks_cmdline_detect_utils.h
)

add_executable(dbsks_cmdline_detect ${dbsks_cmdline_detect_sources})

target_link_libraries(dbsks_cmdline_detect dbsks_algo dbsksp_xio dbsksp_pro dbsks_pro dbsksp_vis vgui dbgui buld)


endif (VXL_VGUI_FOUND)


