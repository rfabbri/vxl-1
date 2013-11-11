//:
// \file
// \brief A base class with a simple list to hold the parameters of the algorithms along with their attributes such as param name, group, description etc
//
// \author Ozge C Ozcanli (Brown)
// \date   December 20, 2007
//
// \verbatim
//  Modifications
// \endverbatim
//

#if !defined(_dborl_algo_params_base_h)
#define _dborl_algo_params_base_h

#include "dborl_parameter.h"

#include <vbl/vbl_ref_count.h>
//#include <vgl/vgl_point_2d.h>
//#include <dborl/dborl_evaluation.h>

//: the simplest base class with only a param list
class dborl_algo_params_base : public vbl_ref_count
{
public:

  vcl_string algo_name_;

  //: constructor
  dborl_algo_params_base(vcl_string algo_name) : algo_name_(algo_name) {};

  virtual ~dborl_algo_params_base() { param_list_.clear(); // no allocation with new for the pointers so no need to call delete
                                    }

  vcl_string output_file_postfix();
  vcl_string output_file_postfix(vcl_string replacement_algo_name);
 
  //: if any additional params are defined in the deriving classes, they should be added to the following list in the constructor
  vcl_vector<dborl_parameter_base*> param_list_;
};

#endif  //_dborl_algo_params_base_h
