# This is brcv/trk/dbetl/vis/CMakeLists.txt
# Brown Epipolar Tracking Visualization library

include_directories( ${LEMS_SOURCE_DIR}/breye1 )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc )

if (VXL_VGUI_FOUND)
find_package(OpenGL)
  

set(dbetl_vis_sources
   dbetl_episeg_tools.cxx        dbetl_episeg_tools.h
)

# Add the following files if bgui3d is available for 3D rendering
if (VXL_BGUI3D_FOUND)
set(dbetl_vis_sources ${dbetl_vis_sources}
   dbetl_track_displayer.cxx     dbetl_track_displayer.h
)
endif (VXL_BGUI3D_FOUND)

add_definitions ( -DCOIN_NOT_DLL )
include_directories( ${VXL_COIN3D_INCLUDE_DIR} )

#aux_source_directory(Templates dbetl_vis_sources)

add_library(dbetl_vis ${dbetl_vis_sources})

target_link_libraries(dbetl_vis dbetl_pro becld_vis bvis1 vgui ${OPENGL_LIBRARIES})

# Add the bgui3d library if bgui3d is built is available for 3D rendering
if (VXL_BGUI3D_FOUND)
  target_link_libraries(dbetl_vis bgui3d ${VXL_COIN3D_LIBRARY} vpgl)
endif (VXL_BGUI3D_FOUND)


if(BUILD_TESTING)
#  add_subdirectory(tests)
endif(BUILD_TESTING)


endif (VXL_VGUI_FOUND)
