//:
// \file
// \brief The parameter class for object detection via shock patches
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 11/14/07
//      
// \verbatim
//   Modifications
//  
// \endverbatim

//
//

#if !defined(_dbskr_detect_patch_params_h)
#define _dbskr_detect_patch_params_h

#include <bxml/bxml_document.h>
#include <dbskr/dbskr_sm_cor.h>
#include "dbskr_extract_patch_params.h"
#include <dborl/dborl_algo_params_base.h>

class dbskr_detect_instance_params
{
public:
  int top_N_;           // use top N matches of the image to each model patch
  int k_;      // put as a constraint that at least k model patches have top_N_ matches less than threshold

  vcl_string output_file_postfix_;
  
  bool parse_from_data(bxml_data_sptr root);
  static bxml_element *create_default_document_data();
  bxml_element *create_document_data();
};

class dbskr_detect_patch_params
{ public:

    float min_thres_;   
    float thres_inc_;   // 
    float max_thres_;   // 
    bool use_normalized_costs_;
    bool use_reconst_boundary_length_;        // use reconstructed scurve boundary's length computed by the dbskr_tree for normalization
    bool use_total_splice_cost_;              // use this value computed by the dbskr_tree for normalization
    float box_overlap_ratio_threshold_;       // e.g if 0.5 then ratio = (inters->area()/uni->area()) >= 0.5 --> TP 
                                              // intersection of ground truth box and the detection box is at least half of their union
    bool do_categorization_;          // if a box is detected assign the category, regardless of the overlap

    vcl_string index_file_;
    vcl_string model_dir_;  // directory of the model object
    vcl_string model_name_;  // name of the model object
    vcl_string matches_dir_;

    dbskr_detect_instance_params det_params_;

    dbskr_tree_edit_params shock_match_params_;  // --> will only use match_data part
    dbskr_extract_patch_params query_patch_params_;  // --> will only use patch_data part
    dbskr_extract_patch_params model_patch_params_;  // --> will only use patch_data part
    
    vcl_string output_file_postfix_;
    vcl_string out_file_;

    bool parse_from_data(bxml_data_sptr root);
    static bxml_element *create_default_document_data();
    bxml_element *create_document_data();
};

//: the following two classes should replace the two above eventually everywhere they are used
class dbskr_detect_instance_params2 : public dborl_algo_params_base
{
public:

  dborl_parameter<bool> use_only_patches_;   // if true detect instance using only the pairwise similarities of prototype matches and query matches
                                             // otherwise use the method that takes into account spatial layout of patch pairs to detect instances
  //: if use_only_patches
  dborl_parameter<int> top_N_;           // use top N matches of the image to each model patch
  dborl_parameter<int> k_;      // put as a constraint that at least k model patches have top_N_ matches less than threshold

  dborl_parameter<bool> detect_single_instance_;  // if true returns the best detection on the query if exists
                                                  // if false it is possible to return multiple detections

  dborl_parameter<float> sim_threshold_;  // sim threshold to declare a detection
  dborl_parameter<float> upper_sim_threshold_;  // detection algo using quads of patches requires an upper sim threshold
  dborl_parameter<float> alpha_;  // detection algo using quads of patches requires an alpha
  dborl_parameter<bool> use_approx_path_cost_;  // use approx path cost 
  dborl_parameter<bool> use_paths_;  // use paths directly as opposed to constructing another patch that connects the two
  dborl_parameter<bool> impose_geom_cons_;
  dborl_parameter<float> geom_threshold_;  // Lie dist threshold to eliminate proto transformations and decrease number of quads

  dborl_parameter<bool> use_normalized_costs_;
  dborl_parameter<bool> use_reconst_boundary_length_;        // use reconstructed scurve boundary's length computed by the dbskr_tree for normalization
  dborl_parameter<bool> use_total_splice_cost_;              // use this value computed by the dbskr_tree for normalization

  dbskr_detect_instance_params2() : dborl_algo_params_base("detect_ins") { 
    use_only_patches_.set_values(param_list_, "det", "use_only_patches", "use_only_patches?", true, true);
    top_N_.set_values(param_list_, "patch_det", "top_N", "use top N matches of the image to each model patch", 3, 3); 
    k_.set_values(param_list_, "patch_det", "k", "put as a constraint that at least k model patches have top_N_ matches less than threshold", 1, 1); 

    detect_single_instance_.set_values(param_list_, "det", "detect_single_instance", "detect_single_instance?", true, true);

    sim_threshold_.set_values(param_list_, "det", "sim_threshold", "sim_threshold", 0.5f, 0.5f);
    upper_sim_threshold_.set_values(param_list_, "quad_det", "upper_sim_threshold", "upper_sim_threshold", 1.5f, 1.5f);
    alpha_.set_values(param_list_, "quad_det", "alpha", "alpha", 0.3f, 0.3f);
    use_paths_.set_values(param_list_, "quad_det", "use_paths", "use_paths", true, true);
    use_approx_path_cost_.set_values(param_list_, "quad_det", "use_approx_path_cost", "use_approx_path_cost", true, true);
    impose_geom_cons_.set_values(param_list_, "quad_det", "impose_geom_cons", "impose_geom_cons", true, true);
    geom_threshold_.set_values(param_list_, "quad_det", "geom_threshold", "geom_threshold", 100.0f, 100.0f);

    use_normalized_costs_.set_values(param_list_, "det", "use_normalized_costs", "use_normalized_costs?", true, true);
    use_reconst_boundary_length_.set_values(param_list_, "det", "use_reconst_boundary_length", "use_reconst_boundary_length?", true, true);        // use reconstructed scurve boundary's length computed by the dbskr_tree for normalization
    use_total_splice_cost_.set_values(param_list_, "det", "use_total_splice_cost", "use_total_splice_cost?", false, false);                  // use this value computed by the dbskr_tree for normalization
  }

};

class dbskr_evaluate_detection_params : public dborl_algo_params_base
{ public:

    dborl_parameter<float> box_overlap_ratio_threshold_;       // e.g if 0.5 then ratio = (inters->area()/uni->area()) >= 0.5 --> TP 
                                              // intersection of ground truth box and the detection box is at least half of their union
    
    dborl_parameter<bool> do_categorization_;          // if a box is detected assume that the algo assigns the category to the query, do not check the box overlap to determine TP, FP etc.

    dbskr_evaluate_detection_params() : dborl_algo_params_base("eval_det") {       
      box_overlap_ratio_threshold_.set_values(param_list_, "data", "box_overlap_ratio_threshold", "box_overlap_ratio_threshold", 0.5f, 0.5f);
      do_categorization_.set_values(param_list_, "data", "do_categorization", "do_categorization? (i.e.dont check the overlap)", true, true);
    }


};


class dbskr_detect_patch_params2 : public dborl_algo_params_base
{ public:

    dborl_parameter<float> min_thres_;   
    dborl_parameter<float> thres_inc_;   // 
    dborl_parameter<float> max_thres_;   // 

    dborl_parameter<float> box_overlap_ratio_threshold_;       // e.g if 0.5 then ratio = (inters->area()/uni->area()) >= 0.5 --> TP 
                                              // intersection of ground truth box and the detection box is at least half of their union
    
    dborl_parameter<bool> do_categorization_;          // if a box is detected assume that the algo assigns the category to the query, do not check the box overlap to determine TP, FP etc.

    dbskr_detect_patch_params2() : dborl_algo_params_base("det_patch") { 
      min_thres_.set_values(param_list_, "data", "min_thres", "min threshold for detections", 0.1f, 0.1f);
      thres_inc_.set_values(param_list_, "data", "thres_inc", "thres increment for detections", 0.05f, 0.05f);
      max_thres_.set_values(param_list_, "data", "max_thres", "max threshold for detections", 2.0f, 2.0f);
      
      box_overlap_ratio_threshold_.set_values(param_list_, "data", "box_overlap_ratio_threshold", "e.g if 0.5 then ratio = (inters->area()/uni->area()) >= 0.5 --> TP ", 0.5f, 0.5f);
      do_categorization_.set_values(param_list_, "data", "do_categorization", "if a box is detected assign the category, regardless of the overlap", true, true);
    }


};

#endif  //_dbskr_detect_patch_params_h
