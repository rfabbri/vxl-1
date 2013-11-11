//:
// \file
// \brief The shock match parameters
//
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 11/09/07
//      
// \verbatim
//   Modifications
//  
// \endverbatim

//
//

#if !defined(_dbskr_match_shock_params_h)
#define _dbskr_match_shock_params_h

#include <bxml/bxml_document.h>
#include <dbskr/dbskr_sm_cor.h>

class dbskr_match_shock_params
{ public:

  dbskr_tree_edit_params edit_params_;

  bool save_shgms_;
  vcl_string shgm_folder_;

  bool orl_format_folders_;

  bool db_train_same_;  // CAUTION: if the user passes the training set and the database (test set) to be the same set (same size, from same directory, with the same list)
                        //          and sets the option of db_train_same_ on
                        //          then the algorithm works in a special mode, and creates a single matrix which is made into a symmetric matrix in the finalize method
                        //          if db_train_same_ is set to off then it is assumed that the train and test sets are different then two sets of computations are performed to make sure
                        //          that both edit(t1, t2) and edit(t2, t1) are performed and the min cost is reported in the final matrix


  bool generate_output_;
  vcl_string output_file_;
  //vcl_string output_file_postfix_;

  vcl_string train_dir_, train_list_, db_dir_, db_list_;

  bool parse_from_data(bxml_data_sptr root);
  bool parse_from_match_data(bxml_data_sptr result);
  static bxml_element *create_default_document_match_data();
  bxml_element *create_document_match_data();
  bxml_element *create_default_document_data();
};

class dbskr_tree_edit_params2 : public dborl_algo_params_base
{
public:

  dborl_parameter<float> scurve_sample_ds_;  // Sampling ds to reconstruct the scurve, def: 1.0
  dborl_parameter<float> scurve_interpolate_ds_;  // Interpolation ds to get densely interpolated versions of the scurves, def: 1.0 --> meaningful if localized_edit option is ON
  dborl_parameter<bool> localized_edit_; // improve elastic matching cost of scurves using the densely interpolated versions, def: false
  dborl_parameter<bool> elastic_splice_cost_; // use the elastic splice cost computation, def: false
  //: the defualt for this R parameter is 6.0f in original Sebastian implementation of shock matching
  dborl_parameter<double> curve_matching_R_; // curve matching R that weighs bending over stretching, default 6.0f
  
  //: the following option is only false when shock fragments are being matched, 
  //  to match shock graphs of simpled closed contours, i.e. shock trees, this option should always be TRUE
  dborl_parameter<bool> circular_ends_; // use circular completions at leaf branches during shock matching, def: true 

  dborl_parameter<bool> combined_edit_; // use combined edit cost during shock matching, def: false
  dborl_parameter<bool> coarse_edit_; // run coarse edit distance algorithm, def: false
  dborl_parameter<bool> use_approx_; // use cost approximation to skip processing the highly dissimilar shapes

  void get_edit_params_instance(dbskr_tree_edit_params& p) {
    p.elastic_splice_cost_ = elastic_splice_cost_();
    p.scurve_sample_ds_ = scurve_sample_ds_();
    p.scurve_interpolate_ds_ = scurve_interpolate_ds_();
    p.localized_edit_ = localized_edit_();
    p.curve_matching_R_ = curve_matching_R_();
    p.circular_ends_ = circular_ends_();
    p.combined_edit_ = combined_edit_();
    p.coarse_edit_ = coarse_edit_();
  }

  dbskr_tree_edit_params2() : dborl_algo_params_base("edit") { 
      scurve_sample_ds_.set_values(param_list_, "data", "edit_data-scurve_sample_ds", "[edit-data] Sampling ds to reconstruct the scurve", 1.0f, 1.0f);
      scurve_interpolate_ds_.set_values(param_list_, "data", "edit_data-scurve_interpolate_ds", "[edit-data] Interpolation ds to get densely interpolated versions of the scurves: meaningful if localized_edit option is ON", 1.0f, 1.0f);
      
      localized_edit_.set_values(param_list_, "data", "edit_data-localized_edit", "[edit-data] Localized edit, improves elastic matching cost of scurves using the densely interpolated versions", false, false);

      elastic_splice_cost_.set_values(param_list_, "data", "edit_data-elastic_splice_cost", "[edit-data] use the elastic splice cost computation", false, false);
      curve_matching_R_.set_values(param_list_, "data", "edit_data-curve_matching_R", "[edit-data] curve matching R that weighs bending over stretching", 6.0f, 6.0f);
      
      circular_ends_.set_values(param_list_, "data", "edit_data-circular_ends", "[edit-data] use circular completions at leaf branches during shock matching", true, true);
      combined_edit_.set_values(param_list_, "data", "edit_data-combined_edit", "[edit-data] use combined edit cost during shock matching", false, false);
      coarse_edit_.set_values(param_list_, "data", "edit_data-coarse_edit", "[edit-data] run coarse edit distance algorithm", false, false);
      use_approx_.set_values(param_list_, "data", "edit_data-use_approx", "[edit-data] use cost approximation to skip matching dissimilar shapes", true, true);

    }
 
};

#endif  //_dbskr_match_shock_params_h
