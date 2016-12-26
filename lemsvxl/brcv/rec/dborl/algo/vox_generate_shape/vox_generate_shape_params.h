
#ifndef vox_generate_shape_params_h_
#define vox_generate_shape_params_h_

//:
// \file
// \brief parameter set for generate_shape algorithm
//
// \author Yuliang Guo 
// \date Sep 3, 2014
//      
// \verbatim
//   Modifications
//  
// \endverbatim


#include <dborl/algo/dborl_algo_params.h>


//==============================================================================
// vox_generate_shape_params
//==============================================================================

//: Parameters of generate_shape algorithm
class vox_generate_shape_params : public dborl_algo_params
{
public:
  //: Constructor
  vox_generate_shape_params(vcl_string algo_name);

  // Parameter parsers ---------------------------------------------------------

  vcl_string get_xgraph_file() const;

  vcl_string get_xgraph_geom_file() const;
  vcl_string get_xgraph_geom_param_file() const;

  vcl_string get_output_folder() const; // location of all output files

  vcl_string get_xgraph_prototype_name() const; //> (only) name of the xgraph prototype file

  int num_samples() {return num_samples_();};
//  vcl_string get_appearance_prototype_name() const; //> 
  // MEMBER VARIABLES ----------------------------------------------------------

  //>> Object <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

  //: passes the folder of the input object
  dborl_parameter<vcl_string> input_object_dir_;    

  //>> Trained model for xgraph
  //: Folder containing the object shock graph
  dborl_parameter<vcl_string > input_xgraph_folder_;

  //: xgraph file name
  dborl_parameter<vcl_string > input_xgraph_prototype_filename_;

  //>> xgraph geom model
  
  //: Object categery of input shock graph
  dborl_parameter<vcl_string > input_model_category_;

  //: xgraph geometric model filename (.xml)
  dborl_parameter<vcl_string > input_xgraph_geom_filename_;

  //: xgraph geometric model filename (.xml)
  dborl_parameter<vcl_string > input_xgraph_geom_param_filename_;

  //: Save result to the object folder?
  dborl_parameter<bool> save_to_object_folder_;

  dborl_parameter<vcl_string> output_object_folder_;  

  //: Number of Samples intended to generate. Mention that some one just fail.
  dborl_parameter<int> num_samples_;

  //: tag for this algorithm
  vcl_string algo_abbreviation_;

};

#endif  //_vox_generate_shape_params_h
