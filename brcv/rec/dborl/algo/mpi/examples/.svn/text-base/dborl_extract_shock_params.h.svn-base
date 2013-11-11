//:
// \file
// \brief The parameter class for dborl_extract_shock inherits from dborl_algo_params
//        handles all the io related to input.xml, status.xml, perf.xml, params.xml thanks to inheritance
//        all other parameters needed for dborl_extract_shock is defined here and added to param_list_ of base class in the constructor
//        
//
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 01/14/08
//      
// \verbatim
//   Modifications
//  
// \endverbatim

//
//

#if !defined(_dborl_extract_shock_params_h)
#define _dborl_extract_shock_params_h

#include <dborl/algo/dborl_algo_params.h>
#include <dbskr/algo/io/dbskr_extract_shock_params.h>

//: put all the additional params into this class, and add them to the parameter list in the constructor so that 
//  all the files related to the parameter set of the algorithm are generated automatically via the methods of the base class
//  no need to overwrite any of the methods in the base class thanks to the parameter list
class dborl_extract_shock_params : public dborl_algo_params
{
public:

   dborl_parameter<bool> save_to_output_folder;       // if false saves each con file to the object's folder
                                                     // if saving to output folder then saves VOX style, i.e. creates a new directory for each object
 
  dborl_parameter<vcl_string> index_filename;
  dborl_parameter<vcl_string> output_directory;
  
  dborl_parameter<bool> use_assoc_contours;   // use the associated contour files, if false assumes that the input files exist in input object directory 
  dborl_parameter<vcl_string> input_contour_dir;   // if use_assoc_contours is true, this should be a valid path
  
  dborl_parameter<bool> input_simple_closed_contours;
  dborl_parameter<vcl_string> input_extension;       // extensions of input files to load, assumption: <obj name><.ext> is found in the object directory
  dborl_parameter<vcl_string> output_extension;       // extensions of input files to load, assumption: <obj name><.ext> is found in the object directory

  dbskr_extract_shock_params2 extract_shock_;
  
  dborl_extract_shock_params(vcl_string algo_name) : dborl_algo_params(algo_name) { 
    use_assoc_contours.set_values(param_list_, "io", "use_assoc_contours", "use assoc contours?", true, true);
    input_contour_dir.set_values(param_list_, "io", 
      "input_contour_dir", 
      "input assoc contour dir", "", 
      "/vision/projects/kimia/categorization/99-db/orl-exps/cems/", 0, 
      dborl_parameter_system_info::NOT_DEFINED, "boundary_set", dborl_parameter_type_info::FILEASSOC);
    
    input_simple_closed_contours.set_values(param_list_, "io", "input_simple_closed_contours", "input_simple_closed_contours?", false, false);
    input_extension.set_values(param_list_, "io", "input_extension", "input extension", "", ".cem");  // can be anything to append to the object name e.g. -0.4-0.3.cem
    output_extension.set_values(param_list_, "io", "output_extension", "output extension", "", ".esf");  // can be anything to append to the object name e.g. -0.4-0.3.esf
    
    index_filename.set_values(param_list_, "io", "index_filename", "index file", "", 
      "/vision/projects/kimia/categorization/99-db/orl-exps/99-db-flat-index.xml", 
      0, dborl_parameter_system_info::NOT_DEFINED, "flat_image", dborl_parameter_type_info::FILEASSOC); 

    save_to_output_folder.set_values(param_list_, "io", "save_to_output_folder", "save_to_output_folder?", true, true);
    output_directory.set_values(param_list_, "io", 
      "output_directory", 
      "output directory", "", 
      "/vision/projects/kimia/categorization/99-db/orl-exps/shocks/", 0, dborl_parameter_system_info::OUTPUT_FILE, "shock_set", dborl_parameter_type_info::FILEASSOC);
    
    extract_shock_.algo_name_ = "s_e";
    add_params(extract_shock_);
  }
};

#endif // _dborl_extract_shock_params_h)
