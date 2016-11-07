# This is brcv/shp/dbsk3d/pro/CMakeLists.txt
# Brown Shock3D Processing library

include_directories( ${lems_SOURCE_DIR}/shp)
include_directories( ${VXL_RPL_INCLUDE_DIR} )

set(dbsk3d_pro_sources

    dbsk3d_cmdpara.cxx       dbsk3d_cmdpara.h
    dbsk3d_process.cxx       dbsk3d_process.h
    dbsk3d_pro_bktseg.cxx
)


add_library(dbsk3d_pro ${dbsk3d_pro_sources})

target_link_libraries(dbsk3d_pro dbmsh3d dbmsh3d_algo dbsk3d dbsk3d_algo dbsk3d_flowsheet )

