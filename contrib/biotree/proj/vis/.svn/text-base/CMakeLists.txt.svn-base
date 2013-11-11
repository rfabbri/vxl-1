IF(VXL_VGUI_FOUND)
  INCLUDE( ${VXL_CMAKE_DIR}/UseBGUI3D.cmake)
ENDIF (VXL_VGUI_FOUND)

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/biotree/resc )

ADD_EXECUTABLE(bioproj_response_vis bioproj_response_vis.cxx)
TARGET_LINK_LIBRARIES(bioproj_response_vis biov bgui3d)

