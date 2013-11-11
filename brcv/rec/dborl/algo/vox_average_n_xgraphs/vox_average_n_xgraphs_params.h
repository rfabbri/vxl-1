// This is rec/dborl/algo/vox_average_n_xgraphs/vox_average_n_xgraphs_params.h

#ifndef vox_average_n_xgraphs_params_h_
#define vox_average_n_xgraphs_params_h_

//:
// \file
// \brief parameter set for averaging N xgraphs
//
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date Mar 8, 2010
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
class vox_average_n_xgraphs_params : public dborl_algo_params
{
public:

  //: Constructor
  vox_average_n_xgraphs_params(vcl_string algo_name);

  // MEMBER VARIABLES
  
  //: Name of model to compare against
  dborl_parameter<vcl_string> dataset_object_name_;
  
  //: Passes the folder of the model shock graph
  dborl_parameter<vcl_string> dataset_object_dir_;    

  // if written to this folder as opposed to object folder then the 
  // shock matches gets associated to the input object.
  // if nothing is written here, nothing gets associated
  dborl_parameter<vcl_string> output_average_xgraph_folder_;  

  // parameter for the index file
  // Is a flat image database 
  dborl_parameter<vcl_string> index_file_;

  ////: Number of xgraphs to compute average from
  //dborl_parameter<unsigned > num_xgraphs_to_average_;

  //: Start index (in the dataset) of the range of xgraph to average
  dborl_parameter<unsigned > dataset_start_index_;

  //: End index (in the dataset) of the range of xgraph to average
  dborl_parameter<unsigned > dataset_end_index_;


  //: Averaging method
  // 0: curve shortening
  // 1: iterative merging
  // 2: best-exemplar
  // 3: use existing xgraph in the object
  // 4: choose a specific exemplar
  dborl_parameter<bool > use_curve_shortening_;
  dborl_parameter<bool > use_iterative_merging_;
  dborl_parameter<bool > use_best_exemplar_;
  dborl_parameter<bool > use_existing_xgraph_in_object_folder_;
  dborl_parameter<bool > use_specific_exemplar_;

  //: extension of existing xgraph (only apply when use_existing_xgraph_in_object_folder = true)
  dborl_parameter<vcl_string > extension_of_existing_xgraph_;

  //: Exemplar to choose as average (only apply when use_specific_exemplar_ = true)
  dborl_parameter<unsigned > index_of_specific_exemplar_;


  //: Run gradient descent at the end after the average has been computed?
  dborl_parameter<bool > run_gradient_descent_at_the_end_;

  

  // Process parameters

  // Parameters for process to convert from sk2d shock graph to xgraph
  dborl_parameter<float > convert_sk2d_to_sksp_tol_;

  // Parameters for averaging two xgraphs
  dborl_parameter<float > average_xgraph_scurve_matching_R_;
  dborl_parameter<float > average_xgraph_scurve_sample_ds_;
};






#endif  //_vox_average_n_xgraphs_params_h
