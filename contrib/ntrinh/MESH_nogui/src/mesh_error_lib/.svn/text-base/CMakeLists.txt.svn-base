# This is MESH_nogui/src/mesh_error_lib/CMakeLists.txt
# The MESH_nogui project


INCLUDE_DIRECTORIES(${MESH_nogui_SOURCE_DIR}/lib3d )
INCLUDE_DIRECTORIES(${MESH_nogui_SOURCE_DIR}/mesh_error_lib )

SET( mesh_error_lib_sources
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

ADD_LIBRARY(mesh_error_lib ${mesh_error_lib_sources})
TARGET_LINK_LIBRARIES( mesh_error_lib lib3d )


