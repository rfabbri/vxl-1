# breye/vorl/exec/CMakeLists.txt

include_directories( ${lems_SOURCE_DIR}/brcv/mvg )
include_directories( ${lems_SOURCE_DIR}/brcv/trk )
include_directories( ${lems_SOURCE_DIR}/brcv/rec )
include_directories( ${lems_SOURCE_DIR}/brcv/seg )

include_directories(${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories(${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories(${VXL_GEL_INCLUDE_DIR} )
include_directories(${VXL_OXL_INCLUDE_DIR} )
include_directories(${VXL_MUL_INCLUDE_DIR} )

add_executable(vorl_dbctrk_exec vorl_dbctrk_exec.cxx)
target_link_libraries(vorl_dbctrk_exec vorl dbctrk dbctrk_pro)

#add_executable(vorl_bmrf_exec vorl_bmrf_exec.cxx)
#target_link_libraries(vorl_bmrf_exec vorl dbmrf_pro)

add_executable(vorl_compute_homography_exec vorl_compute_homography_exec.cxx)
target_link_libraries(vorl_compute_homography_exec vorl dbvrl_pro bsta )

add_executable(vorl_mirec_exec vorl_mirec_exec.cxx)
target_link_libraries(vorl_mirec_exec vorl dbsk2d_algo dbru_pro dbru_algo)

add_executable(vorl_fg_detect_exec vorl_fg_detect_exec.cxx)
target_link_libraries(vorl_fg_detect_exec vorl dbbgm_pro dbdet_pro vidpro)

add_executable(vorl_bgbuilder_exec vorl_bgbuilder_exec.cxx)
target_link_libraries(vorl_bgbuilder_exec vorl dbbgm_pro vidpro)



if(BUILD_CONTRIB_vehicleseg) 
  include_directories( ${lems_SOURCE_DIR}/contrib )
  include_directories( ${lems_SOURCE_DIR}/contrib/vehicleseg )
  add_executable(vorl_fgdetect_blob_finder_exec vorl_fgdetect_blob_finder_exec.cxx)
  target_link_libraries(vorl_fgdetect_blob_finder_exec vorl dbdet_pro dbvrl_pro dbbgm_pro)

  add_executable(vorl_fgdetect_con_finder_exec vorl_fgdetect_con_finder_exec.cxx)
  target_link_libraries(vorl_fgdetect_con_finder_exec vorl dbdet_pro  dbbgm_pro)

endif(BUILD_CONTRIB_vehicleseg)
