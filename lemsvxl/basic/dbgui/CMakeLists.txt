# This is basic/dbgui/CMakeLists.txt
# Development Brown GUI Library

include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc )
include_directories(${VXD_BRLD_INCLUDE_DIR})

if (VXL_VGUI_FOUND)

set(dbgui_sources
   dbgui_utils.cxx                 dbgui_utils.h
   dbgui_soview2D.cxx              dbgui_soview2D.h

   dbgui_gnd_cal_tableau.cxx       dbgui_gnd_cal_tableau.h      dbgui_gnd_cal_tableau_sptr.h
)

aux_source_directory(Templates dbgui_sources)

add_library(dbgui  ${dbgui_sources})

target_link_libraries(dbgui vgui gl2ps bgld_algo vpgl_algo )

if( BUILD_EXAMPLES )
  add_subdirectory( examples )
endif( BUILD_EXAMPLES )

endif (VXL_VGUI_FOUND)
