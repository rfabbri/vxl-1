// This is rec/dborl/algo/vox_average_two_xgraphs/vox_average_two_xgraphs_params.h

#ifndef vox_average_two_xgraphs_params_h_
#define vox_average_two_xgraphs_params_h_

//:
// \file
// \brief parameter set for averaging two xgraphs
//
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date Mar 2, 2010
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
class vox_average_two_xgraphs_params : public dborl_algo_params
{
public:

  //: Constructor
  vox_average_two_xgraphs_params(vcl_string algo_name);

  // MEMBER VARIABLES
  
  //: Name of model to compare against
  dborl_parameter<vcl_string> model_object_name_;
  
  //: Passes the folder of the model shock graph
  dborl_parameter<vcl_string> model_object_dir_;    

  //: Name of query to compare against
  dborl_parameter<vcl_string> query_object_name_;
  
  //: Passes the folder of the query shock graph
  dborl_parameter<vcl_string> query_object_dir_;    

  
  // if written to this folder as opposed to object folder then the 
  // shock matches gets associated to the input object.
  // if nothing is written here, nothing gets associated
  dborl_parameter<vcl_string> output_average_xgraph_folder_;  

  ////parameter for the index file
  ////Is a flat image database 
  //dborl_parameter<vcl_string> index_filename_;


  // Process parameters

  // Parameters for process to convert from sk2d shock graph to xgraph
  dborl_parameter<float > convert_sk2d_to_sksp_tol_;

  // Parameters for averaging two xgraphs
  dborl_parameter<float > average_xgraph_weight1_;
  dborl_parameter<float > average_xgraph_weight2_;
  dborl_parameter<float > average_xgraph_scurve_matching_R_;
  dborl_parameter<float > average_xgraph_scurve_sample_ds_;
};






#endif  //_vox_average_two_xgraphs_params_h
