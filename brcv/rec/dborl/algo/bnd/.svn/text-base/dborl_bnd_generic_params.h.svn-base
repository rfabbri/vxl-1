//:
// \file
// \brief parameter set inheriting from dborl_algo_params
//
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 05/08/08
//      
// \verbatim
//   Modifications
//  
// \endverbatim

//
//

#if !defined(_dborl_bnd_generic_params_h)
#define _dborl_bnd_generic_params_h

#include <dborl/algo/dborl_algo_params.h>
#include <dborl/algo/dborl_utilities.h>

#include <dbdet/pro/dbdet_generic_linker_process.h>

//: put all the additional params into this class, and add them to the parameter list in the constructor so that 
//  all the files related to the parameter set of the algorithm are generated automatically via the methods of the base class
//  no need to overwrite any of the methods in the base class thanks to the parameter list
class dborl_bnd_generic_params : public dborl_algo_params
{
public:
  dborl_parameter<bool> save_bnds_;  
  dborl_parameter<bool> save_to_object_folder_;  // if both true deletes random segments and adds random line segments of total length perc_add_*total_length
  
  dborl_parameter<vcl_string> input_object_name_;
  dborl_parameter<vcl_string> input_object_dir_;    // passes the folder of the input object
  dborl_parameter<vcl_string> input_extension_;     // extension of the input image


  dborl_parameter<bool> input_edge_from_object_folder_;
  dborl_parameter<vcl_string> input_edge_folder_;

  dborl_parameter<vcl_string> output_extension_;

  dborl_parameter<vcl_string> output_bnd_folder_;  // if written to this folder as opposed to object folder then the edge map gets associated to the input object
                                                    // if nothing is written here, nothing gets associated

  vcl_string algo_abbreviation_;

  dborl_bnd_generic_params(vcl_string algo_name) : dborl_algo_params(algo_name) { 

    algo_abbreviation_ = "bgl";

    save_bnds_.set_values(param_list_, "io", "save_bnds", "save_bnds?", true, true);
    save_to_object_folder_.set_values(param_list_, "io", "save_to_object_folder", "save to object folder?", false, false);
  
    input_object_name_.set_values(param_list_, "io", "input_object_name", "input_object name", "dummy", "dummy", 
      0,   // for 0th input object
      dborl_parameter_system_info::INPUT_OBJECT_STRING_ID);

    input_object_dir_.set_values(param_list_, "io", "input_object_dir", "input object", "", 
      "/vision/images/misc/object/", 
      0,   // for 0th input object
      dborl_parameter_system_info::INPUT_OBJECT_DIR); 

    input_extension_.set_values(param_list_, "io", "input_extension", "input extension", ".edg", ".edg");
    
    input_edge_from_object_folder_.set_values(param_list_, "io", "input_edge_from_object_folder", "read .edg from obj folder?", false, false);
    input_edge_folder_.set_values(param_list_,  "io",  "input_edge_folder",  "input folder to read edge map (associated)", "", 
      "/vision/projects/kimia/categorization/output/", 
      0,   // associate to 0th input object
      dborl_parameter_system_info::NOT_DEFINED, "edge_map", dborl_parameter_type_info::FILEASSOC);
    
    output_extension_.set_values(param_list_, "io", "output_extension", "output extension", ".cem", ".cem");
    output_bnd_folder_.set_values(param_list_,  "io",  "output_bnd_folder",  "output folder to write bnd map", "", 
      "/vision/projects/kimia/categorization/output/", 
      0,   // associate to 0th input object
      dborl_parameter_system_info::OUTPUT_FILE, "boundary_set", dborl_parameter_type_info::FILEASSOC);

    //: add the parameters of the dbdet_third_order_edge_detector_process
    dbdet_generic_linker_process pro;
    vcl_vector<bpro1_param*> pars = pro.parameters()->get_param_list();
    for (unsigned i = 0; i < pars.size(); i++) {
        param_list_.push_back(
            convert_parameter_from_bpro1(algo_abbreviation_, 
                                         "["+algo_abbreviation_ +"]",
                                         pars[i]));
    }
    
  }

};

#endif  //_dborl_bnd_generic_params_h
