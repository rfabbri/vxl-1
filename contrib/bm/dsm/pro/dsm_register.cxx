//this is /contrib/bm/dsm/pro/dsm_register.cxx
#include<bprb/bprb_macros.h>
#include<bprb/bprb_batch_process_manager.h>
#include<bprb/bprb_func_process.h>

#include"dsm_register.h"
#include"dsm_processes.h"

#include"dsm/dsm_features_sptr.h"
#include"dsm/dsm_feature_sptr.h"
#include"dsm/dsm_time_series_sptr.h"
#include"dsm/dsm_ncn_sptr.h"
#include"dsm/dsm_manager_base_sptr.h"
#include"dsm/dsm_state_machine_base_sptr.h"
#include"dsm/dsm_pixel_time_series_map_sptr.h"
#include"dsm/dsm_target_neighborhood_map_sptr.h"
#include"dsm/dsm_ground_truth_sptr.h"


void dsm_register::register_datatype()
{
    REGISTER_DATATYPE( dsm_ncn_sptr );
    REGISTER_DATATYPE( dsm_features_sptr );
    REGISTER_DATATYPE( dsm_state_machine_base_sptr );
    REGISTER_DATATYPE( dsm_manager_base_sptr );
    REGISTER_DATATYPE( dsm_feature_sptr );
    REGISTER_DATATYPE( dsm_time_series_sptr );
    REGISTER_DATATYPE( dsm_pixel_time_series_map_sptr );
    REGISTER_DATATYPE( dsm_target_neighborhood_map_sptr );
    REGISTER_DATATYPE( dsm_ground_truth_sptr );
}//end dsm_register::register_datatype

void dsm_register::register_process()
{
    REG_PROCESS_FUNC_CONS(bprb_func_process,
                              bprb_batch_process_manager,
                              dsm_create_ncn_sptr_process,
                              "dsmCreateNcnSptrProcess");
    REG_PROCESS_FUNC_CONS(bprb_func_process,
                              bprb_batch_process_manager,
                              dsm_ncn_sptr_process,
                              "dsmNcnSptrProcess");
    REG_PROCESS_FUNC_CONS(bprb_func_process,
                              bprb_batch_process_manager,
                              dsm_ncn_add_target_process,
                              "dsmNcnAddTargetProcess");
    REG_PROCESS_FUNC_CONS(bprb_func_process,
                              bprb_batch_process_manager,
                              dsm_ncn_set_num_pivot_pixels_process,
                              "dsmNcnSetNumPivotPixelsProcess");
    REG_PROCESS_FUNC_CONS(bprb_func_process,
                              bprb_batch_process_manager,
                              dsm_ncn_set_num_neighbors_process,
                              "dsmNcnSetNumNeighborsProcess");
    REG_PROCESS_FUNC_CONS(bprb_func_process,
                              bprb_batch_process_manager,
                              dsm_ncn_set_num_particles_process,
                              "dsmNcnSetNumParticlesProcess");
    REG_PROCESS_FUNC_CONS(bprb_func_process,
                              bprb_batch_process_manager,
                              dsm_ncn_write_neighborhood_dat_process,
                              "dsmNcnWriteNeighborhoodDatProcess");
    REG_PROCESS_FUNC_CONS(bprb_func_process,
                              bprb_batch_process_manager,
                              dsm_ncn_set_video_glob_process,
                              "dsmNcnSetVideoGlobProcess");
    REG_PROCESS_FUNC_CONS(bprb_func_process,
                          bprb_batch_process_manager,
                          dsm_ncn_calculate_temporal_entropy_process,
                          "dsmNcnCalculateTemporalEntropyProcess");
    REG_PROCESS_FUNC_CONS(bprb_func_process,
                          bprb_batch_process_manager,
                          dsm_ncn_write_pivot_pixel_candidates_dat_process,
                          "dsmNcnWritePivotPixelCandidatesDatProcess");

    REG_PROCESS_FUNC_CONS(bprb_func_process,
                          bprb_batch_process_manager,
                          dsm_write_ncn_sptr_bin_process,
                          "dsmWriteNcnSptrBinProcess");
    REG_PROCESS_FUNC_CONS(bprb_func_process,
                          bprb_batch_process_manager,
                          dsm_read_ncn_sptr_bin_process,
                          "dsmReadNcnSptrBinProcess");
    REG_PROCESS_FUNC_CONS(bprb_func_process,
                          bprb_batch_process_manager,
                          dsm_build_ncn_process,
                          "dsmBuildNcnProcess");
    REG_PROCESS_FUNC_CONS(bprb_func_process,
                          bprb_batch_process_manager,
                          dsm_write_neighborhood_mfile_process,
                          "dsmWriteNeighborhoodMfileProcess");
    REG_PROCESS_FUNC_CONS(bprb_func_process,
                          bprb_batch_process_manager,
                          dsm_write_neighborhood_xml_process,
                          "dsmWriteNeighborhoodXmlProcess");

    REG_PROCESS_FUNC_CONS(bprb_func_process,
                          bprb_batch_process_manager,
                          dsm_calculate_temporal_entropy_process,
                          "dsmCalculateTemporalEntropyProcess");
    REG_PROCESS_FUNC_CONS(bprb_func_process,
                          bprb_batch_process_manager,
                          dsm_write_temporal_entropy_dat_process,
                          "dsmWriteTemporalEntropyDatProcess");
    REG_PROCESS_FUNC_CONS(bprb_func_process,
                          bprb_batch_process_manager,
                          dsm_write_temporal_entropy_bin_process,
                          "dsmWriteTemporalEntropyBinProcess");
    REG_PROCESS_FUNC_CONS(bprb_func_process,
                          bprb_batch_process_manager,
                          dsm_read_temporal_entropy_bin_process,
                          "dsmReadTemporalEntropyBinProcess");

    REG_PROCESS_FUNC_CONS(bprb_func_process,
                          bprb_batch_process_manager,
                          dsm_extract_sift_features_process,
                          "dsmExtractSiftFeaturesProcess");
    REG_PROCESS_FUNC_CONS(bprb_func_process,
                          bprb_batch_process_manager,
                          dsm_extract_dsift_features_process,
                          "dsmExtractDsiftFeaturesProcess");
    REG_PROCESS_FUNC_CONS(bprb_func_process,
                          bprb_batch_process_manager,
                          dsm_extract_bapl_dsift_time_series_box_process,
                          "dsmExtractBaplDsiftTimeSeriesBoxProcess");
    REG_PROCESS_FUNC_CONS(bprb_func_process,
                          bprb_batch_process_manager,
                          dsm_extract_intensity_features_process,
                          "dsmExtractIntensityFeaturesProcess");

    REG_PROCESS_FUNC_CONS(bprb_func_process,
                          bprb_batch_process_manager,
                          dsm_extract_intensity_ratio_features_process,
                          "dsmExtractIntensityRatioFeaturesProcess");
    REG_PROCESS_FUNC_CONS(bprb_func_process,
                          bprb_batch_process_manager,
                          dsm_extract_bapl_dsift_feature_process,
                          "dsmExtractBaplDsiftFeatureProcess");
    REG_PROCESS_FUNC_CONS(bprb_func_process,
                          bprb_batch_process_manager,
                          dsm_write_features_sptr_bin_process,
                          "dsmWriteFeaturesSptrBinProcess");
    REG_PROCESS_FUNC_CONS(bprb_func_process,
                          bprb_batch_process_manager,
                          dsm_read_features_sptr_bin_process,
                          "dsmReadFeaturesSptrBinProcess");
    REG_PROCESS_FUNC_CONS(bprb_func_process,
                          bprb_batch_process_manager,
                          dsm_write_reduced_features_mfile_process,
                          "dsmWriteReducedFeaturesMfileProcess");
    REG_PROCESS_FUNC_CONS(bprb_func_process,
                          bprb_batch_process_manager,
                          dsm_reduce_features_dimensionality_process,
                          "dsmReduceFeaturesDimensionalityProcess");

    REG_PROCESS_FUNC_CONS(bprb_func_process,
                          bprb_batch_process_manager,
                          dsm_create_feature_sptr_process,
                          "dsmCreateFeatureSptrProcess");
    //REG_PROCESS_FUNC_CONS(bprb_func_process,
    //                      bprb_batch_process_manager,
    //                      dsm_extract_intensity_feature_process,
    //                      "dsmExtractIntensityFeatureProcess");
    REG_PROCESS_FUNC_CONS(bprb_func_process,
                          bprb_batch_process_manager,
                          dsm_extract_intensity_ratio_feature_process,
                          "dsmExtractIntensityRatioFeatureProcess");
    REG_PROCESS_FUNC_CONS(bprb_func_process,
                          bprb_batch_process_manager,
                     dsm_extract_intensity_ratio_pixel_time_series_map_process,
                     "dsmExtractIntensityRatioPixelTimeSeriesMapProcess");

    REG_PROCESS_FUNC_CONS(bprb_func_process,
                          bprb_batch_process_manager,
                          dsm_create_time_series_process,
                          "dsmCreateTimeSeriesProcess");
    REG_PROCESS_FUNC_CONS(bprb_func_process,
                          bprb_batch_process_manager,
                          dsm_time_series_insert_feature_process,
                          "dsmTimeSeriesInsertFeatureProcess");
    REG_PROCESS_FUNC_CONS(bprb_func_process,
                          bprb_batch_process_manager,
                          dsm_write_time_series_dat_process,
                          "dsmWriteTimeSeriesDatProcess");

    REG_PROCESS_FUNC_CONS(bprb_func_process,
                          bprb_batch_process_manager,
                          dsm_create_pixel_time_series_map_sptr_process,
                          "dsmCreatePixelTimeSeriesMapSptrProcess");
    REG_PROCESS_FUNC_CONS(bprb_func_process,
                          bprb_batch_process_manager,
                          dsm_write_pixel_time_series_map_bin_process,
                          "dsmWritePixelTimeSeriesMapBinProcess");
    REG_PROCESS_FUNC_CONS(bprb_func_process,
                          bprb_batch_process_manager,
                          dsm_write_pixel_time_series_map_dat_process,
                          "dsmWritePixelTimeSeriesMapDatProcess");
    REG_PROCESS_FUNC_CONS(bprb_func_process,
                          bprb_batch_process_manager,
                          dsm_read_pixel_time_series_map_bin_process,
                          "dsmReadPixelTimeSeriesMapBinProcess");
    REG_PROCESS_FUNC_CONS(bprb_func_process,
                          bprb_batch_process_manager,
                          dsm_pixel_time_series_map_insert_process,
                          "dsmPixelTimeSeriesMapInsertProcess");
    REG_PROCESS_FUNC_CONS(bprb_func_process,
                          bprb_batch_process_manager,
                          dsm_pixel_time_series_map_insert_feature_process,
                          "dsmPixelTimeSeriesMapInsertFeatureProcess");
    REG_PROCESS_FUNC_CONS(bprb_func_process,
                          bprb_batch_process_manager,
                          dsm_time_series_sptr_from_map_process,
                          "dsmTimeSeriessptrFromMapProcess");
    REG_PROCESS_FUNC_CONS(bprb_func_process,
                          bprb_batch_process_manager,
                          dsm_reduce_pixel_time_series_map_dimension_process,
                          "dsmReducePixelTimeSeriesMapDimensionProcess");

    REG_PROCESS_FUNC_CONS(bprb_func_process,
                          bprb_batch_process_manager,
                          dsm_state_machine_process,
                          "dsmStateMachineProcess");
    REG_PROCESS_FUNC_CONS(bprb_func_process,
                          bprb_batch_process_manager,
                          dsm_write_state_machine_bin_process,
                          "dsmWriteStateMachineBinProcess");
    REG_PROCESS_FUNC_CONS(bprb_func_process,
                          bprb_batch_process_manager,
                          dsm_read_state_machine_bin_process,
                          "dsmReadStateMachineBinProcess");

    REG_PROCESS_FUNC_CONS(bprb_func_process,
                          bprb_batch_process_manager,
                          dsm_target_neighborhood_map_from_ncn_sptr_process,
                          "dsmTargetNeighborhoodMapFromNcnSptrProcess");
    REG_PROCESS_FUNC_CONS(bprb_func_process,
                          bprb_batch_process_manager,
                          dsm_target_neighborhood_map_write_dat_process,
                          "dsmTargetNeighborhoodMapWriteDatProcess");
    REG_PROCESS_FUNC_CONS(bprb_func_process,
                          bprb_batch_process_manager,
                          dsm_target_neighborhood_map_write_bin_process,
                          "dsmTargetNeighborhoodMapWriteBinProcess");
    REG_PROCESS_FUNC_CONS(bprb_func_process,
                          bprb_batch_process_manager,
                          dsm_target_neighborhood_map_read_bin_process,
                          "dsmTargetNeighborhoodMapReadBinProcess");

    REG_PROCESS_FUNC_CONS(bprb_func_process,
                          bprb_batch_process_manager,
                          dsm_time_series_extract_intensity_box_process,
                          "dsmTimeSeriesExtractIntensityBoxProcess");

    REG_PROCESS_FUNC_CONS(bprb_func_process,
                          bprb_batch_process_manager,
                     dsm_state_machine_classify_pixel_time_series_map_process,
                     "dsmStateMachineClassifyPixelTimeSeriesMapProcess");
    REG_PROCESS_FUNC_CONS(bprb_func_process,
                          bprb_batch_process_manager,
            dsm_state_machine_classify_pixel_time_series_map_no_output_process,
            "dsmStateMachineClassifyPixelTimeSeriesMapNoOutputProcess");

    REG_PROCESS_FUNC_CONS(bprb_func_process,
                          bprb_batch_process_manager,
                          dsm_manager_write_bin_process,
                          "dsmManagerWriteBinProcess");
    REG_PROCESS_FUNC_CONS(bprb_func_process,
                          bprb_batch_process_manager,
                          dsm_manager_read_bin_process,
                          "dsmManagerReadBinProcess");
    REG_PROCESS_FUNC_CONS(bprb_func_process,
                          bprb_batch_process_manager,
                          dsm_manager_write_change_maps_process,
                          "dsmManagerWriteChangeMapsProcess");
    REG_PROCESS_FUNC_CONS(bprb_func_process,
                          bprb_batch_process_manager,
                          dsm_manager_write_output_process,
                          "dsmManagerWriteOutputProcess");
    REG_PROCESS_FUNC_CONS(bprb_func_process,
                          bprb_batch_process_manager,
                          dsm_manager_write_prediction_map_process,
                          "dsmManagerWritePredictionMapProcess");
    REG_PROCESS_FUNC_CONS(bprb_func_process,
                          bprb_batch_process_manager,
                        dsm_manager_write_average_num_states_per_frame_process,
                        "dsmManagerWriteAverageNumStatesPerFrameProcess");

    REG_PROCESS_FUNC_CONS(bprb_func_process,
                          bprb_batch_process_manager,
               dsm_pixel_time_series_map_sptr_extract_sift_and_reduce_process,
               "dsmPixelTimeSeriesMapSptrExtractSiftAndReduceProcess");

    REG_PROCESS_FUNC_CONS(bprb_func_process,
                          bprb_batch_process_manager,
                          dsm_write_pixel_dat_process,
                          "dsmWritePixelDatProcess");

    REG_PROCESS_FUNC_CONS(bprb_func_process,
                          bprb_batch_process_manager,
                          dsm_ground_truth_read_bwm_bin_process,
                          "dsmGroundTruthReadBwmBinProcess");
    REG_PROCESS_FUNC_CONS(bprb_func_process,
                          bprb_batch_process_manager,
                          dsm_ground_truth_build_change_maps_process,
                          "dsmGroundTruthBuildChangeMapsProcess");
    REG_PROCESS_FUNC_CONS(bprb_func_process,
                          bprb_batch_process_manager,
                          dsm_ground_truth_write_change_maps_process,
                          "dsmGroundTruthWriteChangeMapsProcess");
    REG_PROCESS_FUNC_CONS(bprb_func_process,
                          bprb_batch_process_manager,
                          dsm_ground_truth_write_change_maps_classical_process,
                          "dsmGroundTruthWriteChangeMapsClassicalProcess");

    REG_PROCESS_FUNC_CONS(bprb_func_process,
                          bprb_batch_process_manager,
                  dsm_utility_compute_confusion_matrix_from_change_maps_process,
                  "dsmUtilityComputeConfusionMatrixFromChangeMapsProcess");
    REG_PROCESS_FUNC_CONS(bprb_func_process,
                          bprb_batch_process_manager,
                          dsm_utility_bvxm_binary_to_byte_process,
                          "dsmUtilityBvxmBinaryToByteProcess");
}//end dsm_register::register_process
