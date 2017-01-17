
//:
// \file
// \brief parameter set for matching of shock graphs
//
// \author Maruthi Narayanan (mn@lems.brown.edu)
// \date June 18, 2009
//      
// \verbatim
//   Modifications
//  
// \endverbatim

// update by 
//

#ifndef vox_match_shock_graphs_params_h_
#define vox_match_shock_graphs_params_h_

#include <dborl/algo/dborl_algo_params.h>
#include <dbskr/algo/io/dbskr_match_shock_params.h>

//: put all the additional params into this class, and add them 
//  to the parameter list in the constructor so that 
//  all the files related to the parameter set of the algorithm are 
//  generated automatically via the methods of the base class
//  no need to overwrite any of the methods in the base class thanks 
//  to the parameter list
class vox_match_shock_graphs_params : public dborl_algo_params
{
public:

  //: Constructor
  vox_match_shock_graphs_params(vcl_string algo_name);

  // MEMBER VARIABLES
  
  // Holds all tree edit params
  dbskr_tree_edit_params2 edit_params_;

  // Should pruning be performed
  dborl_parameter<bool>  prune_tree_;

  // Shock graph pruning threshold 
  dborl_parameter<double> prune_thres_;

  // If doing this normalize by length instead of total splice cost
  dborl_parameter<bool> norm_reconstruct_;

  //: Turn on/off mirror matching
  dborl_parameter<bool> perform_mirror_matching_;

  //: Save result to the object folder?
  dborl_parameter<bool> save_to_object_folder_;  
  
  //: Name of model to compare against
  dborl_parameter<vcl_string> model_object_name_;
  
  //: Passes the folder of the model shock graph
  dborl_parameter<vcl_string> model_object_dir_;    

  //: Name of query to compare against
  dborl_parameter<vcl_string> query_object_name_;
  
  //: Passes the folder of the query shock graph
  dborl_parameter<vcl_string> query_object_dir_;    

  //: Passes the folder of where the query objects can live
  dborl_parameter<vcl_string> query_assoc_dir_;

  //: extension of the folder where shock patches can be found
  dborl_parameter<vcl_string> input_shock_patch_extension_;     

  // Decide if writing out normalized or unnormalized costs
  dborl_parameter<bool> write_matching_norm_;  

  //: extension of the output for output shock patches
  dborl_parameter<vcl_string> output_match_extension_;     

  // if written to this folder as opposed to object folder then the 
  // shock matches gets associated to the input object.
  // if nothing is written here, nothing gets associated
  dborl_parameter<vcl_string> output_shock_match_folder_;  

  // if written to this folder as opposed to object folder then the 
  // shock matches gets associated to the input object.
  // if nothing is written here, nothing gets associated
  dborl_parameter<vcl_string> output_shock_patch_match_folder_;  

  //parameter for the index file
  //Is a flat image database 
  dborl_parameter<vcl_string> index_filename_;

};

#endif  //_vox_match_shock_graphs_params_h
