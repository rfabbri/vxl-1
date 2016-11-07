# Brown "contrib" Bounding Box Optimization helper tools
# contrib/sadali/dbbbopt_vis/CMakeLists.txt



set(dbbbopt_vis_sources

dbvis1_polyg_select_img_tool.cxx   dbvis1_polyg_select_img_tool.h
dbvis1_polyg_select_tool.cxx   dbvis1_polyg_select_tool.h
   
)

aux_source_directory(Templates dbbbopt_vis_sources)

add_library(dbbbopt_vis  ${dbbbopt_vis_sources})

target_link_libraries(dbbbopt_vis vsol vgl)


