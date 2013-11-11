# This is brcv/seg/dbsks/bin/CMakeLists.txt
# Brown SKS Executables

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/trk )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/rec )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg )
INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${VXL_GEL_INCLUDE_DIR} )


ADD_EXECUTABLE(dbsks_cmdline
    dbsks_cmdline_main.cxx
    dbsks_cmdline_utils.h       dbsks_cmdline_utils.cxx 
)
TARGET_LINK_LIBRARIES( dbsks_cmdline dbsks_algo dbsks_xio dbsksp_xio dbul dbil_algo vul vil )
#dbsksp_pro dbsks_pro dbsksp_vis vgui dbgui dbul  )

# An application to compute cost of a fragment as it translates / rotates in an image
ADD_EXECUTABLE(dbsks_xfrag_cost_map dbsks_xfrag_cost_map_main.cxx)
TARGET_LINK_LIBRARIES( dbsks_xfrag_cost_map dbsks_algo dbsks_xio dbsksp_xio dbul dbil_algo vul vil  )
