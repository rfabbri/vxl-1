#include "vox_mcs_eval_params.h"
#include <dborl/algo/dborl_algo_params.h>

//: Constructor
vox_mcs_eval_params::
vox_mcs_eval_params(vcl_string algo_name) : 
    dborl_algo_params(algo_name)
{ 
  // Save result to the object folder?
  this->save_to_object_folder_.set_values(this->param_list_, "io", 
    "save_to_object_folder", "-io: save result to object folder ?", 
                                          true, true);

  //: passes the folder of the input object
  this->input_folder_.set_values(
      this->param_list_, 
      "io", "input_folder",
      "main input folder containing the data for all views and other inputs", "", 
      "",
      0, // for 0th input object
      dborl_parameter_system_info::
      INPUT_OBJECT_DIR);

  this->gt_object_name_.
      set_values(this->param_list_, 
                 "io", "gt_object_name",
                 "ground truth correspondence nae", "dummy", "",
                 0, // for 1th input object
                 dborl_parameter_system_info::INPUT_OBJECT_STRING_ID);

  this->gt_object_dir_.
      set_values(this->param_list_, 
                 "io", "gt_object_dir",
                 "ground truth object folder", "dummy", 
                 "",
                 0, // for 1th input object
                 dborl_parameter_system_info::INPUT_OBJECT_DIR);

  this->output_mcs_eval_extension_.set_values
      (this->param_list_, 
       "io", 
       "output_mcs_eval_extension_", 
       "-io: output extension of correspondence evaluation results on dataset ", 
       "_eval_results.xml", "_eval_results.xml");

  // Output shock folder for evaluation of detect results
  this->output_mcs_eval_folder_.
      set_values(this->param_list_, "io", 
                 "output_mcs_eval_folder", 
                 "output folder to write evaluation results off experiment", "",
                 "",
                 0, // associated to 1th input object
                 dborl_parameter_system_info::OUTPUT_FILE,
                 "mcs_eval",
                 dborl_parameter_type_info::FILEASSOC);


  this->dataset_name_.set_values(
      param_list_,
      "io",
      "dataset_name",
      "String identifying the dataset",
      "");

  this->cam_type_.set_values(
      param_list_,
      "io",
      "cam_type",
      "Input camera type (intrinsic_extrinsic or projcamera)",
      "intrinsic_extrinsic"
      );

  this->dump_corresps_.set_values(this->param_list_, "io", 
    "dump_corresps", "save all correspondences (into output dir)?", false, false);

  this->distance_threshold_list_.set_values(
      param_list_,
      "mcs_reprojection",
      "distance_threshold_list",
      "space-separated list of thresholds (in pixels) for an edgel to be an inlier to the reprojected curve in each view",
      "10.0"
      );

  this->dtheta_threshold_list_.set_values(
      param_list_,
      "mcs_reprojection",
      "dtheta_threshold_list",
      "space-separated list of thresholds (in degrees) in orientation difference for an edgel to be an inlier to reprojected curve in each view",
      "10.0"
      );

  this->prune_by_length_.set_values(this->param_list_, "mcs_basics", 
    "prune_by_length", "prune by length?", true, true);

  this->min_samples_per_curve_frag_list_.set_values(
      param_list_,
      "mcs_basics",
      "min_samples_per_curve_frag_list",
      "space-separated list of values used to prune the curves by enforcing a minimum number of samples. If prune_by_length is set, this list is intepreted as lengths (doubles)",
      "40"
      );

  this->min_epipolar_overlap_list_.set_values(
      param_list_,
      "mcs_basics",
      "min_epipolar_overlap_list",
      "space-separated list of values; minimum number of intersections a curve has to have with the epiband to be a candidate",
      "5"
      );

  this->min_epiangle_list_.set_values(
      param_list_,
      "mcs_basics",
      "min_epiangle_list",
      "space-separated list of values; (in degrees) minimum angle between an epipolar line and curve samples to consider",
      "30.0"
      );

  this->min_inliers_per_view_list_.set_values(
      param_list_,
      "mcs_reprojection",
      "min_inliers_per_view_list",
      "space-separated list of values; if a view has less than this number of inliers, it will not vote at all towards the curve.",
      "10"
      );

  this->min_num_inlier_edgels_per_curvelet_list_.set_values(
      param_list_,
      "mcs_reprojection",
      "min_num_inlier_edgels_per_curvelet_list",
      "space-separated list of values; if a curvelet has less than this number of inlier edgels, it will not vote at all towards the curve.",
      "3"
      );

  this->use_curvelets_.set_values(this->param_list_, "mcs_reprojection", 
    "use_curvelets", "use curvelets?", false, false);

  this->min_total_inliers_list_.set_values(
      param_list_,
      "mcs_reprojection",
      "min_total_inliers_list",
      "space-separated list of values; if a match has less than this number of inliers, it will be ruled out.",
      "5"
      );

  this->min_first_to_second_best_ratio_list_.set_values(
      param_list_,
      "mcs_reprojection",
      "min_first_to_second_best_ratio_list",
      "space-separated list of values; minimum ratio of first to second best to consider as reliable unambiguous match. Set to zero if you always want to keep the best match.",
      "1.5"
      );

  this->lonely_threshold_list_.set_values(
      param_list_,
      "mcs_reprojection",
      "lonely_threshold_list",
      "space-separated list of values; minimum #votes of a lonely correspondence = -lonely*-first_to_second_ratio will be considered as reliable match (defaults to mininliers_total)",
      "0.0"
      );


}
