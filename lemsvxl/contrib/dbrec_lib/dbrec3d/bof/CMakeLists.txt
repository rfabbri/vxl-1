# This is contrib/dbrec3d/bof/CMakeLists.txt
# CMakeList for Brown Development 3d recognition library
# This library contains for bag of features recognition in boxm_scenes

option( VXL_FORCE_V3P_RPLY "Use v3p ply library" YES)
mark_as_advanced( VXL_FORCE_V3P_RPLY )

if(VXL_EXPAT_FOUND)

include_directories( ${LEMS_SOURCE_DIR}/brcv/seg/ )
include_directories( ${LEMS_SOURCE_DIR}/contrib/dbrec_lib/dbrec3d )
include_directories( ${VXL_V3P_INCLUDE_DIR_deprecated}/rply/ )

set(bof_sources
    bof_info.h                    bof_info.cxx
    bof_util.h                    bof_util.cxx
    bof_codebook.h                bof_codebook.cxx
    bof_scene_categories.h        bof_scene_categories.cxx
    bof_labels_keypoint_joint.h   bof_labels_keypoint_joint.cxx
    bof_class_codebook_util.h     bof_class_codebook_util.cxx

)

aux_source_directory(Templates bof_sources)

add_library(bof ${bof_sources})

target_link_libraries(bof bvpl_octree dbcll vbl vbl_io rply bsta bsta_vis)

add_subdirectory(pro)

if( BUILD_TESTING )
#  add_subdirectory(tests)
endif( BUILD_TESTING )

endif(VXL_EXPAT_FOUND)


