include_directories( ${LEMS_SOURCE_DIR} ${BIOTREE_SOURCE_DIR} ${VXL_DIR})

if(VXL_VGUI_FOUND)
   include( ${VXL_CMAKE_DIR}/UseBGUI3D.cmake)
endif (VXL_VGUI_FOUND)

   include_directories( ${VXL_MUL_INCLUDE_DIR} )

   add_executable(vol_file_write_read vol_file_write_read.cxx)
   target_link_libraries(vol_file_write_read vol_file bgui3d ${SIMVOLEON_LIBRARY})

   add_executable(vol_file_read vol_file_read.cxx)
   target_link_libraries(vol_file_read vol_file bgui3d ${SIMVOLEON_LIBRARY})
