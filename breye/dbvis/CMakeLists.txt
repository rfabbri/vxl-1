# breyes/dbvis/CMakeLists.txt


include_directories( ${VXL_VGUI_INCLUDE_DIR} )

if( BUILD_EXAMPLES )
  subdirs( examples )
endif( BUILD_EXAMPLES )


