# This is brcv/shp/dbsk3d/pro/CMakeLists.txt
# Brown Shock3D Processing library

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/shp)
INCLUDE_DIRECTORIES( ${VXL_RPL_INCLUDE_DIR} )

SET(dbsk3d_pro_sources

    dbsk3d_cmdpara.cxx       dbsk3d_cmdpara.h
    dbsk3d_process.cxx       dbsk3d_process.h
    dbsk3d_pro_bktseg.cxx
)


ADD_LIBRARY(dbsk3d_pro ${dbsk3d_pro_sources})

TARGET_LINK_LIBRARIES(dbsk3d_pro dbmsh3d dbmsh3d_algo dbsk3d dbsk3d_algo dbsk3d_flowsheet )

