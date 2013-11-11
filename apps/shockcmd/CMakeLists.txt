# lemsvxlsrc/apps/shockcmd/CMakeLists.txt

SET(shockcmd_sources
   dbvis_shockcmd.cxx
   opt.h                opt.cpp   
)
AUX_SOURCE_DIRECTORY(Templates shockcmd_sources)
ADD_EXECUTABLE(shockcmd ${shockcmd_sources})

TARGET_LINK_LIBRARIES(shockcmd shock extrautils)
