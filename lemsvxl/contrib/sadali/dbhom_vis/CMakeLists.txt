# Brown "contrib" Homography Calculation Tool
# contrib/sadali/dbhom_vis/CMakeLists.txt



set(dbhom_vis_sources

dbvis1_homog_view_tools.cxx   dbvis1_homog_view_tools.h

   
)

aux_source_directory(Templates dbhom_vis_sources)

add_library(dbhom_vis  ${dbhom_vis_sources})

target_link_libraries(dbhom_vis_sources)


