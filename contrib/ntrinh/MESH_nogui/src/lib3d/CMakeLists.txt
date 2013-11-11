# This is lib3d/CMakeLists.txt
# The 3d library for MESH_nogui

INCLUDE_DIRECTORIES(${MESH_nogui_SOURCE_DIR}/lib3d )

SET( lib3d_sources
geomutils.c         
geomutils.h
3dmodel.h
model_in.h          
model_in.c          
model_in_raw.c      
model_in_smf.c      
model_in_vrml_iv.c
model_in_ply.h  
model_in_ply.c      
model_in_off.c      
types.h             
block_list.c        
block_list.h        
debug_print.h     
ply.h
ply.c        
)

ADD_LIBRARY( lib3d  ${lib3d_sources} )

#TARGET_LINK_LIBRARIES( lib3d )

