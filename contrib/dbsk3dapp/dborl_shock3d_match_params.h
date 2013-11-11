//:
// \file
// \brief Parameters for the 3D shock matching on VOX
//
// \author Ming-Ching Chang
// \date 08/06/2008
//      
// \verbatim
//   Modifications
//  
// \endverbatim

//
//

#ifndef dborl_shock3d_match_params_h_
#define dborl_shock3d_match_params_h_

#include <dborl/algo/dborl_algo_params.h>
#include <dborl/algo/dborl_utilities.h>

//: put all the additional params into this class, and add them to the parameter list in the constructor so that 
//  all the files related to the parameter set of the algorithm are generated automatically via the methods of the base class
//  no need to overwrite any of the methods in the base class thanks to the parameter list
class dborl_shock3d_match_params : public dborl_algo_params
{
public:
  vcl_string algo_abbreviation_;
  
  //io parameters  
  dborl_parameter<vcl_string> input_1_dir_;    // passes the folder of the input object
  dborl_parameter<vcl_string> input_1_name_;
  dborl_parameter<vcl_string> input_2_dir_;    // passes the folder of the input object
  dborl_parameter<vcl_string> input_2_name_;

  dborl_parameter<vcl_string> input_extension_;     // extension of the input file

  dborl_parameter<vcl_string> output_dir_;          // if written to this folder as opposed to object folder then the edge map gets associated to the input object
                                                    // if nothing is written here, nothing gets associated

  //option parameters
  dborl_parameter<int> smatch_;
  dborl_parameter<int> regrd_;


  //: constructor
  dborl_shock3d_match_params (vcl_string algo_name) : dborl_algo_params(algo_name) 
  { 
    algo_abbreviation_ = "shock3d_match";

    input_1_dir_.set_values (param_list_, "io", "input_1_dir", "input object", "", 
      "/vision/projects/kimia/shock3d/vox/db/", //default directory
      0,   // for 1th input object
      dborl_parameter_system_info::INPUT_OBJECT_DIR);

    input_1_name_.set_values (param_list_, "io", "input_1_name", "Input object 1 name", "dummy", "dummy", 
      0,   // for 1th input object
      dborl_parameter_system_info::INPUT_OBJECT_STRING_ID);

    input_2_dir_.set_values (param_list_, "io", "input_2_dir", "input object", "", 
      "/vision/projects/kimia/shock3d/vox/db/", //default directory
      1,   // for 2nd input object
      dborl_parameter_system_info::INPUT_OBJECT_DIR); 

    input_2_name_.set_values(param_list_, "io", "input_2_name", "Input object 2 name", "dummy", "dummy", 
      1,   // for 2nd input object
      dborl_parameter_system_info::INPUT_OBJECT_STRING_ID);

    input_extension_.set_values (param_list_, "io", "input_extension", "input file extension", ".cms", ".cms");

    output_dir_.set_values(param_list_,  "io",  "output_folder",  "output folder to store results", "/vision/stratch/ming/output/", 
      "/vision/stratch/ming/output/", 
      0,   // associate to 0th input object
      dborl_parameter_system_info::OUTPUT_FILE, "shock3d_match_result", dborl_parameter_type_info::FILEASSOC);
    
    //-smatch 2: shock hypergraph matching using curve length as compatibility.
    //-smatch 3: shock hypergraph matching using curve sum radius over length as compatibility.
    //-smatch 4: shock hypergraph matching using D.P. curve distance as compatibility.
    //-smatch 5: shock hypergraph matching using D.P. shock curve distance as compatibility.
    //-smatch 6: option 5 plus integration of the global (R, T) in node compatibility.
    //-smatch 7: option 3 plus integration of the global (R, T) in node compatibility.
    smatch_.set_values(param_list_, "opt", "opt_smatch", "shock match option", 6, 6);

    //-regrd: after matching, register object1 rigidly to object2
    smatch_.set_values(param_list_, "opt", "opt_regrd", "after matching, register object1 rigidly to object2", 1, 1);
    
  }

};

#endif  //dborl_shock3d_match_params_h_
