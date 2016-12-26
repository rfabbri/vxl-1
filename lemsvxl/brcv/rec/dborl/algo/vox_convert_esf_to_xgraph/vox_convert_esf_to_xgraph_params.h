// This is rec/dborl/algo/vox_convert_esf_to_xgraph/vox_convert_esf_to_xgraph_params.h

#ifndef vox_convert_esf_to_xgraph_params_h_
#define vox_convert_esf_to_xgraph_params_h_

//:
// \file
// \brief parameter set for converting an esf to dbsksp_xshock_graph (xgraph)
//
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date Mar 19, 2010
//      
// \verbatim
//   Modifications
//  
// \endverbatim


#include <dborl/algo/dborl_algo_params.h>

//: put all the additional params into this class, and add them 
//  to the parameter list in the constructor so that 
//  all the files related to the parameter set of the algorithm are 
//  generated automatically via the methods of the base class
//  no need to overwrite any of the methods in the base class thanks 
//  to the parameter list
class vox_convert_esf_to_xgraph_params : public dborl_algo_params
{
public:

  //: Constructor
  vox_convert_esf_to_xgraph_params(vcl_string algo_name);

  // MEMBER VARIABLES
  
  //: Name of model to compare against
  dborl_parameter<vcl_string> object_name_;
  
  //: Passes the folder of the model shock graph
  dborl_parameter<vcl_string> object_dir_;    


  //: Save result to the object folder?
  dborl_parameter<bool> save_to_object_folder_;  

  // if written to this folder as opposed to object folder then the 
  // shock matches gets associated to the input object.
  // if nothing is written here, nothing gets associated
  dborl_parameter<vcl_string> output_folder_;  


  //: Extension of xgraph file
  dborl_parameter<vcl_string > xgraph_extension_;

  // Process parameters

  // Parameters for process to convert from sk2d shock graph to xgraph
  dborl_parameter<float > convert_sk2d_to_sksp_tol_;
};






#endif  //_vox_convert_esf_to_xgraph_params_h
