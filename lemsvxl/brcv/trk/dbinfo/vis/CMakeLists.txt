# This is brcv/trk/dbinfo/vis/CMakeLists.txt
# Brown Tracking Visualization library

include_directories( ${LEMS_SOURCE_DIR}/breye1 )
#include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )

if (VXL_VGUI_FOUND)

set(dbinfo_vis_sources
   dbinfo_track_displayer.cxx           dbinfo_track_displayer.h
   dbinfo_osl_displayer.cxx             dbinfo_osl_displayer.h
   dbinfo_track_tools.cxx               dbinfo_track_tools.h
   dbinfo_osl_tools.cxx                 dbinfo_osl_tools.h
)


add_library(dbinfo_vis ${dbinfo_vis_sources})

target_link_libraries(dbinfo_vis dbinfo_pro dbinfo bvis1 vidpro1 bgui vgui )


#if(BUILD_TESTING)
#  add_subdirectory(tests)
#endif(BUILD_TESTING)


endif (VXL_VGUI_FOUND)
