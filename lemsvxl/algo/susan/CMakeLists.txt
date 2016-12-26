# susan library
# algo/susan/CMakeLists.txt

include_directories(${VXL_GEL_INCLUDE_DIR} )
include_directories(${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories(${VXL_BRL_INCLUDE_DIR}/bbas )

set(susan_sources

susan_main.c 
edge_draw.c  
enlarge.c 
susan_corners.c 
susan_edges.c 
susan_edges_small.c 
int_to_char.c 
median.c 
susan_smooth.c 
susan_thin.c 
setup_brightness_lut.c 
susan_sub.c 
susan_edges_medium.c 
susan_sub_function.c 

)

aux_source_directory(Templates susan_sources)

add_library(susan  ${susan_sources})

target_link_libraries(susan )

