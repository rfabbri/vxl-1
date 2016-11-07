# This is brcv/trk/dbmrf/vis/CMakeLists.txt
# Brown Markov Random Field Track Visualization Library

include_directories( ${lems_SOURCE_DIR}/breye1 )
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc )

if (VXL_VGUI_FOUND)

set(dbmrf_vis_sources
   dbmrf_bmrf_tools.cxx        dbmrf_bmrf_tools.h
   dbmrf_bmrf_displayer.cxx    dbmrf_bmrf_displayer.h
)

# Add the following files if bgui3d is available for 3D rendering
if (VXL_BGUI3D_FOUND)

add_definitions ( -DCOIN_NOT_DLL )
include_directories( ${VXL_COIN3D_INCLUDE_DIR} )

set(dbmrf_vis_sources ${dbmrf_vis_sources}
   dbmrf_3d_picker_tool.cxx         dbmrf_3d_picker_tool.h
   dbmrf_curvel_3d_tools.cxx        dbmrf_curvel_3d_tools.h
   dbmrf_curvel_3d_displayer.cxx    dbmrf_curvel_3d_displayer.h
   SoCurvel3D.cxx                   SoCurvel3D.h
)
endif (VXL_BGUI3D_FOUND)


#aux_source_directory(Templates dbmrf_vis_sources)

add_library(dbmrf_vis ${dbmrf_vis_sources})

target_link_libraries(dbmrf_vis dbmrf_pro bvis1)

# Add the bgui3d library if bgui3d is built is available for 3D rendering
if (VXL_BGUI3D_FOUND)
  target_link_libraries(dbmrf_vis bgui3d)
endif (VXL_BGUI3D_FOUND)


if(BUILD_TESTING)
#  add_subdirectory(tests)
endif(BUILD_TESTING)


endif (VXL_VGUI_FOUND)
