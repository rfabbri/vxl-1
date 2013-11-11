// This is vox_mcs_eval.h
#ifndef vox_mcs_eval_h
#define vox_mcs_eval_h
//:
//\file
//\brief class for evaluation of mcs_stereo.
//\author Ricardo Fabbri (rfabbri), Brown University  (rfabbri@lems.brown.edu)
//\date 10/16/2009 04:51:49 PM PDT
//

#include <mw/cmd/vox_mcs_eval_params_sptr.h>
#include <mw/cmd/vox_mcs_eval_params.h>
#include <mw/mw_util.h>
#include <mw/mw_discrete_corresp.h>
#include <mw/dbmcs_view_set.h>
#include <mw/algo/mw_data.h>

class vox_mcs_eval {
public:
  vox_mcs_eval_params_sptr params_;
  mw_util::camera_file_type cam_type_;
  mw_discrete_corresp gt_;
  mw_curve_stereo_data_path dpath_;
  dbmcs_stereo_instance_views all_instances_;
  dbmcs_stereo_instance_views one_instance_;
  
  vcl_vector<double> distance_threshold_list_;
  vcl_vector<double> dtheta_threshold_list_;
  vcl_vector<double> min_samples_per_curve_frag_list_;
  bool prune_by_length_;
  vcl_vector<unsigned> min_inliers_per_view_list_;
  vcl_vector<unsigned> min_total_inliers_list_;
  bool use_curvelets_;
  vcl_vector<unsigned> min_num_inlier_edgels_per_curvelet_list_;
  vcl_vector<double> min_first_to_second_best_ratio_list_;
  vcl_vector<double> lonely_threshold_list_;
  vcl_vector<double> min_epiangle_list_;
  vcl_vector<unsigned> min_epipolar_overlap_list_;

  void read_cams();
  void read_frame_data();
  void read_gt();
  void read_one_mcs_instance();
  void read_param_lists();
};

#endif // vox_mcs_eval_h

