# This is contrib/dbrec3d/CMakeLists.txt
# CMakeList for Brown Development 3d recognition library
# This library contains specialization of dbrec to 3d

if(VXL_EXPAT_FOUND)
  include_directories( ${VXL_EXPAT_INCLUDE_DIR} )
  message("${VXL_EXPAT_INCLUDE_DIR}")
  include_directories( ${VXL_CORE_INCLUDE_DIR} )
  include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc )
  include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
  include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
  include_directories( ${VXL_BRL_INCLUDE_DIR}/b3p/expatpp )

include_directories( ${LEMS_SOURCE_DIR}/contrib/dbrec_lib )

set(dbrec3d_sources
    dbrec3d_part.h                            dbrec3d_part.cxx                       
    dbrec3d_composite_part.h                  dbrec3d_composite_part.cxx
    dbrec3d_primitive_kernel_part.h           dbrec3d_primitive_kernel_part.cxx
    dbrec3d_pairwise_compositor.h             dbrec3d_pairwise_compositor.cxx
    dbrec3d_part_instance.h                   dbrec3d_part_instance.cxx
    dbrec3d_parts_manager.h                   dbrec3d_parts_manager.cxx
    dbrec3d_octree_context.h                  dbrec3d_octree_context.cxx
    dbrec3d_context_manager.h                 dbrec3d_context_manager.cxx
    dbrec3d_models.h                          dbrec3d_models.cxx
    dbrec3d_local_max_functor.h
    dbrec3d_kernel_max_functor.h
    dbrec3d_io_visitors.h                     dbrec3d_io_visitors.cxx
    dbrec3d_visitor.h
    dbrec3d_vrml_visitor.h
    dbrec3d_scene_utils.h                     dbrec3d_scene_utils.cxx
)

aux_source_directory(Templates dbrec3d_sources)

add_library(dbrec3d ${dbrec3d_sources})

target_link_libraries(dbrec3d bvpl_octree bsta bxml bvrml)

add_subdirectory(pro)
add_subdirectory(exe)
add_subdirectory(gui)
add_subdirectory(bof)
add_subdirectory(pcl)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif( BUILD_TESTING )

endif(VXL_EXPAT_FOUND)


