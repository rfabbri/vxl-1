# contrib/mleotta/cmd/CMakeLists.txt
# Command line programs for mleotta

#include_directories( ${LEMS_SOURCE_DIR}/brcv/mvg )
#include_directories( ${LEMS_SOURCE_DIR}/brcv/trk )
#include_directories( ${LEMS_SOURCE_DIR}/brcv/rec )
include_directories( ${LEMS_SOURCE_DIR}/brcv/seg )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc )
#include_directories(${VXL_OXL_INCLUDE_DIR} )
#include_directories(${VXL_MUL_INCLUDE_DIR} )


add_executable(modrec_exec modrec_exec.cxx)
target_link_libraries(modrec_exec modrec_pro dbbgm_pro vorl1 vidpro1 bpro1 vpgl)

add_executable(modrec_seg_exec modrec_seg_exec.cxx)
target_link_libraries(modrec_seg_exec modrec_pro dbbgm_pro vorl1 vidpro1 bpro1 vpgl)

add_executable(keypoint2feat3d keypoint2feat3d.cxx)
target_link_libraries( keypoint2feat3d dbpro dbdet modrec_io)

add_executable(extract_patches extract_patches.cxx)
target_link_libraries(extract_patches dbpro dbvidl2_pro bil_algo dbdet modrec_io)

add_executable(cluster_features_3d cluster_features_3d.cxx)
target_link_libraries(cluster_features_3d modrec_io vul dbcll)

add_executable(cluster_position cluster_position.cxx)
target_link_libraries(cluster_position modrec_io vul dbcll)

add_executable(cluster2vrml cluster2vrml.cxx)
target_link_libraries(cluster2vrml modrec_io vul dbcll)

add_executable(cluster2patchimg cluster2patchimg.cxx)
target_link_libraries(cluster2patchimg vul vil vsl)

add_executable(learn_patch_basis learn_patch_basis.cxx)
target_link_libraries(learn_patch_basis vul vil vsl vnl_algo)

add_executable(cluster2mat cluster2mat.cxx)
target_link_libraries(cluster2mat modrec_io vul dbcll)

add_executable(recluster_position recluster_position.cxx)
target_link_libraries(recluster_position modrec_io vul dbcll)

add_executable(recluster_sim3d recluster_sim3d.cxx)
target_link_libraries(recluster_sim3d modrec_io vul dbcll)

add_executable(trace2clusters trace2clusters.cxx)
target_link_libraries(trace2clusters modrec_io vul dbcll)

add_executable(cluster2codebook cluster2codebook.cxx)
target_link_libraries(cluster2codebook modrec_io vul dbcll)

add_subdirectory(mesh)
