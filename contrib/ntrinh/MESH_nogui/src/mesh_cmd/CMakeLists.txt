# This is mesh/CMakeLists.txt
# The MESH_nogui project


include_directories(${MESH_nogui_SOURCE_DIR}/lib3d )
include_directories(${MESH_nogui_SOURCE_DIR}/mesh_error_lib )
include_directories(${MESH_nogui_SOURCE_DIR}/mesh_cmd )

set( mesh_cmd_sources
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

add_executable(mesh_cmd ${mesh_cmd_sources})
target_link_libraries( mesh_cmd lib3d mesh_error_lib )


