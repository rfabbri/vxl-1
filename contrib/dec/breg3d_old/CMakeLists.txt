INCLUDE_DIRECTORIES( ${VXLCORE_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/mvg )


ADD_LIBRARY( breg3d
             breg3d_voxel_grid.h   breg3d_voxel_grid.cxx
             breg3d_proj_camera_cost_function.h  breg3d_proj_camera_cost_function.cxx
             #breg3d_image_cost_function.h   breg3d_image_cost_function.cxx
             breg3d_util.h  breg3d_util.cxx
           )
           
TARGET_LINK_LIBRARIES( breg3d vil vil3d vgl vpgl vimt brip vnl)

SUBDIRS(exe)







                               
