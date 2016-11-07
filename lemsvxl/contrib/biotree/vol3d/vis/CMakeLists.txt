
if(VXL_VGUI_FOUND)
  include( ${VXL_CMAKE_DIR}/UseBGUI3D.cmake)
  include_directories(${VXL_BRL_INCLUDE_DIR}/bbas )
  
  add_executable(vol3d_volume_vis vol3d_volume_vis.cxx )
  target_link_libraries(vol3d_volume_vis vol3d vol3d_io biov imgr_file_format vbl vgl vcl vgui bsta)

endif(VXL_VGUI_FOUND )



