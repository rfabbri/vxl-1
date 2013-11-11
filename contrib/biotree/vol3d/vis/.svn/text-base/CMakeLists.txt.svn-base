
IF(VXL_VGUI_FOUND)
  INCLUDE( ${VXL_CMAKE_DIR}/UseBGUI3D.cmake)
  INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR}/bbas )
  
  ADD_EXECUTABLE(vol3d_volume_vis vol3d_volume_vis.cxx )
  TARGET_LINK_LIBRARIES(vol3d_volume_vis vol3d vol3d_io biov imgr_file_format vbl vgl vcl vgui bsta)

ENDIF(VXL_VGUI_FOUND )



