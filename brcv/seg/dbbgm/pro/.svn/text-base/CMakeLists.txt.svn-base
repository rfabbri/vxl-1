# This is brcv/seg/dbbgm/pro/CMakeLists.txt
# Brown Background Modeling Processing library


# include directories into here if needed
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1)
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )


SET(dbbgm_pro_sources
   dbbgm_process.cxx          dbbgm_process.h
   dbbgm_storage.cxx          dbbgm_storage.h   dbbgm_storage_sptr.h
   
   dbbgm_distribution_image_storage.cxx   dbbgm_distribution_image_storage.h   dbbgm_distribution_image_storage_sptr.h
   dbbgm_image_storage.cxx                dbbgm_image_storage.h   dbbgm_image_storage_sptr.h
   dbbgm_model_process.cxx                dbbgm_model_process.h
   dbbgm_init_model_process.cxx           dbbgm_init_model_process.h
   #dbbgm_joint_model_process.cxx          dbbgm_joint_model_process.h
   dbbgm_truth_model_process.cxx          dbbgm_truth_model_process.h
   dbbgm_bg_detect_process.cxx            dbbgm_bg_detect_process.h
   dbbgm_shadow_detect_process.cxx        dbbgm_shadow_detect_process.h
   #dbbgm_joint_detect_process.cxx         dbbgm_joint_detect_process.h
   dbbgm_hmm_train_process.cxx            dbbgm_hmm_train_process.h
   dbbgm_hmm_learn_trans_process.cxx      dbbgm_hmm_learn_trans_process.h
   dbbgm_hmm_detect_process.cxx           dbbgm_hmm_detect_process.h
   dbbgm_equalize_process.cxx             dbbgm_equalize_process.h
   dbbgm_aerial_bg_model_process.cxx      dbbgm_aerial_bg_model_process.h 
   dbbgm_aerial_bg_model_process1.cxx      dbbgm_aerial_bg_model_process1.h
   dbbgm_aerial_fg_uncertainity_detect_process.cxx
                                  dbbgm_aerial_fg_uncertainity_detect_process.h
   dbbgm_aerial_fg_uncertainity_detect_process1.cxx
                                  dbbgm_aerial_fg_uncertainity_detect_process1.h
   dbbgm_load_bg_model_process.cxx      dbbgm_load_bg_model_process.h
   dbbgm_load_bg_model_process1.cxx             dbbgm_load_bg_model_process1.h
   dbbgm_save_bg_model_process.cxx      dbbgm_save_bg_model_process.h
   dbbgm_save_bg_model_process1.cxx      dbbgm_save_bg_model_process1.h
   

)

AUX_SOURCE_DIRECTORY(Templates dbbgm_pro_sources)

ADD_LIBRARY(dbbgm_pro ${dbbgm_pro_sources})

TARGET_LINK_LIBRARIES(dbbgm_pro dbbgm_algo bbgm dbsta_algo bsta_algo)



