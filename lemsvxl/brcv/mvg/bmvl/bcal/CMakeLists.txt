# multi-view camera library
# brl/bmvl/bcal/CMakeLists.txt

include_directories(${VXL_BRL_INCLUDE_DIR})
include_directories(${lems_SOURCE_DIR}/brcv/mvg/bmvl )

set(bcal_sources
    bcal_camera.h                   bcal_camera.cxx
    bcal_camera_node.h              bcal_camera_node.cxx
    bcal_euclidean_transformation.h bcal_euclidean_transformation.cxx
    bcal_camera_graph.h
    bcal_calibrate_object.h
    bcal_calibrate_plane.h          bcal_calibrate_plane.cxx
    bcal_zhang_camera_node.h        bcal_zhang_camera_node.cxx
    bcal_zhang_linear_calibrate.h   bcal_zhang_linear_calibrate.cxx
)

add_library(bcal ${bcal_sources})
install_targets(/lib bcal)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bmvl/bcal ${bcal_sources})
target_link_libraries(bcal vgl_algo vnl_algo vnl vgl vcsl)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif(BUILD_TESTING)
