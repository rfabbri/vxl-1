# lemsvxlsrc/apps/shockcmd/CMakeLists.txt

set(shockcmd_sources
   dbvis_shockcmd.cxx
   opt.h                opt.cpp   
)
aux_source_directory(Templates shockcmd_sources)
add_executable(shockcmd ${shockcmd_sources})

target_link_libraries(shockcmd shock extrautils)
