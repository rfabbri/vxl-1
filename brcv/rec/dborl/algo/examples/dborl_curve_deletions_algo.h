//:
// \file
// \brief An algorithm to run random segment deletion method on .con files 
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

#if !defined(_dborl_curve_deletions_algo_h)
#define _dborl_curve_deletions_algo_h

#include "dborl_curve_deletions_algo_params_sptr.h"
#include <vcl_string.h>
#include <vcl_vector.h>
#include <dborl/dborl_index_sptr.h>
#include <dborl/dborl_index_node_sptr.h>

class dborl_curve_deletions_algo 
{
public:
  dborl_curve_deletions_algo(dborl_curve_deletions_algo_params_sptr params) : params_(params) {}
  bool initialize();
  bool process();
  bool finalize();
  bool parse_index(vcl_string index_file);

protected:
  dborl_curve_deletions_algo_params_sptr params_;

  dborl_index_sptr ind_;
  dborl_index_node_sptr root_;
};

#endif  //_dborl_curve_deletions_algo_h
