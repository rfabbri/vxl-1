// This is vox_mcs_eval_params.h
#ifndef vox_mcs_eval_params_h
#define vox_mcs_eval_params_h
//:
//\file
//\brief Parameter set for evaluating two-view multiview stereo
//\author Ricardo Fabbri (rfabbri), Brown University  (rfabbri@gmail.com)
//\date 10/15/2009 05:51:22 PM PDT
//


#include <dborl/algo/dborl_algo_params.h>

#include <string>
#include <vector>

//: put all the additional params into this class, and add them 
//  to the parameter list in the constructor so that 
//  all the files related to the parameter set of the algorithm are 
//  generated automatically via the methods of the base class
//  no need to overwrite any of the methods in the base class thanks 
//  to the parameter list
class vox_mcs_eval_params : public dborl_algo_params
{
public:

  //: Constructor
  vox_mcs_eval_params(std::string algo_name);

  // MEMBER VARIABLES
  
  //: Save result to the object folder?
  dborl_parameter<bool> save_to_object_folder_;  
  
  //: passes the folder of the input
  dborl_parameter<std::string> input_folder_;    

  //: Name of ground truth to compare against
  dborl_parameter<std::string> gt_object_name_;
  
  //: The folder of the ground truth
  dborl_parameter<std::string> gt_object_dir_;    

  //: extension of the output for evaluation of mcs
  dborl_parameter<std::string> output_mcs_eval_extension_;     

  // if written to this folder as opposed to object folder then the 
  // evaluation results gets associated to the input object.
  // if nothing is written here, nothing gets associated
  dborl_parameter<std::string> output_mcs_eval_folder_;  

  // A name identifying the dataset.
  dborl_parameter<std::string> dataset_name_;  

  //: Save all corresps? (to output folder)
  dborl_parameter<bool> dump_corresps_;

  // Stereo parameters
  // ----------------------------------------------------------------------

  // input camera type: intrinsic_extrinsic or projcamera
  dborl_parameter<std::string> cam_type_;

  // (in pixels) threshold for an edgel to be an inlier to the reprojected curve
  // in each view. Space-separated list of values.
  dborl_parameter<std::string> distance_threshold_list_;
  dborl_parameter<std::string> dtheta_threshold_list_;

  //: Interpret min_samples_per_curve_frag_list_ as lengths?
  dborl_parameter<bool> prune_by_length_;  

  // Used to prune the curves by enforcing a minimum number of samples.
  // Space-separated list of values.
  dborl_parameter<std::string> min_samples_per_curve_frag_list_;

  // If a view has less than this number of inliers, it will not vote at all
  // towards the curve.
  // Space-separated list of values.
  dborl_parameter<std::string> min_inliers_per_view_list_;

  dborl_parameter<std::string> min_total_inliers_list_;

  dborl_parameter<std::string> min_first_to_second_best_ratio_list_;
  dborl_parameter<std::string> lonely_threshold_list_;

  dborl_parameter<std::string> min_epipolar_overlap_list_;
  dborl_parameter<std::string> min_epiangle_list_;

  // Indicates the mcs_instance to run. This is an index into the
  // mcs_stereo_instances.txt file.
  dborl_parameter<unsigned> instance_id_;

  dborl_parameter<bool> use_curvelets_;
  dborl_parameter<std::string> min_num_inlier_edgels_per_curvelet_list_;
};


#endif // vox_mcs_eval_params_h
