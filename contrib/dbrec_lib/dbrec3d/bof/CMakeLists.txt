# This is contrib/dbrec3d/bof/CMakeLists.txt
# CMakeList for Brown Development 3d recognition library
# This library contains for bag of features recognition in boxm_scenes

OPTION( VXL_FORCE_V3P_RPLY "Use v3p ply library" YES)
MARK_AS_ADVANCED( VXL_FORCE_V3P_RPLY )

IF(VXL_EXPAT_FOUND)

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg/ )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/dbrec_lib/dbrec3d )
INCLUDE_DIRECTORIES( ${VXL_V3P_INCLUDE_DIR_deprecated}/rply/ )

SET(bof_sources
    bof_info.h                    bof_info.cxx
    bof_util.h                    bof_util.cxx
    bof_codebook.h                bof_codebook.cxx
    bof_scene_categories.h        bof_scene_categories.cxx
    bof_labels_keypoint_joint.h   bof_labels_keypoint_joint.cxx
    bof_class_codebook_util.h     bof_class_codebook_util.cxx

)

AUX_SOURCE_DIRECTORY(Templates bof_sources)

ADD_LIBRARY(bof ${bof_sources})

TARGET_LINK_LIBRARIES(bof bvpl_octree dbcll vbl vbl_io rply bsta bsta_vis)

SUBDIRS(pro)

IF( BUILD_TESTING )
#  SUBDIRS(tests)
ENDIF( BUILD_TESTING )

ENDIF(VXL_EXPAT_FOUND)


