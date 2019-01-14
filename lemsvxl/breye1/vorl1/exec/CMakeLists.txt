# breye1/vorl1/exec/CMakeLists.txt

include_directories( ${LEMS_SOURCE_DIR}/brcv/mvg )
include_directories( ${LEMS_SOURCE_DIR}/brcv/trk )
include_directories( ${LEMS_SOURCE_DIR}/brcv/rec )
include_directories( ${LEMS_SOURCE_DIR}/brcv/seg )

include_directories(${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories(${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories(${VXL_GEL_INCLUDE_DIR} )
include_directories(${VXL_OXL_INCLUDE_DIR} )
include_directories(${VXL_MUL_INCLUDE_DIR} )

add_executable(vorl1_dbctrk_exec vorl1_dbctrk_exec.cxx)
target_link_libraries(vorl1_dbctrk_exec vorl1 dbctrk dbctrk_pro)

#add_executable(vorl1_bmrf_exec vorl1_bmrf_exec.cxx)
#target_link_libraries(vorl1_bmrf_exec vorl1 dbmrf_pro)

add_executable(vorl1_compute_homography_exec vorl1_compute_homography_exec.cxx)
target_link_libraries(vorl1_compute_homography_exec vorl1 dbvrl_pro bsta )

add_executable(vorl1_mirec_exec vorl1_mirec_exec.cxx)
target_link_libraries(vorl1_mirec_exec vorl1 dbsk2d_algo dbru_pro dbru_algo)

add_executable(vorl1_fg_detect_exec vorl1_fg_detect_exec.cxx)
target_link_libraries(vorl1_fg_detect_exec vorl1 dbbgm_pro dbdet_pro vidpro1)

add_executable(vorl1_bgbuilder_exec vorl1_bgbuilder_exec.cxx)
target_link_libraries(vorl1_bgbuilder_exec vorl1 dbbgm_pro vidpro1)

# FIXME: This executable should not be here, it depends on contrib
#add_executable(vorl1_cri_curve vorl1_cri_curve.cxx)
#  target_link_libraries(vorl1_cri_curve  vorl1 dbcri_pro dbcri_accu_pro dbcri_accu_edge_pro  dbcri_whole_pro brct)

if(BUILD_CONTRIB_vehicleseg) 
  include_directories( ${LEMS_SOURCE_DIR}/contrib )
  include_directories( ${LEMS_SOURCE_DIR}/contrib/vehicleseg )
  add_executable(vorl1_fgdetect_blob_finder_exec vorl1_fgdetect_blob_finder_exec.cxx)
  target_link_libraries(vorl1_fgdetect_blob_finder_exec vorl1 dbdet_pro dbvrl_pro dbbgm_pro)

  add_executable(vorl1_fgdetect_con_finder_exec vorl1_fgdetect_con_finder_exec.cxx)
  target_link_libraries(vorl1_fgdetect_con_finder_exec vorl1 dbdet_pro  dbbgm_pro)

endif(BUILD_CONTRIB_vehicleseg)
