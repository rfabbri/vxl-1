# This is MESH_nogui/src/mesh_error_lib/CMakeLists.txt
# The MESH_nogui project


include_directories(${MESH_nogui_SOURCE_DIR}/lib3d )
include_directories(${MESH_nogui_SOURCE_DIR}/mesh_error_lib )

set( mesh_error_lib_sources
colormap.h        
compute_error.c                 
compute_error.h
colormap.c        
model_analysis.h
reporting.c     
reporting.h     
model_analysis.c

xalloc.h
xalloc.c
            
)

add_library(mesh_error_lib ${mesh_error_lib_sources})
target_link_libraries( mesh_error_lib lib3d )


