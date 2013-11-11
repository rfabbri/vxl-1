//:
// \file
// \brief The parameter class for dborl_extract_contour inherits from dborl_algo_params
//        handles all the io related to input.xml, status.xml, perf.xml, params.xml thanks to inheritance
//        all other parameters needed for dborl_extract_contour is defined here and added to param_list_ of base class in the constructor
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

#if !defined(_dborl_extract_contour_params_h)
#define _dborl_extract_contour_params_h

#include <dborl/algo/dborl_algo_params.h>

//: put all the additional params into this class, and add them to the parameter list in the constructor so that 
//  all the files related to the parameter set of the algorithm are generated automatically via the methods of the base class
//  no need to overwrite any of the methods in the base class thanks to the parameter list
class dborl_extract_contour_params : public dborl_algo_params
{
public:

  dborl_parameter<float> sigma;
  dborl_parameter<int> nsteps;
  dborl_parameter<float> curvature_smoothing_beta;  
  dborl_parameter<int> intensity_threshold;
  dborl_parameter<bool> save_to_output_folder;       // if false saves each con file to the object's folder
  dborl_parameter<vcl_string> index_filename;
  dborl_parameter<vcl_string> output_directory;
  dborl_parameter<vcl_string> image_extension;       // extensions of images to load, assumption: <obj name><.ext> is found in the object directory
  
  dborl_extract_contour_params(vcl_string algo_name) : dborl_algo_params(algo_name) { 
    sigma.set_values(param_list_, "data", "sigma", "sigma for smoothing", 1.0f, 1.0f); 
    nsteps.set_values(param_list_, "data", "nsteps", "number smoothing steps", 1, 1); 
    curvature_smoothing_beta.set_values(param_list_, "data", "beta", "curvature smoothing beta", 0.3f, 0.3f); 
    intensity_threshold.set_values(param_list_, "data", "intensity_threshold", "intensity threshold", -1, -1); // set automaticly if -1
    image_extension.set_values(param_list_, "data", "image_extension", "string to append to object name to load its image for tracing", "", ".png");  // can be anything to append to the object name e.g. _mask.jpg
    save_to_output_folder.set_values(param_list_, "io_data", "save_to_output_folder", "if false saves each con to objects' input folders", true, true);
    index_filename.set_values(param_list_, "io_data", "index_filename", "path of index file", "", "", 0, dborl_parameter_system_info::NOT_DEFINED, "flat_image", dborl_parameter_type_info::FILEASSOC); 
    output_directory.set_values(param_list_, "io_data", "output_directory", "output directory", "", "", -1, dborl_parameter_system_info::OUTPUT_DIRECTORY, "");
  }
};

#endif // _dborl_extract_contour_params_h)
