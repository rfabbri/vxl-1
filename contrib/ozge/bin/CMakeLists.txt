# This is contrib/ozge/bin/CMakeLists.txt
# Brown Shock Recognition Library Executables

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/trk )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/rec )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg )
INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR}/bbas )
#INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/Gregistration )
INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/db3p )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/db3p/expatpp/expat/lib )


SET( dbskr_visualize_patch_detections_sources
  dbskr_visualize_patch_detections.h          dbskr_visualize_patch_detections.cxx 
  dbskr_visualize_patch_detections_main.cxx
)

ADD_EXECUTABLE (dbskr_visualize_patch_detections ${dbskr_visualize_patch_detections_sources})
TARGET_LINK_LIBRARIES (dbskr_visualize_patch_detections dborl_algo dbskr_algoio dbskr_algo dbskr_pro dbsk2d_pro dbsk2d_algo dbdet_algo vil brip vul)


ADD_EXECUTABLE(dborl_cmd_line_utilities
dborl_cmd_line.cxx
dborl_cmd_line.h
dborl_cmd_line_main.cxx
)

TARGET_LINK_LIBRARIES( dborl_cmd_line_utilities dborl_algo dbskr_algoio vil  )


#ADD_EXECUTABLE( dbru_match dbru_match.cxx )
#TARGET_LINK_LIBRARIES( dbru_match dbskr dbsk2d_pro dbru dbru_algo vil btol brip vidpro1)

#ADD_EXECUTABLE( shock_match_99 match_shock_99db.cxx )
#TARGET_LINK_LIBRARIES( shock_match_99 dbskr dbsk2d_pro dbsk2d_algo dbru_algo vil btol brip)

#ADD_EXECUTABLE( dbru_match_shock dbru_match_shock.cxx )
#TARGET_LINK_LIBRARIES( dbru_match_shock dbskr dbsk2d_pro dbru dbru_algo vil btol brip vidpro1)

#ADD_EXECUTABLE( dbru_match_curve dbru_match_curve.cxx )
#TARGET_LINK_LIBRARIES( dbru_match_curve dbskr dbsk2d_pro dbru dbru_algo vil btol brip vidpro1)

#ADD_EXECUTABLE( dber_match_edgels dber_match_edgels.cxx )
#TARGET_LINK_LIBRARIES( dber_match_edgels dber dbru_algo vil btol brip)

#ADD_EXECUTABLE( dber_match_edgels_video dber_match_edgels_video.cxx )
#TARGET_LINK_LIBRARIES( dber_match_edgels_video dber georegister dbru_algo vsl vil_io btol brip)

#ADD_EXECUTABLE( dbru_match_ma_grimson dbru_match_ma_grimson.cxx )
#TARGET_LINK_LIBRARIES( dbru_match_ma_grimson dbskr dbsk2d_pro dbru dbru_algo vil btol brip vidpro1 dbdet)

# match sedan & taxi
#ADD_EXECUTABLE( dbru_match_ma_grimson_st dbru_match_ma_grimson_st.cxx )
#TARGET_LINK_LIBRARIES( dbru_match_ma_grimson_st dbskr dbsk2d_pro dbru dbru_algo vil btol brip vidpro1 dbdet)

# match sedan & taxi
#ADD_EXECUTABLE( dbru_match_ma_grimson_shock_st dbru_match_ma_grimson_shock_st.cxx )
#TARGET_LINK_LIBRARIES( dbru_match_ma_grimson_shock_st dbskr dbsk2d_pro dbru dbru_algo vil btol brip vidpro1 dbdet)

#ADD_EXECUTABLE( dbru_match_ma_grimson_shock dbru_match_ma_grimson_shock.cxx )
#TARGET_LINK_LIBRARIES( dbru_match_ma_grimson_shock dbskr dbsk2d_pro dbru dbru_algo vil btol brip vidpro1 dbdet)

#ADD_EXECUTABLE( dbskr_shock_pradeep match_test.cxx )
#TARGET_LINK_LIBRARIES( dbskr_shock_pradeep dbskr dbsk2d_pro dbru dbru_algo vil btol brip vidpro1)





#ADD_EXECUTABLE( dbskr_batch_run dbskr_batch_run.cxx )
#TARGET_LINK_LIBRARIES( dbskr_batch_run dbskr dbsk2d_pro dbru vil btol brip strk vidpro1)

#ADD_EXECUTABLE( dbskr_mutual_info_command dbskr_mutual_info_command.cxx )
#TARGET_LINK_LIBRARIES( dbskr_mutual_info_command dbskr dbru dbsk2d_pro vil btol brip strk vidpro1)

#ADD_EXECUTABLE( dbskr_batch_shgwrite dbskr_batch_shgwrite.cxx )
#TARGET_LINK_LIBRARIES( dbskr_batch_shgwrite dbskr dbsk2d_algo )

#ADD_EXECUTABLE( dbcvr_run dbcvr_run_clsd_cvmatch.cxx )
#TARGET_LINK_LIBRARIES( dbcvr_run dbcvr bsol)

#ADD_EXECUTABLE( dbcvr_run_even dbcvr_run_clsd_cvmatch_even.cxx )
#TARGET_LINK_LIBRARIES( dbcvr_run_even dbcvr bsol dbsol dbsol_algo)

#ADD_EXECUTABLE( dbcvr_run_info dbcvr_run_mutual_info.cxx )
#TARGET_LINK_LIBRARIES( dbcvr_run_info dbcvr dbru bsol dbsol dbsol_algo brip strk bsta vtol)

#ADD_EXECUTABLE( dbcvr_run_oblique dbcvr_run_clsd_cvmatch_oblique.cxx )
#TARGET_LINK_LIBRARIES( dbcvr_run_oblique dbcvr bsol dbsol dbsol_algo)

#ADD_EXECUTABLE( dbcvr_run_analyse dbcvr_analyse_results.cxx )
#TARGET_LINK_LIBRARIES( dbcvr_run_analyse dbcvr bsol)



