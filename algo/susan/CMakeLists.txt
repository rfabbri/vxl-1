# susan library
# algo/susan/CMakeLists.txt

INCLUDE_DIRECTORIES(${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR}/bbas )

SET(susan_sources

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

AUX_SOURCE_DIRECTORY(Templates susan_sources)

ADD_LIBRARY(susan  ${susan_sources})

TARGET_LINK_LIBRARIES(susan )

