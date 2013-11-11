//:
// \file
// \brief The extract patch parameter class 
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 11/08/07
//      
// \verbatim
//   Modifications
//  
// \endverbatim

//
//

#if !defined(_dbskr_extract_shock_params_h)
#define _dbskr_extract_shock_params_h

#include <bxml/bxml_document.h>
#include <dborl/dborl_algo_params_base.h>

class dbskr_extract_shock_params
{ public:

    float prune_threshold_;
    float sampling_ds_;   // usually 1.0f;

    bool perform_gap_transforms_;
    double curve_length_gamma_;   // usually 2.0f
    float cont_thres_;
    float app_thres_;
    float alpha_cont_;
    float alpha_app_;  // alpha_cont_ + alpha_app_ should equal 1
    bool add_noise_in_a_loop_;
    float noise_radius_;

    vcl_string output_file_postfix_;

    bool parse_from_data(bxml_data_sptr root);
    static bxml_element *create_default_document_data();
    bxml_element *create_document_data();
};

class dbskr_extract_shock_params2 : public dborl_algo_params_base
{ public:

    dborl_parameter<bool> fit_lines_;
    dborl_parameter<float> fit_lines_rms_;

    dborl_parameter<bool> add_bounding_box_;
    dborl_parameter<int> bounding_box_offset_;  // enlarge pixel by 2*offset in width and in height
                                                // only valid for extracting from cem files

    dborl_parameter<float> prune_threshold_;
    dborl_parameter<float> sampling_ds_;   // usually 1.0f;
    dborl_parameter<bool> add_noise_in_a_loop_;
    dborl_parameter<float> noise_radius_;

    dborl_parameter<bool> perform_gap_transforms_;
    dborl_parameter<double> curve_length_gamma_;   // usually 2.0f
    dborl_parameter<float> cont_thres_;
    dborl_parameter<float> app_thres_;
    dborl_parameter<float> alpha_cont_;
    dborl_parameter<float> alpha_app_;  // alpha_cont_ + alpha_app_ should equal 1

    dbskr_extract_shock_params2() : dborl_algo_params_base("ext_sh") { 

      //: pre-processing
      fit_lines_.set_values(param_list_, "pre", "fit_lines", "fit lines?", true, true);
      fit_lines_rms_.set_values(param_list_, "pre", "fit_lines_rms", "fit lines rms", 0.005f, 0.005f);
      
      add_noise_in_a_loop_.set_values(param_list_, "pre", "add_noise_in_a_loop", "add_noise_in_a_loop", false, false);
      noise_radius_.set_values(param_list_, "pre", "noise_radius", "noise_radius", 0.002f, 0.002f);
      
      add_bounding_box_.set_values(param_list_, "pre", "add_bounding_box", "add bounding box?", true, true);
      bounding_box_offset_.set_values(param_list_, "pre", "bounding_box_offset", "bounding box offset", 3, 3);
      
      //: main parameters of shock extraction
      prune_threshold_.set_values(param_list_, "main", "prune_threshold", "prune_threshold", 1.0f, 1.0f);
      sampling_ds_.set_values(param_list_, "main", "sampling_ds", "sampling_ds", 1.0f, 1.0f);
      
      //: parameters required when extracting from a general boundary set
      perform_gap_transforms_.set_values(param_list_, "gap_t", "perform_gap_transforms", "perform_gap_transforms", false, false);
      noise_radius_.set_values(param_list_, "gap_t", "noise_radius", "noise_radius", 0.002f, 0.002f);
      curve_length_gamma_.set_values(param_list_, "gap_t", "curve_length_gamma", "curve_length_gamma", 2.0, 2.0);
      cont_thres_.set_values(param_list_, "gap_t", "cont_thres", "cont_thres", 0.5f, 0.5f);
      app_thres_.set_values(param_list_, "gap_t", "app_thres", "app_thres", 0.5f, 0.5f);
      alpha_cont_.set_values(param_list_, "gap_t", "alpha_cont", "alpha_cont", 0.5f, 0.5f);
      alpha_app_.set_values(param_list_, "gap_t", "alpha_app", "alpha_cont_ + alpha_app_ should equal 1", 0.5f, 0.5f);
      
    }

    
};


#endif  //_dbskr_extract_shock_params_h
