# This is mesh/CMakeLists.txt
# The MESH_nogui project


INCLUDE_DIRECTORIES(${MESH_nogui_SOURCE_DIR}/lib3d )
INCLUDE_DIRECTORIES(${MESH_nogui_SOURCE_DIR}/mesh_error_lib )
INCLUDE_DIRECTORIES(${MESH_nogui_SOURCE_DIR}/mesh_cmd )

SET( mesh_cmd_sources
# colormap.h        
# compute_error.c                 
# compute_error.h
# colormap.c        
# model_analysis.h
# reporting.c     
# reporting.h     
# model_analysis.c

mesh.h              
mesh_run.c          
mesh_run.h          
mesh.cpp

# xalloc.h
# xalloc.c
            
)

ADD_EXECUTABLE(mesh_cmd ${mesh_cmd_sources})
TARGET_LINK_LIBRARIES( mesh_cmd lib3d mesh_error_lib )


