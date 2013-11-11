//:
// \file
// \brief Parameters for the archaeology project on VOX
//
// \author Eli Fine
// \date 09/29/2008
//      
// \verbatim
//   Modifications
//  
// \endverbatim

//
//

#ifndef dborl_archaeology_algo_params_h_
#define dborl_archaeology_algo_params_h_

#include <dborl/algo/dborl_algo_params.h>
//#include <dborl/algo/dborl_utilities.h>

//: put all the additional params into this class, and add them to the parameter list in the constructor so that 
//  all the files related to the parameter set of the algorithm are generated automatically via the methods of the base class
//  no need to overwrite any of the methods in the base class thanks to the parameter list
class dborl_archaeology_algo_params : public dborl_algo_params
{
public:
  vcl_string algo_abbreviation_;
 
  //io parameters  
  dborl_parameter<vcl_string> input_dir_;    // passes the folder of the input object
  dborl_parameter<vcl_string> input_name_;
  dborl_parameter<vcl_string> input_extension_;     // extension of the input file
  dborl_parameter<vcl_string> output_dir_;          // if written to this folder as opposed to object folder then the edge map gets associated to the input object
                                                    // if nothing is written here, nothing gets associated

  //optional parameters
  dborl_parameter<float> TQUAN;
  dborl_parameter<int> scale;
  dborl_parameter<float> mthresh;
  //dborl_parameter<int> intensity_threshold_;
  //dborl_parameter<int> len_threshold_;
  //dborl_parameter<bool> bSub_con_;
  //dborl_parameter<bool> bSmart_;
  //dborl_parameter<double> c_ds_; 

  //: constructor
  dborl_archaeology_algo_params (vcl_string algo_name) : dborl_algo_params(algo_name) 
  { 
    algo_abbreviation_ = "dborl_archaeology_algo";

    input_dir_.set_values (param_list_, "io", "input_dir", "input object", "", 
      "/vision/projects/kimia/categorization/99-database/pgms/", //default directory
      0,  
      dborl_parameter_system_info::INPUT_OBJECT_DIR);

    input_name_.set_values (param_list_, "io", "input_name", "Input object name", "dummy", "dummy", 
      0,   
      dborl_parameter_system_info::INPUT_OBJECT_STRING_ID);

    input_extension_.set_values (param_list_, "io", "input_extension", "input file extension", ".jpg", ".jpg");

    output_dir_.set_values(param_list_,  "io",  "output_folder",  "output folder to store results", "/vision/projects/kimia/categorization/99-database/output/", 
      "/vision/projects/kimia/categorization/99-database/output/", 
      0,   // associate to 0th input object
      dborl_parameter_system_info::OUTPUT_FILE, "dborl_archaeology_algo_result", dborl_parameter_type_info::FILEASSOC);

    TQUAN.set_values(param_list_, "opt", "opt_TQUAN", "Color Quantization", 0.0f, 0.0f);
    scale.set_values(param_list_, "opt", "opt_scale", "Number of scales", 0, 0);
    mthresh.set_values(param_list_, "opt", "opt_mthresh", "Merge Threshold", 0.4f, 0.4f);
    //len_threshold_.set_values(param_list_, "opt", "opt_len_threshold", "length threshold", 20, 20);
    //bSub_con_.set_values(param_list_,"opt","opt_bSub_con","subsample contour", true, true);
    //bSmart_.set_values(param_list_,"opt","opt_bSmart","subsample smartly", true, true);
    //c_ds_.set_values(param_list_, "opt", "opt_c_ds", "subsampling step size", 1, 1);
  }

};

#endif  //dborl_archaeology_algo_params_h_



