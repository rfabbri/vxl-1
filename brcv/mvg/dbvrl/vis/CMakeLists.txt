# This is brcv/mvg/dbvrl/vis/CMakeLists.txt
# Brown Video Registration Visualization library

include_directories( ${lems_SOURCE_DIR}/breye1 )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )

if (VXL_VGUI_FOUND)

set(dbvrl_vis_sources
   dbvrl_region_tableau.cxx      dbvrl_region_tableau.h       dbvrl_region_tableau_sptr.h
   dbvrl_region_displayer.cxx    dbvrl_region_displayer.h
   dbvrl_region_tools.cxx        dbvrl_region_tools.h
)

#aux_source_directory(Templates dbvrl_vis_sources)

add_library(dbvrl_vis ${dbvrl_vis_sources})

target_link_libraries(dbvrl_vis dbvrl_pro bvis1)


if(BUILD_TESTING)
#  subdirs(tests)
endif(BUILD_TESTING)


endif (VXL_VGUI_FOUND)
