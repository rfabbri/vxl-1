# breye1/vorl1/exec/CMakeLists.txt

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/mvg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/trk )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/rec )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg )

INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES(${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${VXL_OXL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${VXL_MUL_INCLUDE_DIR} )

ADD_EXECUTABLE(vorl1_dbctrk_exec vorl1_dbctrk_exec.cxx)
TARGET_LINK_LIBRARIES(vorl1_dbctrk_exec vorl1 dbctrk dbctrk_pro)

#ADD_EXECUTABLE(vorl1_bmrf_exec vorl1_bmrf_exec.cxx)
#TARGET_LINK_LIBRARIES(vorl1_bmrf_exec vorl1 dbmrf_pro)

ADD_EXECUTABLE(vorl1_compute_homography_exec vorl1_compute_homography_exec.cxx)
TARGET_LINK_LIBRARIES(vorl1_compute_homography_exec vorl1 dbvrl_pro bsta )

ADD_EXECUTABLE(vorl1_mirec_exec vorl1_mirec_exec.cxx)
TARGET_LINK_LIBRARIES(vorl1_mirec_exec vorl1 dbsk2d_algo dbru_pro dbru_algo)

ADD_EXECUTABLE(vorl1_fg_detect_exec vorl1_fg_detect_exec.cxx)
TARGET_LINK_LIBRARIES(vorl1_fg_detect_exec vorl1 dbbgm_pro dbdet_pro vidpro1)

ADD_EXECUTABLE(vorl1_bgbuilder_exec vorl1_bgbuilder_exec.cxx)
TARGET_LINK_LIBRARIES(vorl1_bgbuilder_exec vorl1 dbbgm_pro vidpro1)

# FIXME: This executable should not be here, it depends on contrib
#ADD_EXECUTABLE(vorl1_cri_curve vorl1_cri_curve.cxx)
#  TARGET_LINK_LIBRARIES(vorl1_cri_curve  vorl1 dbcri_pro dbcri_accu_pro dbcri_accu_edge_pro  dbcri_whole_pro brct)

IF(BUILD_CONTRIB_vehicleseg) 
  INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib )
  INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/vehicleseg )
  ADD_EXECUTABLE(vorl1_fgdetect_blob_finder_exec vorl1_fgdetect_blob_finder_exec.cxx)
  TARGET_LINK_LIBRARIES(vorl1_fgdetect_blob_finder_exec vorl1 dbdet_pro dbvrl_pro dbbgm_pro)

  ADD_EXECUTABLE(vorl1_fgdetect_con_finder_exec vorl1_fgdetect_con_finder_exec.cxx)
  TARGET_LINK_LIBRARIES(vorl1_fgdetect_con_finder_exec vorl1 dbdet_pro  dbbgm_pro)

ENDIF(BUILD_CONTRIB_vehicleseg)
