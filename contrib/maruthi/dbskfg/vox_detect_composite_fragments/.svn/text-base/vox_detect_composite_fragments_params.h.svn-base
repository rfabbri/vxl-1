
//:
// \file
// \brief parameter set for detection of model fragments within a query set
//
//
// \author Maruthi Narayanan (mn@lems.brown.edu)
// \date October 25th , 2010 
//      
// \verbatim
//   Modifications
//  
// \endverbatim

// update by 
//

#ifndef vox_detect_composite_fragments_params_h_
#define vox_detect_composite_fragments_params_h_

#include <dborl/algo/dborl_algo_params.h>

//: put all the additional params into this class, and add them 
//  to the parameter list in the constructor so that 
//  all the files related to the parameter set of the algorithm are 
//  generated automatically via the methods of the base class
//  no need to overwrite any of the methods in the base class thanks 
//  to the parameter list
class vox_detect_composite_fragments_params : public dborl_algo_params
{
public:
  //: Constructor
  vox_detect_composite_fragments_params(vcl_string algo_name);

  // MEMBER VARIABLES

  //: Save result to the object folder?
  dborl_parameter<bool> save_to_object_folder_;  
  
  //: Name of query to compare against
  dborl_parameter<vcl_string> query_object_name_;
  
  //: Passes the folder of the query composite graph list to match
  dborl_parameter<vcl_string> query_object_dir_;    

  //: Passes the assoc folder for input composite graph match files
  dborl_parameter<vcl_string> input_assoc_dir_;    

  //: extension of the input composite graph fragment match file
  dborl_parameter<vcl_string> input_cgraph_match_file_extension_;    

  //: minimum threshold for detection
  dborl_parameter<double> min_threshold_;

  //: maximum threshold for detection
  dborl_parameter<double> max_threshold_;

  //: threshold inc for detection
  dborl_parameter<double> threshold_inc_;

  //: top N model patches
  dborl_parameter<unsigned int> model_N_;

  //: top k model patches out each N
  dborl_parameter<unsigned int> top_k_;

  // if written to this folder as opposed to object folder then the 
  // fragment detections gets associated to the input object.
  // if nothing is written here, nothing gets associated
  dborl_parameter<vcl_string> output_cgraph_fragment_detect_folder_;  
  
};

#endif  //_vox_detect_composite_fragments_params_h
