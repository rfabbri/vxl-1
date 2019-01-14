include_directories( ${VXLCORE_INCLUDE_DIR} )
include_directories( ${VXL_MUL_INCLUDE_DIR} )
include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )

include_directories( ${LEMS_SOURCE_DIR}/brcv/mvg )


add_library( breg3d
             breg3d_voxel_grid.h   breg3d_voxel_grid.cxx
             breg3d_proj_camera_cost_function.h  breg3d_proj_camera_cost_function.cxx
             #breg3d_image_cost_function.h   breg3d_image_cost_function.cxx
             breg3d_util.h  breg3d_util.cxx
           )
           
target_link_libraries( breg3d vil vil3d vgl vpgl vimt brip vnl)

add_subdirectory(exe)







                               
