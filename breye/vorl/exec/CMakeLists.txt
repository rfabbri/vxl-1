# breye/vorl/exec/CMakeLists.txt

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/mvg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/trk )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/rec )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg )

INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES(${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${VXL_OXL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${VXL_MUL_INCLUDE_DIR} )

ADD_EXECUTABLE(vorl_dbctrk_exec vorl_dbctrk_exec.cxx)
TARGET_LINK_LIBRARIES(vorl_dbctrk_exec vorl dbctrk dbctrk_pro)

#ADD_EXECUTABLE(vorl_bmrf_exec vorl_bmrf_exec.cxx)
#TARGET_LINK_LIBRARIES(vorl_bmrf_exec vorl dbmrf_pro)

ADD_EXECUTABLE(vorl_compute_homography_exec vorl_compute_homography_exec.cxx)
TARGET_LINK_LIBRARIES(vorl_compute_homography_exec vorl dbvrl_pro bsta )

ADD_EXECUTABLE(vorl_mirec_exec vorl_mirec_exec.cxx)
TARGET_LINK_LIBRARIES(vorl_mirec_exec vorl dbsk2d_algo dbru_pro dbru_algo)

ADD_EXECUTABLE(vorl_fg_detect_exec vorl_fg_detect_exec.cxx)
TARGET_LINK_LIBRARIES(vorl_fg_detect_exec vorl dbbgm_pro dbdet_pro vidpro)

ADD_EXECUTABLE(vorl_bgbuilder_exec vorl_bgbuilder_exec.cxx)
TARGET_LINK_LIBRARIES(vorl_bgbuilder_exec vorl dbbgm_pro vidpro)



IF(BUILD_CONTRIB_vehicleseg) 
  INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib )
  INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/vehicleseg )
  ADD_EXECUTABLE(vorl_fgdetect_blob_finder_exec vorl_fgdetect_blob_finder_exec.cxx)
  TARGET_LINK_LIBRARIES(vorl_fgdetect_blob_finder_exec vorl dbdet_pro dbvrl_pro dbbgm_pro)

  ADD_EXECUTABLE(vorl_fgdetect_con_finder_exec vorl_fgdetect_con_finder_exec.cxx)
  TARGET_LINK_LIBRARIES(vorl_fgdetect_con_finder_exec vorl dbdet_pro  dbbgm_pro)

ENDIF(BUILD_CONTRIB_vehicleseg)
