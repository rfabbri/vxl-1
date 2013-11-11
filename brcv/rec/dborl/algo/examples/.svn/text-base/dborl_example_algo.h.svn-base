//:
// \file
// \brief A simple example algorithm to illustrate the use of dborl_algo_params 
//
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 10/25/07
//      
// \verbatim
//   Modifications
//  
// \endverbatim

//
//

#if !defined(_dborl_example_algo_h)
#define _dborl_example_algo_h

#include "dborl_example_algo_params_sptr.h"
#include <vcl_string.h>
#include <vcl_vector.h>

class dborl_example_algo 
{
public:
  dborl_example_algo(dborl_example_algo_params_sptr params) : params_(params) {}
  bool initialize();
  bool process();
  bool finalize();

protected:
  dborl_example_algo_params_sptr params_;
  vcl_vector<vcl_string> names_;
};

#endif  //_dborl_example_algo_h
