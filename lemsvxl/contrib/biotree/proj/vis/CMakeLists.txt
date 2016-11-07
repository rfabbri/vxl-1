if(VXL_VGUI_FOUND)
  include( ${VXL_CMAKE_DIR}/UseBGUI3D.cmake)
endif (VXL_VGUI_FOUND)

include_directories( ${lems_SOURCE_DIR}/basic )
include_directories( ${lems_SOURCE_DIR}/contrib/biotree/resc )

add_executable(bioproj_response_vis bioproj_response_vis.cxx)
target_link_libraries(bioproj_response_vis biov bgui3d)

