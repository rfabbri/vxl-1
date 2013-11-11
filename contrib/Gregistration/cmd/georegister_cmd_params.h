// This is contrib/Gregistration/cmd/georegister_cmd_params.h

//:
// \file
// \brief parameter set for georegister_cmd algorithm
//
// \author Anil Usumezbas (anil_usumezbas@brown.edu)
// \date Sep 14, 2011
//      
// \verbatim
//   Modifications
//  
// \endverbatim

#ifndef georegister_cmd_params_h_
#define georegister_cmd_params_h_

#include <dborl/algo/dborl_algo_params.h>

//: put all the additional params into this class, and add them 
//  to the parameter list in the constructor so that 
//  all the files related to the parameter set of the algorithm are 
//  generated automatically via the methods of the base class
//  no need to overwrite any of the methods in the base class thanks 
//  to the parameter list
class georegister_cmd_params: public dborl_algo_params
{
public:
  //: Constructor
  georegister_cmd_params(vcl_string algo_name);

  // MEMBER VARIABLES
  
  //: passes the folder of the input object
  dborl_parameter<vcl_string> input_object_dir_;  

  //: tag for third order color edge detector
  vcl_string tag_third_order_color_;

  //: tag for background modeling
  vcl_string tag_bg_modeling_;

  //: tag for foreground extraction
  vcl_string tag_fg_extraction_;
  
};

#endif  //_vox_compute_ishock_params_h
