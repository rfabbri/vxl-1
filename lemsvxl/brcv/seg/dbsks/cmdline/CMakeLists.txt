# This is brcv/seg/dbsks/bin/CMakeLists.txt
# Brown SKS Executables

include_directories( ${LEMS_SOURCE_DIR}/breye1 )
include_directories( ${LEMS_SOURCE_DIR}/brcv/trk )
include_directories( ${LEMS_SOURCE_DIR}/brcv/shp )
include_directories( ${LEMS_SOURCE_DIR}/brcv/rec )
include_directories( ${LEMS_SOURCE_DIR}/brcv/seg )
include_directories(${VXL_BRL_INCLUDE_DIR} )
include_directories(${VXL_GEL_INCLUDE_DIR} )


add_executable(dbsks_cmdline
    dbsks_cmdline_main.cxx
    dbsks_cmdline_utils.h       dbsks_cmdline_utils.cxx 
)
target_link_libraries( dbsks_cmdline dbsks_algo dbsks_xio dbsksp_xio buld bild_algo vul vil )
#dbsksp_pro dbsks_pro dbsksp_vis vgui dbgui buld  )

# An application to compute cost of a fragment as it translates / rotates in an image
add_executable(dbsks_xfrag_cost_map dbsks_xfrag_cost_map_main.cxx)
target_link_libraries( dbsks_xfrag_cost_map dbsks_algo dbsks_xio dbsksp_xio buld bild_algo vul vil  )
