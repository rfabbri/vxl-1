# breyes/bvis/examples/CMakeLists.txt


IF (VXL_VGUI_FOUND)

SET(brown_eyes_sources

   bvis_brown_eyes_menu.cxx       bvis_brown_eyes_menu.h
   bvis_brown_eyes_main.cxx
)

AUX_SOURCE_DIRECTORY(Templates bvis_sources)

ADD_EXECUTABLE(brown_eyes ${brown_eyes_sources})

TARGET_LINK_LIBRARIES(brown_eyes bvis bpro vbl_io vbl)

ENDIF (VXL_VGUI_FOUND)
