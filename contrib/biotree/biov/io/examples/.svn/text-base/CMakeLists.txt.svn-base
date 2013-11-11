INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR} ${BIOTREE_SOURCE_DIR} ${VXL_DIR})

IF(VXL_VGUI_FOUND)
   INCLUDE( ${VXL_CMAKE_DIR}/UseBGUI3D.cmake)
ENDIF (VXL_VGUI_FOUND)

   INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR} )

   ADD_EXECUTABLE(vol_file_write_read vol_file_write_read.cxx)
   TARGET_LINK_LIBRARIES(vol_file_write_read vol_file bgui3d ${SIMVOLEON_LIBRARY})

   ADD_EXECUTABLE(vol_file_read vol_file_read.cxx)
   TARGET_LINK_LIBRARIES(vol_file_read vol_file bgui3d ${SIMVOLEON_LIBRARY})
