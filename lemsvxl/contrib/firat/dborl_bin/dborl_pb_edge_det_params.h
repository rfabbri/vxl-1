//:
// \file
// \brief Parameters for the shock matcher on VOX
//
// \author Firat Kalaycilar
// \date 
//      
// \verbatim
//   
//  
// \endverbatim

//
//

#ifndef dborl_pb_edge_det_params_h_
#define dborl_pb_edge_det_params_h_

#include <dborl/algo/dborl_algo_params.h>
#include <dborl/algo/dborl_utilities.h>

//: put all the additional params into this class, and add them to the parameter list in the constructor so that 
//  all the files related to the parameter set of the algorithm are generated automatically via the methods of the base class
//  no need to overwrite any of the methods in the base class thanks to the parameter list
class dborl_pb_edge_det_params : public dborl_algo_params
{
public:
  vcl_string algo_abbreviation_;
  
  //io parameters  
  dborl_parameter<vcl_string> input_dir_;
  dborl_parameter<vcl_string> input_name_;    // passes the name of the input object
    
  dborl_parameter<vcl_string> output_dir_;          // if written to this folder as opposed to object folder then the edge map gets associated to the input object
                                                    // if nothing is written here, nothing gets associated

  //optional parameters
  
  //parameter for the index file
  dborl_parameter<vcl_string> index_filename_;

  //: constructor
  dborl_pb_edge_det_params(vcl_string algo_name);
};

#endif  

