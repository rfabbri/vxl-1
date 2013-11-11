# This is contrib/dbrec3d/CMakeLists.txt
# CMakeList for Brown Development 3d recognition library
# This library contains specialization of dbrec to 3d

IF(VXL_EXPAT_FOUND)
  INCLUDE_DIRECTORIES( ${VXL_EXPAT_INCLUDE_DIR} )
  MESSAGE("${VXL_EXPAT_INCLUDE_DIR}")
  INCLUDE_DIRECTORIES( ${VXL_CORE_INCLUDE_DIR} )
  INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc )
  INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
  INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
  INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/b3p/expatpp )

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/dbrec_lib )

SET(dbrec3d_sources
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

AUX_SOURCE_DIRECTORY(Templates dbrec3d_sources)

ADD_LIBRARY(dbrec3d ${dbrec3d_sources})

TARGET_LINK_LIBRARIES(dbrec3d bvpl_octree bsta bxml bvrml)

SUBDIRS(pro)
SUBDIRS(exe)
SUBDIRS(gui)
SUBDIRS(bof)
SUBDIRS(pcl)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )

ENDIF(VXL_EXPAT_FOUND)


