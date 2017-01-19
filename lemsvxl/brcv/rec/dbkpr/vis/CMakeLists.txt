# This is brcv/rec/dbkpr/vis/CMakeLists.txt
# Brown Keypoint Recognition Visualization Library

include_directories( ${lems_SOURCE_DIR}/breye1 )
include_directories( ${lems_SOURCE_DIR}/bmvgd )
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )

if (VXL_VGUI_FOUND)

set(dbkpr_vis_sources
   dbkpr_keypoint_tools.cxx        dbkpr_keypoint_tools.h
)

if (VXL_BGUI3D_FOUND)

add_definitions ( -DCOIN_NOT_DLL )
include_directories( ${VXL_COIN3D_INCLUDE_DIR} )

set(dbkpr_vis_sources ${dbkpr_vis_sources}
   dbkpr_corr3d_displayer.cxx      dbkpr_corr3d_displayer.h
)
endif (VXL_BGUI3D_FOUND)


#aux_source_directory(Templates dbkpr_vis_sources)

add_library(dbkpr_vis ${dbkpr_vis_sources})

target_link_libraries(dbkpr_vis dbkpr_pro dbdet_vis)

# Add the bgui3d library if bgui3d is built is available for 3D rendering
if (VXL_BGUI3D_FOUND)
  target_link_libraries(dbkpr_vis bgui3d)
endif (VXL_BGUI3D_FOUND)


if(BUILD_TESTING)
#  add_subdirectory(tests)
endif(BUILD_TESTING)


endif (VXL_VGUI_FOUND)
