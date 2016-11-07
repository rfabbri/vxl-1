# This is contrib/ozge/bin/CMakeLists.txt
# Brown Shock Recognition Library Executables

include_directories( ${lems_SOURCE_DIR}/breye1 )
include_directories( ${lems_SOURCE_DIR}/brcv/trk )
include_directories( ${lems_SOURCE_DIR}/brcv/shp )
include_directories( ${lems_SOURCE_DIR}/brcv/rec )
include_directories( ${lems_SOURCE_DIR}/brcv/seg )
include_directories(${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories(${VXL_BRL_INCLUDE_DIR}/bbas )
#include_directories( ${lems_SOURCE_DIR}/contrib/Gregistration )
include_directories( ${VXL_MUL_INCLUDE_DIR} )
include_directories( ${lems_SOURCE_DIR}/db3p )
include_directories( ${lems_SOURCE_DIR}/db3p/expatpp/expat/lib )


set( dbskr_visualize_patch_detections_sources
  dbskr_visualize_patch_detections.h          dbskr_visualize_patch_detections.cxx 
  dbskr_visualize_patch_detections_main.cxx
)

add_executable (dbskr_visualize_patch_detections ${dbskr_visualize_patch_detections_sources})
target_link_libraries (dbskr_visualize_patch_detections dborl_algo dbskr_algoio dbskr_algo dbskr_pro dbsk2d_pro dbsk2d_algo dbdet_algo vil brip vul)


add_executable(dborl_cmd_line_utilities
dborl_cmd_line.cxx
dborl_cmd_line.h
dborl_cmd_line_main.cxx
)

target_link_libraries( dborl_cmd_line_utilities dborl_algo dbskr_algoio vil  )


#add_executable( dbru_match dbru_match.cxx )
#target_link_libraries( dbru_match dbskr dbsk2d_pro dbru dbru_algo vil btol brip vidpro1)

#add_executable( shock_match_99 match_shock_99db.cxx )
#target_link_libraries( shock_match_99 dbskr dbsk2d_pro dbsk2d_algo dbru_algo vil btol brip)

#add_executable( dbru_match_shock dbru_match_shock.cxx )
#target_link_libraries( dbru_match_shock dbskr dbsk2d_pro dbru dbru_algo vil btol brip vidpro1)

#add_executable( dbru_match_curve dbru_match_curve.cxx )
#target_link_libraries( dbru_match_curve dbskr dbsk2d_pro dbru dbru_algo vil btol brip vidpro1)

#add_executable( dber_match_edgels dber_match_edgels.cxx )
#target_link_libraries( dber_match_edgels dber dbru_algo vil btol brip)

#add_executable( dber_match_edgels_video dber_match_edgels_video.cxx )
#target_link_libraries( dber_match_edgels_video dber georegister dbru_algo vsl vil_io btol brip)

#add_executable( dbru_match_ma_grimson dbru_match_ma_grimson.cxx )
#target_link_libraries( dbru_match_ma_grimson dbskr dbsk2d_pro dbru dbru_algo vil btol brip vidpro1 dbdet)

# match sedan & taxi
#add_executable( dbru_match_ma_grimson_st dbru_match_ma_grimson_st.cxx )
#target_link_libraries( dbru_match_ma_grimson_st dbskr dbsk2d_pro dbru dbru_algo vil btol brip vidpro1 dbdet)

# match sedan & taxi
#add_executable( dbru_match_ma_grimson_shock_st dbru_match_ma_grimson_shock_st.cxx )
#target_link_libraries( dbru_match_ma_grimson_shock_st dbskr dbsk2d_pro dbru dbru_algo vil btol brip vidpro1 dbdet)

#add_executable( dbru_match_ma_grimson_shock dbru_match_ma_grimson_shock.cxx )
#target_link_libraries( dbru_match_ma_grimson_shock dbskr dbsk2d_pro dbru dbru_algo vil btol brip vidpro1 dbdet)

#add_executable( dbskr_shock_pradeep match_test.cxx )
#target_link_libraries( dbskr_shock_pradeep dbskr dbsk2d_pro dbru dbru_algo vil btol brip vidpro1)





#add_executable( dbskr_batch_run dbskr_batch_run.cxx )
#target_link_libraries( dbskr_batch_run dbskr dbsk2d_pro dbru vil btol brip strk vidpro1)

#add_executable( dbskr_mutual_info_command dbskr_mutual_info_command.cxx )
#target_link_libraries( dbskr_mutual_info_command dbskr dbru dbsk2d_pro vil btol brip strk vidpro1)

#add_executable( dbskr_batch_shgwrite dbskr_batch_shgwrite.cxx )
#target_link_libraries( dbskr_batch_shgwrite dbskr dbsk2d_algo )

#add_executable( dbcvr_run dbcvr_run_clsd_cvmatch.cxx )
#target_link_libraries( dbcvr_run dbcvr bsol)

#add_executable( dbcvr_run_even dbcvr_run_clsd_cvmatch_even.cxx )
#target_link_libraries( dbcvr_run_even dbcvr bsol dbsol dbsol_algo)

#add_executable( dbcvr_run_info dbcvr_run_mutual_info.cxx )
#target_link_libraries( dbcvr_run_info dbcvr dbru bsol dbsol dbsol_algo brip strk bsta vtol)

#add_executable( dbcvr_run_oblique dbcvr_run_clsd_cvmatch_oblique.cxx )
#target_link_libraries( dbcvr_run_oblique dbcvr bsol dbsol dbsol_algo)

#add_executable( dbcvr_run_analyse dbcvr_analyse_results.cxx )
#target_link_libraries( dbcvr_run_analyse dbcvr bsol)



