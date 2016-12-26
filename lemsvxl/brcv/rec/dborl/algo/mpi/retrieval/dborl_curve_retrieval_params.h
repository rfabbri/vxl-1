//:
// \file
// \brief An example parameter set inheriting from dborl_algo_params
//
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 03/26/08
//      
// \verbatim
//   Modifications
//  
// \endverbatim

//
//

#if !defined(_dborl_curve_retrieval_params_h)
#define _dborl_curve_retrieval_params_h

#include <dborl/algo/dborl_algo_params.h>

//: put all the additional params into this class, and add them to the parameter list in the constructor so that 
//  all the files related to the parameter set of the algorithm are generated automatically via the methods of the base class
//  no need to overwrite any of the methods in the base class thanks to the parameter list
class dborl_curve_retrieval_params : public dborl_algo_params
{
public:
  
  //: parameters from the example algo in ORL documentations for parameter handling
  dborl_parameter<vcl_string> db_index_;
  dborl_parameter<vcl_string> output_folder_;
  //: will be concatanated at the end of the object names to find esf files e.g. 0.5-0.1.esf
  dborl_parameter<vcl_string> input_ext_;

  dborl_parameter<float> matching_R_;
  dborl_parameter<float> line_fit_rms_;  // only one of these should be true
  dborl_parameter<int> DP_template_size_;
  dborl_parameter<bool> use_rho_;  // use similarity values to find nearest neighbors
  dborl_parameter<float> rho_;
  dborl_parameter<int> k_;   // if use_rho is false pick top k_ neighbors

  dborl_parameter<bool> normalize_; // if true normalize using lengths of the input curves
  
  dborl_curve_retrieval_params() : dborl_algo_params("dborl_curve_retrieval") { 

    output_folder_.set_values(param_list_, "io", "output_folder", "output folder", "", "/vision/projects/kimia/categorization/99-db/orl-exps/shape_retrieval_output/", -1, dborl_parameter_system_info::OUTPUT_DIRECTORY);
    db_index_.set_values(param_list_, "io", "db_index", "path of index file", "", "/vision/projects/kimia/categorization/99-db/orl-exps/99-db-flat-index.xml", 0, dborl_parameter_system_info::NOT_DEFINED, "flat_image", dborl_parameter_type_info::FILEASSOC); 
    input_ext_.set_values(param_list_, "data", "input_ext", "will be concatanated at the end of the object names to find con files e.g. 0.5-0.1.con", ".con", ".con");

    matching_R_.set_values(param_list_, "data", "matching_R", "matching_R", 10.0f, 10.0f);
    line_fit_rms_.set_values(param_list_, "data", "line_fit_rms", "line_fit_rms, set 0 if not required", 0.05f, 0.05f);
    DP_template_size_.set_values(param_list_, "data", "DP_template_size", "DP_template_size (1, 3, 5 or 11)", 3, 3);
    use_rho_.set_values(param_list_, "data", "use_rho", "use similarity threshold to pick nearest neighbors, otherwise use k?", false, false);
    rho_.set_values(param_list_, "data", "rho", "similarity threshold to pick nearest neighbors", 0.5f, 0.5f);
    k_.set_values(param_list_, "data", "k", "number of nearest neighbors", 5, 5);
    normalize_.set_values(param_list_, "data", "normalize", "normalize cost?", true, true);

  }
};

#endif  //_dborl_curve_retrieval_params_h
