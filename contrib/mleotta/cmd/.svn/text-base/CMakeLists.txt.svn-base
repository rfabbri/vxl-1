# contrib/mleotta/cmd/CMakeLists.txt
# Command line programs for mleotta

#INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/mvg )
#INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/trk )
#INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/rec )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc )
#INCLUDE_DIRECTORIES(${VXL_OXL_INCLUDE_DIR} )
#INCLUDE_DIRECTORIES(${VXL_MUL_INCLUDE_DIR} )


ADD_EXECUTABLE(modrec_exec modrec_exec.cxx)
TARGET_LINK_LIBRARIES(modrec_exec modrec_pro dbbgm_pro vorl1 vidpro1 bpro1 vpgl)

ADD_EXECUTABLE(modrec_seg_exec modrec_seg_exec.cxx)
TARGET_LINK_LIBRARIES(modrec_seg_exec modrec_pro dbbgm_pro vorl1 vidpro1 bpro1 vpgl)

ADD_EXECUTABLE(keypoint2feat3d keypoint2feat3d.cxx)
TARGET_LINK_LIBRARIES( keypoint2feat3d dbpro dbdet modrec_io)

ADD_EXECUTABLE(extract_patches extract_patches.cxx)
TARGET_LINK_LIBRARIES(extract_patches dbpro dbvidl2_pro bil_algo dbdet modrec_io)

ADD_EXECUTABLE(cluster_features_3d cluster_features_3d.cxx)
TARGET_LINK_LIBRARIES(cluster_features_3d modrec_io vul dbcll)

ADD_EXECUTABLE(cluster_position cluster_position.cxx)
TARGET_LINK_LIBRARIES(cluster_position modrec_io vul dbcll)

ADD_EXECUTABLE(cluster2vrml cluster2vrml.cxx)
TARGET_LINK_LIBRARIES(cluster2vrml modrec_io vul dbcll)

ADD_EXECUTABLE(cluster2patchimg cluster2patchimg.cxx)
TARGET_LINK_LIBRARIES(cluster2patchimg vul vil vsl)

ADD_EXECUTABLE(learn_patch_basis learn_patch_basis.cxx)
TARGET_LINK_LIBRARIES(learn_patch_basis vul vil vsl vnl_algo)

ADD_EXECUTABLE(cluster2mat cluster2mat.cxx)
TARGET_LINK_LIBRARIES(cluster2mat modrec_io vul dbcll)

ADD_EXECUTABLE(recluster_position recluster_position.cxx)
TARGET_LINK_LIBRARIES(recluster_position modrec_io vul dbcll)

ADD_EXECUTABLE(recluster_sim3d recluster_sim3d.cxx)
TARGET_LINK_LIBRARIES(recluster_sim3d modrec_io vul dbcll)

ADD_EXECUTABLE(trace2clusters trace2clusters.cxx)
TARGET_LINK_LIBRARIES(trace2clusters modrec_io vul dbcll)

ADD_EXECUTABLE(cluster2codebook cluster2codebook.cxx)
TARGET_LINK_LIBRARIES(cluster2codebook modrec_io vul dbcll)

SUBDIRS(mesh)
