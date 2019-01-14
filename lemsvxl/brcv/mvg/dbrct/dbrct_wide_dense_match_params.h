// This is brcv/mvl/dbrct_wide_dense_match_params.h

#ifndef _dbrct_wide_dense_match_params_h_
#define _dbrct_wide_dense_match_params_h_
//:
// \file
// \brief This class manages the parameters for dbrct_wide_dense_match
// \author Vishal Jain, (vj@lems.brown.edu)
// \date 08/31/04

#include <iostream>
#include <vector>
#include <vsrl/vsrl_parameters.h>

class dbrct_wide_dense_match_params
{

 public:
         dbrct_wide_dense_match_params();
         dbrct_wide_dense_match_params(int no_of_levels, int downsampling);
         ~dbrct_wide_dense_match_params();
         int levels;
        std::vector<vsrl_parameters> paramslist;
         int coarsestscale;


};

#endif
