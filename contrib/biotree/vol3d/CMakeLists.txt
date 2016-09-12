# created by K. Kang for holding all the filters to 3d volume data

include_directories( ${VXL_MUL_INCLUDE_DIR} ${VXL_GEL_INCLUDE_DIR}/mrc )
include_directories(${biotree_SOURCE_DIR}/resc)
include_directories(${lems_SOURCE_DIR}/basic)

set(vol3d_src
    vol3d_radius_filter.cxx                     vol3d_radius_filter.h
    vol3d_gaussian_kernel_3d.cxx                vol3d_gaussian_kernel_3d.h
)

aux_source_directory(Templates vol3d_src)
add_library(vol3d ${vol3d_src})

target_link_libraries(vol3d vcl vbl vil3d imgr biob xmvg dbil)

subdirs(algo)

subdirs(vis)

#if(BUILD_TESTING)
#  subdirs(tests)
#endif(BUILD_TESTING)
#
#if(BUILD_EXAMPLES)
#  subdirs(examples)
#endif(BUILD_EXAMPLES)
