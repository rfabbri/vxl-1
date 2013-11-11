#include "bof_register.h"

#include <bprb/bprb_macros.h>
#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_func_process.h>

#include "bof_processes.h"

#include <bof/bof_util.h>
#include <bof/bof_codebook.h>
#include <bof/bof_labels_keypoint_joint.h>
#include <bof/bof_scene_categories.h>


void bof_register::register_datatype()
{
  REGISTER_DATATYPE(bof_feature_vector_sptr);
  REGISTER_DATATYPE(bof_codebook_sptr);
  REGISTER_DATATYPE(bof_p_xc_sptr);
  REGISTER_DATATYPE(bof_scene_categories_sptr);
}

void bof_register::register_process()
{
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bof_sample_from_train_process, "bofSampleFromTrainProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bof_init_info_file_process, "bofInitInfoFileProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bof_rnd_means_process, "bofRndMeansProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bof_k_means_subsample_process, "bofKMeansSubsampleProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bof_combine_means_process, "bofCombineMeansProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bof_k_means_on_vector_process, "bofKMeansOnVectorProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bof_choose_min_distortion_clustering_process, "bofChooseMinDistortionClusteringProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bof_k_means_train_process, "bofKMeansTrainProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bof_init_category_info_process, "bofInitCategoryInfoProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bof_init_codebook_process, "bofInitCodebookProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bof_learn_categories_process, "bofLearnCategoriesProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bof_combine_quantization_process, "bofCombineQuantizationProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bof_save_category_scene_raw_process, "bofSaveCategorySceneRawProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bof_load_p_xc_process, "bofLoadPXCProcess"); 
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bof_classify_process, "bofClassifyProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bof_examine_ground_truth_process, "bofExamineGroundTruthProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bof_scale_ply_process, "bofScalePLYProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bof_make_cluster_id_scene_process, "bofMakeClusterIdSceneProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bof_save_cluster_id_scene_raw_process, "bofSaveClusterIdSceneRawProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bof_compute_typical_bbox_process, "bofComputeTypicalBboxProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bof_learn_category_codebook_process, "bofLearnCategoryCodebookProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bof_load_category_info_process, "bofLoadCategoryInfoProces");
  REG_PROCESS_FUNC_CONS2(bof_class_probability_process);
} 

